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
 #include "library-config.h"   // System settings from the build configuration
#endif

#include "errorlogger.h"
#include "path.h"

#include <sstream>
#include <vector>

ErrorLogger::ErrorMessage::ErrorMessage()
    :_severity(Severity::none)
{

}

ErrorLogger::ErrorMessage::ErrorMessage(const std::list<FileLocation> &callStack, Severity::SeverityType severity, const std::string &msg, const std::string &id)
{
    _callStack = callStack;
    _severity = severity;
    setmsg(msg);
    _id = id;
}

void ErrorLogger::ErrorMessage::setmsg(const std::string &msg)
{
    const std::string::size_type pos = msg.find("\n");
    if (pos == std::string::npos)
    {
        _shortMessage = msg;
        _verboseMessage = msg;
    }
    else
    {
        _shortMessage = msg.substr(0, pos);
        _verboseMessage = msg.substr(pos + 1);
    }
}

std::string ErrorLogger::ErrorMessage::serialize() const
{
    std::ostringstream oss;
    oss << _id.length() << " " << _id;
    oss << Severity::toString(_severity).length() << " " << Severity::toString(_severity);
    oss << _shortMessage.length() << " " << _shortMessage;
    oss << _callStack.size() << " ";

    for (std::list<ErrorLogger::ErrorMessage::FileLocation>::const_iterator tok = _callStack.begin(); tok != _callStack.end(); ++tok)
    {
        std::ostringstream smallStream;
        smallStream << (*tok).line << ":" << (*tok).getfile();
        oss << smallStream.str().length() << " " << smallStream.str();
    }
    return oss.str();
}

bool ErrorLogger::ErrorMessage::deserialize(const std::string &data)
{
    _callStack.clear();
    std::istringstream iss(data);
    std::vector<std::string> results;
    while (iss.good())
    {
        unsigned int len = 0;
        if (!(iss >> len))
            return false;

        iss.get();
        std::string temp;
        for (unsigned int i = 0; i < len && iss.good(); ++i)
        {
            char c = static_cast<char>(iss.get());
            temp.append(1, c);
        }

        results.push_back(temp);
        if (results.size() == 3)
            break;
    }

    _id = results[0];
    _severity = Severity::fromString(results[1]);
    _shortMessage = results[2];

    unsigned int stackSize = 0;
    if (!(iss >> stackSize))
        return false;

    while (iss.good())
    {
        unsigned int len = 0;
        if (!(iss >> len))
            return false;

        iss.get();
        std::string temp;
        for (unsigned int i = 0; i < len && iss.good(); ++i)
        {
            char c = static_cast<char>(iss.get());
            temp.append(1, c);
        }

        ErrorLogger::ErrorMessage::FileLocation loc;
        loc.setfile(temp.substr(temp.find(':') + 1));
        temp = temp.substr(0, temp.find(':'));
        std::istringstream fiss(temp);
        fiss >> loc.line;

        _callStack.push_back(loc);

        if (_callStack.size() >= stackSize)
            break;
    }

    return true;
}

std::string ErrorLogger::ErrorMessage::getXMLHeader(int version)
{
    std::ostringstream strver;
    if (version > 1)
        strver << " version=\"" << version << "\"";

    return  "<?xml version=\"1.0\"?>\n"
            "<results" + strver.str() + ">";
}

std::string ErrorLogger::ErrorMessage::getXMLFooter()
{
    return "</results>";
}

static std::string stringToXml(std::string s)
{
    std::string::size_type pos = 0;
    while ((pos = s.find_first_of("<>&\"\n", pos)) != std::string::npos)
    {
        if (s[pos] == '<')
            s.insert(pos + 1, "&lt;");
        else if (s[pos] == '>')
            s.insert(pos + 1, "&gt;");
        else if (s[pos] == '&')
            s.insert(pos + 1, "&amp;");
        else if (s[pos] == '"')
            s.insert(pos + 1, "&quot;");
        else if (s[pos] == '\n')
            s.insert(pos + 1, "&#xa;");
        s.erase(pos, 1);
        ++pos;
    }
    return s;
}

