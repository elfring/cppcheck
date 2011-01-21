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
 #include "test-config.h"   // System settings from the build configuration
#endif

#include "testsuite.h"
#include "cmdlineparser.h"
#include "settings.h"
#include "redirect.h"

class TestCmdlineParser : public TestFixture
{
public:
    TestCmdlineParser() : TestFixture("TestCmdlineParser")
    { }

private:

    void run()
    {
        TEST_CASE(nooptions);
        TEST_CASE(helpshort);
        TEST_CASE(helplong);
        TEST_CASE(showversion);
        TEST_CASE(onefile);
        TEST_CASE(onepath);
        TEST_CASE(optionwithoutfile);
        TEST_CASE(verboseshort);
        TEST_CASE(verboselong);
        TEST_CASE(debug);
        TEST_CASE(debugwarnings);
        TEST_CASE(forceshort);
        TEST_CASE(forcelong);
        TEST_CASE(quietshort);
        TEST_CASE(quietlong);
        TEST_CASE(defines);
        TEST_CASE(defines2);
        TEST_CASE(defines3);
        TEST_CASE(includesnopath);
        TEST_CASE(includes);
        TEST_CASE(includes2);
        TEST_CASE(enabledAll);
        TEST_CASE(enabledStyle);
        TEST_CASE(enabledUnusedFunction);
        TEST_CASE(enabledMissingInclude);
        TEST_CASE(errorExitcode);
        TEST_CASE(errorExitcodeMissing);
        TEST_CASE(errorExitcodeStr);
        TEST_CASE(exitcodeSuppressions); // TODO: Create and test real suppression file
        TEST_CASE(fileList); // TODO: Create and test real file listing file
        TEST_CASE(inlineSuppr);
        TEST_CASE(jobs);
        TEST_CASE(jobsMissingCount);
        TEST_CASE(jobsInvalid);
        TEST_CASE(reportProgress);
        TEST_CASE(suppressions); // TODO: Create and test real suppression file
        TEST_CASE(templates);
        TEST_CASE(templatesGcc);
        TEST_CASE(templatesVs);
        TEST_CASE(xml);
        TEST_CASE(unknownParam);
    }


