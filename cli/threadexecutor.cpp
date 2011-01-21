/*
 * Cppcheck - A tool for static C/C++ code analysis
 * Copyright (C) 2007-2010 Daniel Marjamäki and Cppcheck team.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef CPPCHECK_BUILD_USE_CONFIGURATION_HEADER
 #include "cli-config.h"   // System settings from the build configuration
#endif

#include "threadexecutor.h"
#include "cppcheck.h"
#include <iostream>
#include <algorithm>
#if (defined(__GNUC__) || defined(__sun)) && !defined(__MINGW32__)
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <errno.h>
#endif

ThreadExecutor::ThreadExecutor(const std::vector<std::string> &filenames, const Settings &settings, ErrorLogger &errorLogger)
    : _filenames(filenames), _settings(settings), _errorLogger(errorLogger), _fileCount(0)
{
#if (defined(__GNUC__) || defined(__sun)) && !defined(__MINGW32__)
    _pipe[0] = _pipe[1] = 0;
#endif
}

ThreadExecutor::~ThreadExecutor()
{
    //dtor
}

void ThreadExecutor::addFileContent(const std::string &path, const std::string &content)
{
    _fileContents[ path ] = content;
}

///////////////////////////////////////////////////////////////////////////////
////// This code is for __GNUC__ and __sun only ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if (defined(__GNUC__) || defined(__sun)) && !defined(__MINGW32__)

int ThreadExecutor::handleRead(unsigned int &result)
{
    char type = 0;
    if (read(_pipe[0], &type, 1) <= 0)
    {
        if (errno == EAGAIN)
            return 0;

        return -1;
    }

    if (type != '1' && type != '2' && type != '3')
    {
        std::cerr << "#### You found a bug from cppcheck.\nThreadExecutor::handleRead error, type was:" << type << std::endl;
        exit(0);
    }

    unsigned int len = 0;
    if (read(_pipe[0], &len, sizeof(len)) <= 0)
    {
        std::cerr << "#### You found a bug from cppcheck.\nThreadExecutor::handleRead error, type was:" << type << std::endl;
        exit(0);
    }

    char *buf = new char[len];
    if (read(_pipe[0], buf, len) <= 0)
    {
        std::cerr << "#### You found a bug from cppcheck.\nThreadExecutor::handleRead error, type was:" << type << std::endl;
        exit(0);
    }

    if (type == '1')
    {
        _errorLogger.reportOut(buf);
    }
    else if (type == '2')
    {
        ErrorLogger::ErrorMessage msg;
        msg.deserialize(buf);

        // Alert only about unique errors
        std::string errmsg = msg.toString(_settings._verbose);
        if (std::find(_errorList.begin(), _errorList.end(), errmsg) == _errorList.end())
        {
            _errorList.push_back(errmsg);
            _errorLogger.reportErr(msg);
        }
    }
    else if (type == '3')
    {
        _fileCount++;
        std::istringstream iss(buf);
        unsigned int fileResult = 0;
        iss >> fileResult;
        result += fileResult;
        _errorLogger.reportStatus(_fileCount, _filenames.size());
        delete [] buf;
        return -1;
    }

    delete [] buf;
    return 1;
}

unsigned int ThreadExecutor::check()
{
    _fileCount = 0;
    unsigned int result = 0;
    if (pipe(_pipe) == -1)
    {
        perror("pipe");
        exit(1);
    }

    int flags = 0;
    if ((flags = fcntl(_pipe[0], F_GETFL, 0)) < 0)
    {
        perror("fcntl");
        exit(1);
    }

    if (fcntl(_pipe[0], F_SETFL, flags | O_NONBLOCK) < 0)
    {
        perror("fcntl");
        exit(1);
    }

    unsigned int childCount = 0;
    unsigned int i = 0;
    while (true)
    {
        // Start a new child
        if (i < _filenames.size() && childCount < _settings._jobs)
        {
            pid_t pid = fork();
            if (pid < 0)
            {
                // Error
                std::cerr << "Failed to create child process" << std::endl;
                exit(EXIT_FAILURE);
            }
            else if (pid == 0)
            {
                CppCheck fileChecker(*this);
                fileChecker.settings(_settings);

                if (_fileContents.size() > 0 && _fileContents.find(_filenames[i]) != _fileContents.end())
                {
                    // File content was given as a string
                    fileChecker.addFile(_filenames[i], _fileContents[ _filenames[i] ]);
                }
                else
                {
                    // Read file from a file
                    fileChecker.addFile(_filenames[i]);
                }

                unsigned int resultOfCheck = fileChecker.check();
                std::ostringstream oss;
                oss << resultOfCheck;
                writeToPipe('3', oss.str());
                exit(0);
            }

            ++childCount;
            ++i;
        }
        else if (childCount > 0)
        {
            // Wait for child to quit before stating new processes
            while (true)
            {
                int readRes = handleRead(result);
                if (readRes == -1)
                    break;
                else if (readRes == 0)
                    usleep(5000); // 5 ms
            }

            int stat = 0;
            waitpid(0, &stat, 0);
            --childCount;
        }
        else if (childCount == 0)
        {
            // All done
            break;
        }
    }


    return result;
}

void ThreadExecutor::writeToPipe(char type, const std::string &data)
{
    unsigned int len = data.length() + 1;
    char *out = new char[ len + 1 + sizeof(len)];
    out[0] = type;
    std::memcpy(&(out[1]), &len, sizeof(len));
    std::memcpy(&(out[1+sizeof(len)]), data.c_str(), len);
    if (write(_pipe[1], out, len + 1 + sizeof(len)) <= 0)
    {
        delete [] out;
        out = 0;
        std::cerr << "#### ThreadExecutor::writeToPipe, Failed to write to pipe" << std::endl;
        exit(0);
    }

    delete [] out;
}

void ThreadExecutor::reportOut(const std::string &outmsg)
{
    writeToPipe('1', outmsg);
}

void ThreadExecutor::reportErr(const ErrorLogger::ErrorMessage &msg)
{
    writeToPipe('2', msg.serialize());
}

void ThreadExecutor::reportStatus(unsigned int /*index*/, unsigned int /*max*/)
{
    // Not used
}

#else
unsigned int ThreadExecutor::check()
{
    return 0;
}

void ThreadExecutor::reportOut(const std::string &/*outmsg*/)
{

}
void ThreadExecutor::reportErr(const ErrorLogger::ErrorMessage &/*msg*/)
{

}

void ThreadExecutor::reportStatus(unsigned int /*index*/, unsigned int /*max*/)
{

}
#endif
