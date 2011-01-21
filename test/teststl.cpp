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

#include "tokenize.h"
#include "checkstl.h"
#include "testsuite.h"
#include <sstream>

extern std::ostringstream errout;

class TestStl : public TestFixture
{
public:
    TestStl() : TestFixture("TestStl")
    { }

private:
    void run()
    {
        TEST_CASE(iterator1);
        TEST_CASE(iterator2);
        TEST_CASE(iterator3);
        TEST_CASE(iterator4);
        TEST_CASE(iterator5);
        TEST_CASE(iterator6);
        TEST_CASE(iterator7);
        TEST_CASE(iterator8);

        TEST_CASE(dereference);
        TEST_CASE(dereference_member);

        TEST_CASE(STLSize);
        TEST_CASE(STLSizeNoErr);
        TEST_CASE(erase1);
        TEST_CASE(erase2);
        TEST_CASE(erase3);
        TEST_CASE(erase4);
        TEST_CASE(erase5);
        TEST_CASE(eraseBreak);
        TEST_CASE(eraseContinue);
        TEST_CASE(eraseReturn1);
        TEST_CASE(eraseReturn2);
        TEST_CASE(eraseReturn3);
        TEST_CASE(eraseGoto);
        TEST_CASE(eraseAssign1);
        TEST_CASE(eraseAssign2);
        TEST_CASE(eraseErase);
        TEST_CASE(eraseByValue);

        TEST_CASE(pushback1);
        TEST_CASE(pushback2);
        TEST_CASE(pushback3);
        TEST_CASE(pushback4);
        TEST_CASE(pushback5);
        TEST_CASE(pushback6);
        TEST_CASE(pushback7);
        TEST_CASE(pushback8);
        TEST_CASE(pushback9);
        TEST_CASE(pushback10);

        TEST_CASE(insert1);

        TEST_CASE(invalidcode);

        TEST_CASE(stlBoundries1);
        TEST_CASE(stlBoundries2);
        TEST_CASE(stlBoundries3);

        // if (str.find("ab"))
        TEST_CASE(if_find);
        TEST_CASE(if_str_find);

        TEST_CASE(size1);

        // Redundant conditions..
        // if (ints.find(123) != ints.end()) ints.remove(123);
        TEST_CASE(redundantCondition1);
        TEST_CASE(redundantCondition2);

        // missing inner comparison when incrementing iterator inside loop
        TEST_CASE(missingInnerComparison1);
        TEST_CASE(missingInnerComparison2);     // no FP when there is comparison
        TEST_CASE(missingInnerComparison3);     // no FP when there is iterator shadowing
        TEST_CASE(missingInnerComparison4);     // no FP when "break;" is used
        TEST_CASE(missingInnerComparison5);     // Ticket #2154 - FP

        // catch common problems when using the string::c_str() function
        TEST_CASE(cstr);
    }

    void check(const std::string &code)
    {
        // Clear the error buffer..
        errout.str("");

        Settings settings;
        settings.inconclusive = true;
        settings._checkCodingStyle = true;

        // Tokenize..
        Tokenizer tokenizer(&settings, this);
        std::istringstream istr(code.c_str());
        tokenizer.tokenize(istr, "test.cpp");
        tokenizer.simplifyTokenList();

        // Check..
        CheckStl checkStl;
        checkStl.runSimplifiedChecks(&tokenizer, &settings, this);
    }


    void iterator1()
    {
        check("void f()\n"
              "{\n"
              "    list<int> l1;\n"
              "    list<int> l2;\n"
              "    for (list<int>::iterator it = l1.begin(); it != l2.end(); ++it)\n"
              "    { }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:5]: (error) Same iterator is used with both l1 and l2\n", errout.str());
    }

    void iterator2()
    {
        check("void foo()\n"
              "{\n"
              "    list<int> l1;\n"
              "    list<int> l2;\n"
              "    list<int>::iterator it = l1.begin();\n"
              "    while (it != l2.end())\n"
              "    {\n"
              "        ++it;\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:6]: (error) Same iterator is used with both l1 and l2\n", errout.str());
    }