std::string ErrorLogger::ErrorMessage::toXML(bool verbose, int version) const
{
    std::ostringstream xml;

    if (version == 1)
    {
        xml << "<error";
        if (!_callStack.empty())
        {
            xml << " file=\"" << stringToXml(_callStack.back().getfile()) << "\"";
            xml << " line=\"" << _callStack.back().line << "\"";
        }
        xml << " id=\"" << _id << "\"";
        xml << " severity=\"" << (_severity == Severity::error ? "error" : "style") << "\"";
        xml << " msg=\"" << stringToXml(verbose ? _verboseMessage : _shortMessage) << "\"";
        xml << "/>";
    }
    else if (version == 2)
    {
        xml << "  <error";
        xml << " id=\"" << _id << "\"";
        xml << " severity=\"" << Severity::toString(_severity) << "\"";
        xml << " msg=\"" << stringToXml(_shortMessage) << "\"";
        xml << " verbose=\"" << stringToXml(_verboseMessage) << "\"";
        xml << ">" << std::endl;

        for (std::list<FileLocation>::const_reverse_iterator it = _callStack.rbegin(); it != _callStack.rend(); ++it)
        {
            xml << "    <location";
            xml << " file=\"" << stringToXml((*it).getfile()) << "\"";
            xml << " line=\"" << (*it).line << "\"";
            xml << "/>" << std::endl;
        }

        xml << "  </error>";
    }

    return xml.str();
}

void ErrorLogger::ErrorMessage::findAndReplace(std::string &source, const std::string &searchFor, const std::string &replaceWith)
{
    std::string::size_type index = 0;
    while ((index = source.find(searchFor, index)) != std::string::npos)
    {
        source.replace(index, searchFor.length(), replaceWith);
        index += replaceWith.length() - searchFor.length() + 1;
    }
}

std::string ErrorLogger::ErrorMessage::toString(bool verbose, const std::string &outputFormat) const
{
    if (outputFormat.length() == 0)
    {
        std::ostringstream text;
        if (!_callStack.empty())
            text << callStackToString(_callStack) << ": ";
        if (_severity != Severity::none)
            text << "(" << Severity::toString(_severity) << ") ";
        text << (verbose ? _verboseMessage : _shortMessage);
        return text.str();
    }
    else
    {
        std::string result = outputFormat;
        findAndReplace(result, "{id}", _id);
        findAndReplace(result, "{severity}", Severity::toString(_severity));
        findAndReplace(result, "{message}", verbose ? _verboseMessage : _shortMessage);

        if (!_callStack.empty())
        {
            std::ostringstream oss;
            oss << _callStack.back().line;
            findAndReplace(result, "{line}", oss.str());
            findAndReplace(result, "{file}", _callStack.back().getfile());
        }
        else
        {
            findAndReplace(result, "{file}", "");
            findAndReplace(result, "{line}", "");
        }

        return result;
    }
}

std::string ErrorLogger::callStackToString(const std::list<ErrorLogger::ErrorMessage::FileLocation> &callStack)
{
    std::ostringstream ostr;
    for (std::list<ErrorLogger::ErrorMessage::FileLocation>::const_iterator tok = callStack.begin(); tok != callStack.end(); ++tok)
        ostr << (tok == callStack.begin() ? "" : " -> ") << "[" << (*tok).getfile() << ":" << (*tok).line << "]";
    return ostr.str();
}


std::string ErrorLogger::ErrorMessage::FileLocation::getfile(bool convert) const
{
    std::string f = Path::simplifyPath(_file.c_str());

    if (convert)
        f = Path::toNativeSeparators(f);
    return f;
}

void ErrorLogger::ErrorMessage::FileLocation::setfile(const std::string &file)
{
    _file = file;
    _file = Path::fromNativeSeparators(_file);
}