    void nooptions()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(1, argv));
        ASSERT_EQUALS(true, parser.GetShowHelp());
    }

    void helpshort()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "-h"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(2, argv));
        ASSERT_EQUALS(true, parser.GetShowHelp());
    }

    void helplong()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--help"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(2, argv));
        ASSERT_EQUALS(true, parser.GetShowHelp());
    }

    void showversion()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--version"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(2, argv));
        ASSERT_EQUALS(true, parser.GetShowVersion());
    }

    void onefile()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(2, argv));
        ASSERT_EQUALS(1, parser.GetPathNames().size());
        ASSERT_EQUALS("file.cpp", parser.GetPathNames().at(0));
    }

    void onepath()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "src"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(2, argv));
        ASSERT_EQUALS(1, parser.GetPathNames().size());
        ASSERT_EQUALS("src", parser.GetPathNames().at(0));
    }

    void optionwithoutfile()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "-v"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT_EQUALS(false, parser.ParseFromArgs(2, argv));
        ASSERT_EQUALS(0, parser.GetPathNames().size());
    }

    void verboseshort()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "-v", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(3, argv));
        ASSERT_EQUALS(true, settings._verbose);
    }

    void verboselong()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--verbose", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(3, argv));
        ASSERT_EQUALS(true, settings._verbose);
    }

    void debug()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--debug", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(3, argv));
        ASSERT_EQUALS(true, settings.debug);
    }

    void debugwarnings()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--debug-warnings", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(3, argv));
        ASSERT_EQUALS(true, settings.debugwarnings);
    }

    void forceshort()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "-f", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(3, argv));
        ASSERT_EQUALS(true, settings._force);
    }

    void forcelong()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--force", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(3, argv));
        ASSERT_EQUALS(true, settings._force);
    }

    void quietshort()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "-q", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(3, argv));
        ASSERT_EQUALS(true, settings._errorsOnly);
    }

    void quietlong()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--quiet", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(3, argv));
        ASSERT_EQUALS(true, settings._errorsOnly);
    }

    void defines()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "-D_WIN32", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(3, argv));
        ASSERT_EQUALS("_WIN32", settings.userDefines);
    }

    void defines2()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "-D_WIN32", "-DNODEBUG", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(4, argv));
        ASSERT_EQUALS("_WIN32;NODEBUG", settings.userDefines);
    }

    void defines3()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "-D", "DEBUG", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(4, argv));
        ASSERT_EQUALS("DEBUG", settings.userDefines);
    }

    void includesnopath()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "-I", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT_EQUALS(false, parser.ParseFromArgs(3, argv));
    }

    void includes()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "-I include", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(3, argv));
        ASSERT_EQUALS(" include/", settings._includePaths.front());
    }

    void includes2()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "-I include/", "-I framework/", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(4, argv));
        ASSERT_EQUALS(" include/", settings._includePaths.front());
        settings._includePaths.pop_front();
        ASSERT_EQUALS(" framework/", settings._includePaths.front());
    }

    void enabledAll()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--enable=all", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(3, argv));
        ASSERT(settings._checkCodingStyle);
        ASSERT(settings.isEnabled("unusedFunction"));
        ASSERT(settings.isEnabled("missingInclude"));
    }

    void enabledStyle()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--enable=style", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(3, argv));
        ASSERT(settings._checkCodingStyle);
    }

    void enabledUnusedFunction()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--enable=unusedFunction", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(3, argv));
        ASSERT(settings.isEnabled("unusedFunction"));
    }

    void enabledMissingInclude()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--enable=missingInclude", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(3, argv));
        ASSERT(settings.isEnabled("missingInclude"));
    }

    void errorExitcode()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--error-exitcode=5", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(3, argv));
        ASSERT_EQUALS(5, settings._exitCode);
    }

    void errorExitcodeMissing()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--error-exitcode=", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(!parser.ParseFromArgs(3, argv));
    }

    void errorExitcodeStr()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--error-exitcode=foo", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(!parser.ParseFromArgs(3, argv));
    }

    void exitcodeSuppressions()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--error-exitcode-suppressions suppr.txt", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(!parser.ParseFromArgs(3, argv));
    }

    void fileList()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--file-list files.txt", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(!parser.ParseFromArgs(3, argv));
    }

    void inlineSuppr()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--inline-suppr", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(3, argv));
    }

    void jobs()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "-j 3", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(3, argv));
        ASSERT_EQUALS(3, settings._jobs);
    }

    void jobsMissingCount()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "-j", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(!parser.ParseFromArgs(3, argv));
    }

    void jobsInvalid()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "-j e", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(!parser.ParseFromArgs(3, argv));
    }

    void reportProgress()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--report-progress", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(3, argv));
        ASSERT(settings.reportProgress);
    }

    void suppressions()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--suppressions suppr.txt", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(!parser.ParseFromArgs(3, argv));
    }

    void templates()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--template", "{file}:{line},{severity},{id},{message}", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(4, argv));
        ASSERT_EQUALS("{file}:{line},{severity},{id},{message}", settings._outputFormat);
    }

    void templatesGcc()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--template", "gcc", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(4, argv));
        ASSERT_EQUALS("{file}:{line}: {severity}: {message}", settings._outputFormat);
    }

    void templatesVs()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--template", "vs", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(4, argv));
        ASSERT_EQUALS("{file}({line}): {severity}: {message}", settings._outputFormat);
    }

    void xml()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--xml", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(parser.ParseFromArgs(3, argv));
        ASSERT(settings._xml);
    }

    void unknownParam()
    {
        REDIRECT;
        const char *argv[] = {"cppcheck", "--foo", "file.cpp"};
        Settings settings;
        CmdLineParser parser(&settings);
        ASSERT(!parser.ParseFromArgs(3, argv));
    }
};

REGISTER_TEST(TestCmdlineParser)