    void iterator3()
    {
        check("void foo()\n"
              "{\n"
              "    list<int> l1;\n"
              "    list<int> l2;\n"
              "    list<int>::iterator it = l1.begin();\n"
              "    l2.insert(it, 0);\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:6]: (error) Same iterator is used with both l1 and l2\n", errout.str());
    }

    void iterator4()
    {
        check("void foo(std::vector<std::string> &test)\n"
              "{\n"
              "    std::set<int> result;\n"
              "    for (std::vector<std::string>::const_iterator cit = test.begin();\n"
              "        cit != test.end();\n"
              "        ++cit)\n"
              "    {\n"
              "        result.insert(cit->size());\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());
    }

    void iterator5()
    {
        check("void foo()\n"
              "{\n"
              "    std::vector<int> ints1;\n"
              "    std::vector<int> ints2;\n"
              "    std::vector<int>::iterator it = std::find(ints1.begin(), ints2.end(), 22);\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:5]: (error) mismatching containers\n", errout.str());
    }

    void iterator6()
    {
        // Ticket #1357
        check("void foo(const std::set<int> &ints1)\n"
              "{\n"
              "    std::set<int> ints2;\n"
              "    std::set<int>::iterator it1 = ints1.begin();\n"
              "    std::set<int>::iterator it2 = ints1.end();\n"
              "    ints2.insert(it1, it2);\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());
    }

    void iterator7()
    {
        // Ticket #1600
        check("void foo(std::vector<int> &r)\n"
              "{\n"
              "    std::vector<int>::iterator aI = r.begin();\n"
              "    while(aI != r.end())\n"
              "    {\n"
              "        if (*aI == 0)\n"
              "        {\n"
              "            r.insert(aI, 42);\n"
              "            return;\n"
              "        }\n"
              "        ++aI;\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());

        // Execution path checking..
        check("void foo(std::vector<int> &r, int c)\n"
              "{\n"
              "    std::vector<int>::iterator aI = r.begin();\n"
              "    while(aI != r.end())\n"
              "    {\n"
              "        if (*aI == 0)\n"
              "        {\n"
              "            r.insert(aI, 42);\n"
              "            if (c)\n"
              "            {\n"
              "                return;\n"
              "            }\n"
              "        }\n"
              "        ++aI;\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());
        TODO_ASSERT_EQUALS("[test.cpp:14] (error) After insert, the iterator 'aI' may be invalid", errout.str());
    }

    void iterator8()
    {
        // Ticket #1679
        check("void foo()\n"
              "{\n"
              "    std::set<int> s1;\n"
              "    std::set<int> s2;\n"
              "    for (std::set<int>::iterator it = s1.begin(); it != s1.end(); ++it)\n"
              "    {\n"
              "        if (true) { }\n"
              "        if (it != s2.end()) continue;\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:8]: (error) Same iterator is used with both s1 and s2\n", errout.str());
    }

    // Dereferencing invalid pointer
    void dereference()
    {
        check("void f()\n"
              "{\n"
              "    std::vector<int> ints;\n"
              "    std::vector<int>::iterator iter;\n"
              "    iter = ints.begin() + 2;\n"
              "    ints.erase(iter);\n"
              "    std::cout << (*iter) << std::endl;\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:7]: (error) Dereferenced iterator 'iter' has been erased\n", errout.str());
    }

    void dereference_member()
    {
        check("void f()\n"
              "{\n"
              "    std::map<int, int> ints;\n"
              "    std::map<int, int>::iterator iter;\n"
              "    iter = ints.begin();\n"
              "    ints.erase(iter);\n"
              "    std::cout << iter->first << std::endl;\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:7]: (error) Dereferenced iterator 'iter' has been erased\n", errout.str());
    }


    void STLSize()
    {
        check("void foo()\n"
              "{\n"
              "    std::vector<int> foo;\n"
              "    for (unsigned int ii = 0; ii <= foo.size(); ++ii)\n"
              "    {\n"
              "       foo[ii] = 0;\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:6]: (error) When ii==foo.size(), foo[ii] is out of bounds\n", errout.str());

        check("void foo()\n"
              "{\n"
              "    std::vector<int> foo;\n"
              "    foo.push_back(1);\n"
              "    for (unsigned int ii = 0; ii <= foo.size(); ++ii)\n"
              "    {\n"
              "    }\n"
              "    int ii = 0;\n"
              "    foo[ii] = 0;\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());
    }

    void STLSizeNoErr()
    {
        {
            check("void foo()\n"
                  "{\n"
                  "    std::vector<int> foo;\n"
                  "    for (unsigned int ii = 0; ii < foo.size(); ++ii)\n"
                  "    {\n"
                  "       foo[ii] = 0;\n"
                  "    }\n"
                  "}\n");
            ASSERT_EQUALS("", errout.str());
        }

        {
            check("void foo()\n"
                  "{\n"
                  "    std::vector<int> foo;\n"
                  "    for (unsigned int ii = 0; ii <= foo.size(); ++ii)\n"
                  "    {\n"
                  "    }\n"
                  "}\n");
            ASSERT_EQUALS("", errout.str());
        }

        {
            check("void foo()\n"
                  "{\n"
                  "    std::vector<int> foo;\n"
                  "    for (unsigned int ii = 0; ii <= foo.size(); ++ii)\n"
                  "    {\n"
                  "        if (ii == foo.size())\n"
                  "        {\n"
                  "        }\n"
                  "        else\n"
                  "        {\n"
                  "            foo[ii] = 0;\n"
                  "        }\n"
                  "    }\n"
                  "}\n");
            ASSERT_EQUALS("", errout.str());
        }
    }





    void erase1()
    {
        check("void f()\n"
              "{\n"
              "    for (it = foo.begin(); it != foo.end(); ++it)\n"
              "    {\n"
              "        foo.erase(it);\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:5]: (error) Dangerous iterator usage. After erase the iterator is invalid so dereferencing it or comparing it with another iterator is invalid.\n", errout.str());

        check("for (it = foo.begin(); it != foo.end(); ++it)\n"
              "{\n"
              "    foo.erase(it);\n"
              "}\n"
              "for (it = foo.begin(); it != foo.end(); ++it)\n"
              "{\n"
              "    foo.erase(it);\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:3]: (error) Dangerous iterator usage. After erase the iterator is invalid so dereferencing it or comparing it with another iterator is invalid.\n"
                      "[test.cpp:7]: (error) Dangerous iterator usage. After erase the iterator is invalid so dereferencing it or comparing it with another iterator is invalid.\n", errout.str());

        check("void f(std::list<int> &ints)\n"
              "{\n"
              "    std::list<int>::iterator i = ints.begin();\n"
              "    i = ints.erase(i);\n"
              "    *i = 0;\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());

        // #2101
        check("void f(vector< list<int> > &ints, unsigned int i)\n"
              "{\n"
              "    list<int>::iterator it;\n"
              "    for(it = ints[i].begin(); it != ints[i].end(); it++) {\n"
              "        if (*it % 2)\n"
              "            it = ints[i].erase(it);\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());
    }

    void erase2()
    {
        check("static void f()\n"
              "{\n"
              "    for (iterator it = foo.begin(); it != foo.end(); it = next)\n"
              "    {\n"
              "        next = it;\n"
              "        next++;\n"
              "        foo.erase(it);\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());
    }

    void erase3()
    {
        check("static void f(std::list<abc> &foo)\n"
              "{\n"
              "    std::list<abc>::iterator it = foo.begin();\n"
              "    foo.erase(it->a);\n"
              "    if (it->b);\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());
    }

    void erase4()
    {
        check("void f()\n"
              "{\n"
              "    std::list<int>::iterator it, it2;\n"
              "    for (it = foo.begin(); it != i2; ++it)\n"
              "    {\n"
              "        foo.erase(it);\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:6]: (error) Dangerous iterator usage. After erase the iterator is invalid so dereferencing it or comparing it with another iterator is invalid.\n", errout.str());

        check("void f()\n"
              "{\n"
              "    std::list<int>::iterator it = foo.begin();\n"
              "    for (; it != i2; ++it)\n"
              "    {\n"
              "        foo.erase(it);\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:6]: (error) Dangerous iterator usage. After erase the iterator is invalid so dereferencing it or comparing it with another iterator is invalid.\n", errout.str());

        check("void f()\n"
              "{\n"
              "    std::list<int>::iterator it = foo.begin();\n"
              "    while (it != i2)\n"
              "    {\n"
              "        foo.erase(it);\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:6]: (error) Dangerous iterator usage. After erase the iterator is invalid so dereferencing it or comparing it with another iterator is invalid.\n", errout.str());

        check("void f()\n"
              "{\n"
              "    std::list<int>::iterator it = foo.begin();\n"
              "    while (it != i2)\n"
              "    {\n"
              "        foo.erase(++it);\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:6]: (error) Dangerous iterator usage. After erase the iterator is invalid so dereferencing it or comparing it with another iterator is invalid.\n", errout.str());
    }

    void erase5()
    {
        check("void f()\n"
              "{\n"
              "    std::list<int> foo;\n"
              "    std::list<int>::iterator it;\n"
              "    for (it = foo.begin(); it != foo.end(); ++it)\n"
              "    {\n"
              "        if (*it == 123)\n"
              "            foo.erase(it);\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:8]: (error) Dangerous iterator usage. After erase the iterator is invalid so dereferencing it or comparing it with another iterator is invalid.\n", errout.str());
    }

    void eraseBreak()
    {
        check("void f()\n"
              "{\n"
              "    for (iterator it = foo.begin(); it != foo.end(); ++it)\n"
              "    {\n"
              "        foo.erase(it);\n"
              "        if (x)"
              "            break;\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:5]: (error) Dangerous iterator usage. After erase the iterator is invalid so dereferencing it or comparing it with another iterator is invalid.\n", errout.str());

        check("void f()\n"
              "{\n"
              "    for (iterator it = foo.begin(); it != foo.end(); ++it)\n"
              "    {\n"
              "        if (x) {\n"
              "            foo.erase(it);\n"
              "            break;\n"
              "        }\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());
    }

    void eraseContinue()
    {
        check("void f(std::vector<int> &ints)\n"
              "{\n"
              "    std::vector<int>::iterator it;\n"
              "    std::vector<int>::iterator jt = ints.begin();\n"
              "    for (it = ints.begin(); it != ints.end(); it = jt) {\n"
              "        ++jt;\n"
              "        if (*it == 1) {\n"
              "            jt = ints.erase(it);\n"
              "            continue;\n"
              "        }\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());
    }

    void eraseReturn1()
    {
        check("void f()\n"
              "{\n"
              "    std::vector<int> foo;\n"
              "    std::vector<int>::iterator it;\n"
              "    for (it = foo.begin(); it != foo.end(); ++it)\n"
              "    {\n"
              "        foo.erase(it);\n"
              "        return;\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());
    }

    void eraseReturn2()
    {
        check("void f()\n"
              "{\n"
              "    std::vector<int> foo;\n"
              "    std::vector<int>::iterator it;\n"
              "    for (it = foo.begin(); it != foo.end(); ++it)\n"
              "    {\n"
              "        if (*it == 1) {\n"
              "            foo.erase(it);\n"
              "            return;\n"
              "        }\n"
              "        else {\n"
              "            foo.erase(it);\n"
              "            return;\n"
              "        }\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());
    }

    void eraseReturn3()
    {
        check("void f()\n"
              "{\n"
              "    std::vector<int> foo;\n"
              "    std::vector<int>::iterator it;\n"
              "    for (it = foo.begin(); it != foo.end(); ++it)\n"
              "    {\n"
              "        if (somecondition) {\n"
              "            if (*it == 1)\n"
              "                foo.erase(it);\n"
              "            else\n"
              "                *it = 0;\n"
              "            return;\n"
              "        }\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());

        check("void f()\n"
              "{\n"
              "    std::vector<int> foo;\n"
              "    std::vector<int>::iterator it;\n"
              "    for (it = foo.begin(); it != foo.end(); ++it)\n"
              "    {\n"
              "        if (a) {\n"
              "            if (b)\n"
              "                foo.erase(it);\n"
              "            else\n"
              "                *it = 0;\n"
              "        }\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:9]: (error) Dangerous iterator usage. After erase the iterator is invalid so dereferencing it or comparing it with another iterator is invalid.\n", errout.str());
    }

    void eraseGoto()
    {
        check("void f()\n"
              "{\n"
              "    for (iterator it = foo.begin(); it != foo.end(); ++it)\n"
              "    {\n"
              "        foo.erase(it);\n"
              "        goto abc;\n"
              "    }\n"
              "bar:\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());
    }

    void eraseAssign1()
    {
        check("void f()\n"
              "{\n"
              "    for (iterator it = foo.begin(); it != foo.end(); ++it)\n"
              "    {\n"
              "        foo.erase(it);\n"
              "        it = foo.begin();\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());
    }

    void eraseAssign2()
    {
        check("void f(list<int> &ints)\n"
              "{\n"
              "    for (list<int>::iterator it = ints.begin(); it != ints.end();) {\n"
              "        if (*it == 123) {\n"
              "            list<int>::iterator copy = it;\n"
              "            ++copy;\n"
              "            ints.erase(it);\n"
              "            it = copy;\n"
              "        } else {\n"
              "            it->second = 123;\n"
              "            ++it;\n"
              "        }\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());
    }

    void eraseErase()
    {
        check("void f(std::vector<ints> &ints)\n"
              "{\n"
              "    std::vector<int>::iterator iter;\n"
              "    iter = ints.begin() + 2;\n"
              "    ints.erase(iter);\n"
              "    ints.erase(iter);\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:6]: (error) Invalid iterator: iter\n", errout.str());
    }

    void eraseByValue()
    {
        check("void f()\n"
              "{\n"
              "    std::set<int> foo;\n"
              "    for (std::set<int> it = foo.begin(); it != foo.end(); ++it)\n"
              "    {\n"
              "        foo.erase(*it);\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:6]: (error) Iterator 'it' becomes invalid when deleted by value from 'foo'\n", errout.str());
    }


    void pushback1()
    {
        check("void f(const std::vector<int> &foo)\n"
              "{\n"
              "    std::vector<int>::const_iterator it = foo.begin();\n"
              "    foo.push_back(123);\n"
              "    *it;\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:5]: (error) After push_back, the iterator 'it' may be invalid\n", errout.str());
    }

    void pushback2()
    {
        check("void f()\n"
              "{\n"
              "    std::vector<int>::const_iterator it = foo.begin();\n"
              "    foo.push_back(123);\n"
              "    {\n"
              "        int *it = &foo[0];\n"
              "        *it = 456;\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());
    }

    void pushback3()
    {
        check("void f()\n"
              "{\n"
              "    std::vector<int> foo;\n"
              "    foo.push_back(10);\n"
              "    std::vector<int>::iterator it;\n"
              "    for (it = foo.begin(); it != foo.end(); ++it)\n"
              "    {\n"
              "        foo.push_back(123);\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:8]: (error) After push_back, the iterator 'it' may be invalid\n", errout.str());
    }

    void pushback4()
    {
        check("void f()\n"
              "{\n"
              "    std::vector<int> ints;\n"
              "    ints.push_back(1);\n"
              "    int *first = &ints[0];\n"
              "    ints.push_back(2);\n"
              "    *first;\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:7]: (error) Invalid pointer 'first' after push_back / push_front\n", errout.str());
    }

    void pushback5()
    {
        check("void f()\n"
              "{\n"
              "    std::vector<int>::const_iterator i;\n"
              "\n"
              "    for (i=v.begin(); i!=v.end(); ++i)\n"
              "    {\n"
              "    }\n"
              "\n"
              "    for (i=rhs.v.begin(); i!=rhs.v.end(); ++i)\n"
              "    {\n"
              "        v.push_back(*i);\n"
              "    }\n"
              "}");
        ASSERT_EQUALS("", errout.str());
    }

    void pushback6()
    {
        // ticket #735
        check("void f()\n"
              "{\n"
              "    vector<int> v;\n"
              "    vector.push_back(1);\n"
              "    vector.push_back(2);\n"
              "    for (vector<int>::iterator it = v.begin(); it != v.end(); ++it)\n"
              "    {\n"
              "        if (*it == 1)\n"
              "            v.push_back(10);\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:9]: (error) After push_back, the iterator 'it' may be invalid\n", errout.str());

        check("void f()\n"
              "{\n"
              "    std::vector<int> v;\n"
              "    vector.push_back(1);\n"
              "    vector.push_back(2);\n"
              "    for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)\n"
              "    {\n"
              "        if (*it == 1)\n"
              "            v.push_back(10);\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:9]: (error) After push_back, the iterator 'it' may be invalid\n", errout.str());
    }

    void pushback7()
    {
        check("void f()\n"
              "{\n"
              "    std::vector<int> foo;\n"
              "    foo.push_back(10);\n"
              "    std::vector<int>::iterator it;\n"
              "    for (it = foo.begin(); it != foo.end(); it++)\n"
              "    {\n"
              "        foo.push_back(123);\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:8]: (error) After push_back, the iterator 'it' may be invalid\n", errout.str());
    }

    void pushback8()
    {
        check("void f()\n"
              "{\n"
              "    std::vector<int> ints;\n"
              "    std::vector<int>::const_iterator end = ints.end();\n"
              "    ints.push_back(10);\n"
              "    std::vector<int>::iterator it;\n"
              "    unsigned int sum = 0;\n"
              "    for (it = ints.begin(); it != end; ++it)\n"
              "    {\n"
              "        sum += *it;\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:8]: (error) After push_back, the iterator 'end' may be invalid\n", errout.str());
    }

    void pushback9()
    {
        check("struct A {\n"
              "    std::vector<int> ints;\n"
              "};\n"
              "\n"
              "void f()\n"
              "{\n"
              "    std::vector<int> ints;\n"
              "    A a;\n"
              "    std::vector<int>::const_iterator i = ints.begin();\n"
              "    std::vector<int>::const_iterator e = ints.end();\n"
              "    while (i != e)\n"
              "    {\n"
              "        a.ints.push_back(*i);\n"
              "        ++i;\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());
    }

    void pushback10()
    {
        check("void f(std::vector<int> &foo)\n"
              "{\n"
              "    std::vector<int>::const_iterator it = foo.begin();\n"
              "    foo.reserve(100);\n"
              "    *it = 0;\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:5]: (error) After reserve, the iterator 'it' may be invalid\n", errout.str());

        // in loop
        check("void f()\n"
              "{\n"
              "    std::vector<int> foo;\n"
              "    foo.push_back(10);\n"
              "    std::vector<int>::iterator it;\n"
              "    for (it = foo.begin(); it != foo.end(); ++it)\n"
              "    {\n"
              "        foo.reserve(123);\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:8]: (error) After reserve, the iterator 'it' may be invalid\n", errout.str());
    }


    void insert1()
    {
        check("void f(std::vector<int> &ints)\n"
              "{\n"
              "    std::vector<int>::iterator iter = ints.begin() + 5;\n"
              "    ints.insert(ints.begin(), 1);\n"
              "    ++iter;\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:5]: (error) After insert, the iterator 'iter' may be invalid\n", errout.str());

        check("void f()\n"
              "{\n"
              "    std::vector<int> ints;\n"
              "    std::vector<int>::iterator iter = ints.begin();\n"
              "    ints.insert(iter, 1);\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());

        check("void f()\n"
              "{\n"
              "    std::vector<int> ints;\n"
              "    std::vector<int>::iterator iter = ints.begin();\n"
              "    ints.insert(iter, 1);\n"
              "    ints.insert(iter, 2);\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:6]: (error) After insert, the iterator 'iter' may be invalid\n", errout.str());
    }



    void invalidcode()
    {
        errout.str("");
        const std::string src = "void f()\n"
                                "{\n"
                                "    for ( \n"
                                "}\n";

        Settings settings;
        Tokenizer tokenizer(&settings, this);
        std::istringstream istr(src);
        ASSERT_EQUALS(false, tokenizer.tokenize(istr, "test.cpp"));
        ASSERT_EQUALS("[test.cpp:3]: (error) Invalid number of character (() when these macros are defined: ''.\n", errout.str());
    }



    void stlBoundries1()
    {
        const int STL_CONTAINER_LIST = 9;
        const std::string stlCont[STL_CONTAINER_LIST] =
        {
            "deque", "list", "set", "multiset", "map",
            "multimap", "hash_map", "hash_multimap", "hash_set"
        };

        for (int i = 0; i < STL_CONTAINER_LIST; ++i)
        {
            check("void f()\n"
                  "{\n"
                  "    std::" + stlCont[i] + "<int>::iterator it;\n"
                  "    for (it = ab.begin(); it < ab.end(); ++it)\n"
                  "        ;\n"
                  "}\n");

            ASSERT_EQUALS("[test.cpp:4]: (error) " + stlCont[i]  + " range check should use != and not < since the order of the pointers isn't guaranteed\n", errout.str());
        }
    }

    void stlBoundries2()
    {
        check("void f()\n"
              "{\n"
              "    std::vector<std::string> files;\n"
              "    std::vector<std::string>::const_iterator it;\n"
              "    for (it = files.begin(); it < files.end(); it++) { }\n"
              "    for (it = files.begin(); it < files.end(); it++) { };\n"
              "}\n");

        ASSERT_EQUALS("", errout.str());
    }

    void stlBoundries3()
    {
        check("void f()\n"
              "{\n"
              "    set<int> files;\n"
              "    set<int>::const_iterator current;\n"
              "    for (current = files.begin(); current != files.end(); ++current)\n"
              "    {\n"
              "       assert(*current < 100)\n"
              "    }\n"
              "}\n");

        ASSERT_EQUALS("", errout.str());
    }



    void if_find()
    {
        // ---------------------------
        // set::find
        // ---------------------------

        // error
        check("void f(std::set<int> s)\n"
              "{\n"
              "    if (s.find(12)) { }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:3]: (warning) Suspicious condition. The result of find is an iterator, but it is not properly checked.\n", errout.str());

        // ok
        check("void f(std::set<int> s)\n"
              "{\n"
              "    if (s.find(123) != s.end()) { }\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());


        // ---------------------------
        // std::find
        // ---------------------------

        // error
        check("void f()\n"
              "{\n"
              "    if (std::find(a,b,c)) { }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:3]: (warning) Suspicious condition. The result of find is an iterator, but it is not properly checked.\n", errout.str());

        // ok
        check("void f()\n"
              "{\n"
              "    if (std::find(a,b,c) != c) { }\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());
    }

    void if_str_find()
    {
        // error
        check("void f(const std::string &s)\n"
              "{\n"
              "    if (s.find(\"abc\")) { }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:3]: (warning) Suspicious condition. string::find will return 0 if the string is found at position 0. If this is what you want to check then string::compare is a faster alternative because it doesn't scan through the string.\n", errout.str());
    }


    void size1()
    {
        check("void f()\n"
              "{\n"
              "    std::list<int> x;\n"
              "    if (x.size() == 0) {}\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:4]: (performance) Possible inefficient checking for 'x' emptiness.\n", errout.str());

        check("void f()\n"
              "{\n"
              "    std::list<int> x;\n"
              "    if (x.size() != 0) {}\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:4]: (performance) Possible inefficient checking for 'x' emptiness.\n", errout.str());

        check("void f()\n"
              "{\n"
              "    std::list<int> x;\n"
              "    if (x.size() > 0) {}\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:4]: (performance) Possible inefficient checking for 'x' emptiness.\n", errout.str());

        check("void f()\n"
              "{\n"
              "    std::list<int> x;\n"
              "    if (x.size()) {}\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:4]: (performance) Possible inefficient checking for 'x' emptiness.\n", errout.str());

        check("void f()\n"
              "{\n"
              "    std::list<int> x;\n"
              "    fun(x.size());\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());
    }

    void redundantCondition1()
    {
        check("void f()\n"
              "{\n"
              "    if (haystack.find(needle) != haystack.end())\n"
              "        haystack.remove(needle);"
              "}\n");
        ASSERT_EQUALS("[test.cpp:3]: (style) Redundant checking of STL container element.\n", errout.str());
    }

    void redundantCondition2()
    {
        check("void f()\n"
              "{\n"
              "    if (haystack.find(needle) != haystack.end())\n"
              "    {\n"
              "        haystack.remove(needle);\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:3]: (style) Redundant checking of STL container element.\n", errout.str());
    }

    void missingInnerComparison1()
    {
        check("void f(std::set<int> &ints) {\n"
              "    for (std::set<int>::iterator it = ints.begin(); it != ints.end(); ++it) {\n"
              "        if (a) {\n"
              "            it++;\n"
              "        }\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("[test.cpp:4]: (warning) Missing bounds check for extra iterator increment in loop.\n", errout.str());
    }

    void missingInnerComparison2()
    {
        check("void f(std::set<int> &ints) {\n"
              "    for (std::set<int>::iterator it = ints.begin(); it != ints.end(); ++it) {\n"
              "        if (a) {\n"
              "            it++;\n"
              "            if (it == ints.end())\n"
              "                return;\n"
              "        }\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());
    }

    void missingInnerComparison3()
    {
        check("void f(std::set<int> &ints) {\n"
              "    for (std::set<int>::iterator it = ints.begin(); it != ints.end(); ++it) {\n"
              "        for (std::set<int>::iterator it = ints2.begin(); it != ints2.end(); ++it)\n"
              "        { }\n"
              "    }\n"
              "}\n");
        ASSERT_EQUALS("", errout.str());
    }

    void missingInnerComparison4()
    {
        check("function f1(std::list<int> &l1) {\n"
              "    for(std::list<int>::iterator i = l1.begin(); i != l1.end(); i++) {\n"
              "        if (*i == 44) {\n"
              "            l1.insert(++i, 55);\n"
              "            break;\n"
              "        }\n"
              "    }\n"
              "}");
        ASSERT_EQUALS("", errout.str());
    }

    void missingInnerComparison5()
    {
        check("void f() {\n"
              "    for(it = map1.begin(); it != map1.end(); it++) {\n"
              "        str[i++] = (*it).first;\n"
              "    }\n"
              "}");
        ASSERT_EQUALS("", errout.str());
    }

    void cstr()
    {
        check("void f() {\n"
              "    std::string errmsg;\n"
              "    throw errmsg.c_str();\n"
              "}");
        ASSERT_EQUALS("[test.cpp:3]: (error) Dangerous usage of c_str()\n", errout.str());

        check("void f() {\n"
              "    std::ostringstream errmsg;\n"
              "    const char *c = errmsg.str().c_str();\n"
              "}");
        ASSERT_EQUALS("[test.cpp:3]: (error) Dangerous usage of c_str()\n", errout.str());

        check("std::string f();\n"
              "\n"
              "void foo() {\n"
              "    const char *c = f().c_str();\n"
              "}");
        ASSERT_EQUALS("[test.cpp:4]: (error) Dangerous usage of c_str()\n", errout.str());
    }
};

REGISTER_TEST(TestStl)
