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

#include "checkstl.h"
#include "token.h"
#include "executionpath.h"
#include <sstream>

// Register this check class (by creating a static instance of it)
namespace
{
CheckStl instance;
}


// Error message for bad iterator usage..
void CheckStl::invalidIteratorError(const Token *tok, const std::string &iteratorName)
{
    reportError(tok, Severity::error, "invalidIterator1", "Invalid iterator: " + iteratorName);
}

void CheckStl::iteratorsError(const Token *tok, const std::string &container1, const std::string &container2)
{
    reportError(tok, Severity::error, "iterators", "Same iterator is used with both " + container1 + " and " + container2);
}

// Error message used when dereferencing an iterator that has been erased..
void CheckStl::dereferenceErasedError(const Token *tok, const std::string &itername)
{
    reportError(tok, Severity::error, "eraseDereference", "Dereferenced iterator '" + itername + "' has been erased");
}

void CheckStl::eraseByValueError(const Token *tok, const std::string &containername, const std::string &itername)
{
    reportError(tok, Severity::error, "eraseByValue", "Iterator '" +  itername + "' becomes invalid when deleted by value from '" + containername +  "'");
}

void CheckStl::iterators()
{
    for (const Token *tok = _tokenizer->tokens(); tok; tok = tok->next())
    {
        if (!Token::Match(tok, "%var% = %var% . begin ( ) ;|+"))
            continue;

        const unsigned int iteratorId(tok->varId());
        const unsigned int containerId(tok->tokAt(2)->varId());
        if (iteratorId == 0 || containerId == 0)
            continue;

        bool validIterator = true;
        unsigned int indent = 0;
        for (const Token *tok2 = tok->tokAt(7); tok2; tok2 = tok2->next())
        {
            if (tok2->str() == "{" && ++indent)
                continue;
            if (tok2->str() == "}" && --indent == 0)
                break;

            if (Token::Match(tok2, "%varid% != %var% . end ( )", iteratorId) && tok2->tokAt(2)->varId() != containerId)
            {
                iteratorsError(tok2, tok->strAt(2), tok2->strAt(2));
                tok2 = tok2->tokAt(6);
            }
            else if (Token::Match(tok2, "%var% . insert|erase ( %varid% )|,", iteratorId))
            {
                if (!validIterator)
                    invalidIteratorError(tok2, tok2->strAt(4));

                if (tok2->varId() != containerId && tok2->tokAt(5)->str() != ".")
                {
                    // skip error message if container is a set..
                    if (tok2->varId() > 0)
                    {
                        const Token *decltok = Token::findmatch(_tokenizer->tokens(), "%varid%", tok2->varId());
                        while (decltok && !Token::Match(decltok, "[;{},(]"))
                            decltok = decltok->previous();
                        if (Token::Match(decltok, "%any% const| std :: set"))
                            continue;	// No warning
                    }

                    // Show error message, mismatching iterator is used.
                    iteratorsError(tok2, tok->strAt(2), tok2->str());
                }
                else if (tok2->strAt(2) == std::string("erase"))
                    validIterator = false;

                tok2 = tok2->tokAt(4);
            }
            else if (Token::Match(tok2, "%varid% = %var% . erase (", iteratorId))
            {
                validIterator = true;
                tok2 = tok2->tokAt(5)->link();
                if (!tok2)
                    break;
            }
            else if (Token::Match(tok2, "%varid% = %var% ;", iteratorId))
            {
                validIterator = true;
                tok2 = tok2->tokAt(2);
            }
            else if (!validIterator && Token::Match(tok2, "* %varid%", iteratorId))
            {
                dereferenceErasedError(tok2, tok2->strAt(1));
                tok2 = tok2->next();
            }
            else if (!validIterator && Token::Match(tok2, "%varid% . %var%", iteratorId))
            {
                dereferenceErasedError(tok2, tok2->strAt(0));
                tok2 = tok2->tokAt(2);
            }
            else if (Token::Match(tok2, "%var% . erase ( * %varid%", iteratorId) && tok2->varId() == containerId)
            {
                eraseByValueError(tok2, tok2->strAt(0), tok2->strAt(5));
            }
            else if (Token::Match(tok2, "return|break ;"))
            {
                validIterator = true;
            }
            else if (tok2->str() == "else")
            {
                validIterator = true;
            }
        }
    }
}


// Error message for bad iterator usage..
void CheckStl::mismatchingContainersError(const Token *tok)
{
    reportError(tok, Severity::error, "mismatchingContainers", "mismatching containers");
}

void CheckStl::mismatchingContainers()
{
    for (const Token *tok = _tokenizer->tokens(); tok; tok = tok->next())
    {
        if (tok->str() != "std")
            continue;

        if (Token::Match(tok, "std :: find|find_if|count|transform|replace|replace_if|sort ( %var% . begin|rbegin ( ) , %var% . end|rend ( ) ,"))
        {
            if (tok->tokAt(4)->str() != tok->tokAt(10)->str())
            {
                mismatchingContainersError(tok);
            }
        }
    }
}


void CheckStl::stlOutOfBounds()
{
    // Scan through all tokens..
    for (const Token *tok = _tokenizer->tokens(); tok; tok = tok->next())
    {
        // only interested in "for" loops
        if (!Token::simpleMatch(tok, "for ("))
            continue;

        // check if the for loop condition is wrong
        unsigned int indent = 0;
        for (const Token *tok2 = tok->tokAt(2); tok2; tok2 = tok2->next())
        {

            if (tok2->str() == "(")
                ++indent;

            else if (tok2->str() == ")")
            {
                if (indent == 0)
                    break;
                --indent;
            }

            if (Token::Match(tok2, "; %var% <= %var% . size ( ) ;"))
            {
                unsigned int indent2 = 0;
                unsigned int numId = tok2->tokAt(1)->varId();
                unsigned int varId = tok2->tokAt(3)->varId();
                for (const Token *tok3 = tok2->tokAt(8); tok3; tok3 = tok3->next())
                {
                    if (tok3->str() == "{")
                        ++indent2;
                    else if (tok3->str() == "}")
                    {
                        if (indent2 <= 1)
                            break;
                        --indent2;
                    }
                    else if (tok3->varId() == varId)
                    {
                        if (Token::simpleMatch(tok3->next(), ". size ( )"))
                            break;
                        else if (Token::Match(tok3->next(), "[ %varid% ]", numId))
                            stlOutOfBoundsError(tok3, tok3->tokAt(2)->str(), tok3->str());
                    }
                }
                break;
            }
        }
    }
}

// Error message for bad iterator usage..
void CheckStl::stlOutOfBoundsError(const Token *tok, const std::string &num, const std::string &var)
{
    reportError(tok, Severity::error, "stlOutOfBounds", "When " + num + "==" + var + ".size(), " + var + "[" + num + "] is out of bounds");
}


/**
 * @brief %Check for invalid iterator usage after erase/insert/etc
 */
class EraseCheckLoop : public ExecutionPath
{
public:
    static void checkScope(CheckStl *checkStl, const Token *it)
    {
        const Token *tok = it;

        // Search for the start of the loop body..
        int indentlevel = 1;
        while (indentlevel > 0 && 0 != (tok = tok->next()))
        {
            if (tok->str() == "(")
                ++indentlevel;
            else if (tok->str() == ")")
                --indentlevel;
        }

        if (! Token::simpleMatch(tok, ") {"))
            return;

        EraseCheckLoop c(checkStl, it->varId());
        std::list<ExecutionPath *> checks;
        checks.push_back(c.copy());
        ExecutionPath::checkScope(tok->tokAt(2), checks);

        c.end(checks, tok->link());

        while (!checks.empty())
        {
            delete checks.back();
            checks.pop_back();
        }
    }

private:
    /** Startup constructor */
    EraseCheckLoop(Check *o, unsigned int varid)
        : ExecutionPath(o, varid), eraseToken(0)
    {
    }

    /** @brief token where iterator is erased (non-zero => the iterator is invalid) */
    const Token *eraseToken;

    /** @brief Copy this check. Called from the ExecutionPath baseclass. */
    ExecutionPath *copy()
    {
        return new EraseCheckLoop(*this);
    }

    /** @brief is another execution path equal? */
    bool is_equal(const ExecutionPath *e) const
    {
        const EraseCheckLoop *c = static_cast<const EraseCheckLoop *>(e);
        return (eraseToken == c->eraseToken);
    }

    /** @brief no implementation => compiler error if used by accident */
    void operator=(const EraseCheckLoop &);

    /** @brief parse tokens */
    const Token *parse(const Token &tok, std::list<ExecutionPath *> &checks) const
    {
        // bail out if there are assignments. We don't check the assignments properly.
        if (Token::Match(&tok, "[;{}] %var% =") || Token::Match(&tok, "= %var% ;"))
        {
            ExecutionPath::bailOutVar(checks, tok.next()->varId());
        }

        // the loop stops here. Bail out all execution checks that reach
        // this statement
        if (Token::Match(&tok, "[;{}] break ;"))
        {
            ExecutionPath::bailOut(checks);
        }

        // erasing iterator => it is invalidated
        if (Token::Match(&tok, "erase ( ++|--| %var% )"))
        {
            // check if there is a "it = ints.erase(it);" pattern. if so
            // the it is not invalidated.
            const Token *token = &tok;
            while (NULL != (token = token ? token->previous() : 0))
            {
                if (Token::Match(token, "[;{}]"))
                    break;
                else if (token->str() == "=")
                    token = 0;
            }

            // the it is invalidated by the erase..
            if (token)
            {
                // get variable id for the iterator
                unsigned int iteratorId = 0;
                if (tok.tokAt(2)->isName())
                    iteratorId = tok.tokAt(2)->varId();
                else
                    iteratorId = tok.tokAt(3)->varId();

                // invalidate this iterator in the corresponding checks
                for (std::list<ExecutionPath *>::const_iterator it = checks.begin(); it != checks.end(); ++it)
                {
                    EraseCheckLoop *c = dynamic_cast<EraseCheckLoop *>(*it);
                    if (c && c->varId == iteratorId)
                    {
                        c->eraseToken = &tok;
                    }
                }
            }
        }

        // don't skip any tokens. return the token that we received.
        return &tok;
    }

    /**
     * Parse condition. @sa ExecutionPath::parseCondition
     * @param tok first token in condition.
     * @param checks The execution paths. All execution paths in the list are executed in the current scope
     * @return true => bail out all checking
     **/
    bool parseCondition(const Token &tok, std::list<ExecutionPath *> &checks)
    {
        // no checking of conditions.
        (void)tok;
        (void)checks;
        return false;
    }

    /** @brief going out of scope - all execution paths end */
    void end(const std::list<ExecutionPath *> &checks, const Token * /*tok*/) const
    {
        // check if there are any invalid iterators. If so there is an error.
        for (std::list<ExecutionPath *>::const_iterator it = checks.begin(); it != checks.end(); ++it)
        {
            EraseCheckLoop *c = dynamic_cast<EraseCheckLoop *>(*it);
            if (c && c->eraseToken)
            {
                CheckStl *checkStl = dynamic_cast<CheckStl *>(c->owner);
                if (checkStl)
                {
                    checkStl->eraseError(c->eraseToken);
                }
            }
        }
    }
};


void CheckStl::erase()
{
    for (const Token *tok = _tokenizer->tokens(); tok; tok = tok->next())
    {
        if (Token::simpleMatch(tok, "for ("))
        {
            for (const Token *tok2 = tok->tokAt(2); tok2; tok2 = tok2->next())
            {
                if (tok2->str() == ";")
                {
                    if (Token::Match(tok2, "; %var% !="))
                    {
                        const unsigned int varid = tok2->next()->varId();
                        if (varid > 0 && Token::findmatch(_tokenizer->tokens(), "> :: iterator %varid%", varid))
                            EraseCheckLoop::checkScope(this, tok2->next());
                    }
                    break;
                }

                if (Token::Match(tok2, "%var% = %var% . begin ( ) ; %var% != %var% . end ( ) ") &&
                    tok2->str() == tok2->tokAt(8)->str() &&
                    tok2->tokAt(2)->str() == tok2->tokAt(10)->str())
                {
                    EraseCheckLoop::checkScope(this, tok2);
                    break;
                }
            }
        }

        if (Token::Match(tok, "while ( %var% !="))
        {
            const unsigned int varid = tok->tokAt(2)->varId();
            if (varid > 0 && Token::findmatch(_tokenizer->tokens(), "> :: iterator %varid%", varid))
                EraseCheckLoop::checkScope(this, tok->tokAt(2));
        }
    }
}


// Error message for bad iterator usage..
void CheckStl::eraseError(const Token *tok)
{
    reportError(tok, Severity::error, "erase", "Dangerous iterator usage. After erase the iterator is invalid so dereferencing it or comparing it with another iterator is invalid.");
}



void CheckStl::pushback()
{
    // Pointer can become invalid after push_back or push_front..
    for (const Token *tok = _tokenizer->tokens(); tok; tok = tok->next())
    {
        if (Token::Match(tok, "%var% = & %var% ["))
        {
            const unsigned int pointerId(tok->varId());
            const unsigned int containerId(tok->tokAt(3)->varId());
            if (pointerId == 0 || containerId == 0)
                continue;

            int indent = 0;
            bool invalidPointer = false;
            for (const Token *tok2 = tok; indent >= 0 && tok2; tok2 = tok2->next())
            {
                if (tok2->str() == "{" || tok2->str() == "(")
                    ++indent;
                else if (tok2->str() == "}" || tok2->str() == ")")
                {
                    if (indent == 0 && Token::simpleMatch(tok2, ") {"))
                        tok2 = tok2->next();
                    else
                        --indent;
                }

                // push_back on vector..
                if (Token::Match(tok2, "%varid% . push_front|push_back", containerId))
                    invalidPointer = true;

                // Using invalid pointer..
                if (invalidPointer && tok2->varId() == pointerId)
                {
                    if (tok2->previous()->str() == "*")
                        invalidPointerError(tok2, tok2->str());
                    else if (tok2->next()->str() == ".")
                        invalidPointerError(tok2, tok2->str());
                    break;
                }
            }
        }
    }

    // Iterator becomes invalid after reserve, push_back or push_front..
    for (const Token *tok = _tokenizer->tokens(); tok; tok = tok->next())
    {
        if (!Token::simpleMatch(tok, "vector <"))
            continue;

        // if iterator declaration inside for() loop
        bool iteratorDeclaredInsideLoop = false;
        if ((tok->tokAt(-2) && Token::simpleMatch(tok->tokAt(-2), "for (")) ||
            (tok->tokAt(-4) && Token::simpleMatch(tok->tokAt(-4), "for ( std ::")))
        {
            iteratorDeclaredInsideLoop = true;
        }

        while (tok && tok->str() != ">")
            tok = tok->next();
        if (!tok)
            break;
        if (!Token::Match(tok, "> :: iterator|const_iterator %var% =|;"))
            continue;

        const unsigned int iteratorid(tok->tokAt(3)->varId());
        if (iteratorid == 0)
            continue;

        if (iteratorDeclaredInsideLoop && tok->tokAt(4)->str() == "=")
        {
            // skip "> :: iterator|const_iterator"
            tok = tok->tokAt(3);
        }

        unsigned int vectorid = 0;
        int indent = 0;
        std::string invalidIterator;
        for (const Token *tok2 = tok; indent >= 0 && tok2; tok2 = tok2->next())
        {
            if (tok2->str() == "{" || tok2->str() == "(")
                ++indent;
            else if (tok2->str() == "}" || tok2->str() == ")")
            {
                if (indent == 0 && Token::simpleMatch(tok2, ") {"))
                    tok2 = tok2->next();
                else
                    --indent;
            }

            // Using push_back or push_front inside a loop..
            if (Token::Match(tok2, "for ("))
            {
                tok2 = tok2->tokAt(2);
            }

            if (Token::Match(tok2, "%varid% = %var% . begin ( ) ; %varid% != %var% . end ( ) ; ++| %varid% ++| ) {", iteratorid))
            {
                const unsigned int varId(tok2->tokAt(2)->varId());
                if (varId == 0)
                    continue;

                const Token *pushbackTok = 0;
                unsigned int indent3 = 0;
                for (const Token *tok3 = tok2->tokAt(20); tok3; tok3 = tok3->next())
                {
                    if (tok3->str() == "{")
                        ++indent3;
                    else if (tok3->str() == "}")
                    {
                        if (indent3 <= 1)
                            break;
                        --indent3;
                    }
                    else if (tok3->str() == "break")
                    {
                        pushbackTok = 0;
                        break;
                    }
                    else if (Token::Match(tok3, "%varid% . push_front|push_back|insert|reserve (", varId))
                    {
                        pushbackTok = tok3->tokAt(2);
                    }
                }

                if (pushbackTok)
                    invalidIteratorError(pushbackTok, pushbackTok->str(), tok2->strAt(0));
            }

            // Assigning iterator..
            if (Token::Match(tok2, "%varid% =", iteratorid))
            {
                if (Token::Match(tok2->tokAt(2), "%var% . begin|end|rbegin|rend ( )"))
                {
                    vectorid = tok2->tokAt(2)->varId();
                    tok2 = tok2->tokAt(6);
                }
                else
                {
                    vectorid = 0;
                }
                invalidIterator = "";
            }

            // push_back on vector..
            if (vectorid > 0 && Token::Match(tok2, "%varid% . push_front|push_back|insert|reserve (", vectorid))
            {
                if (!invalidIterator.empty() && Token::Match(tok2->tokAt(2), "insert ( %varid% ,", iteratorid))
                {
                    invalidIteratorError(tok2, invalidIterator, tok2->strAt(4));
                    break;
                }

                invalidIterator = tok2->strAt(2);
                if (!iteratorDeclaredInsideLoop)
                {
                    tok2 = tok2->tokAt(3)->link();
                    if (!tok2)
                        break;
                }
            }

            else if (tok2->str() == "return")
            {
                invalidIterator.clear();
            }

            // Using invalid iterator..
            if (!invalidIterator.empty())
            {
                if (Token::Match(tok2, "++|--|*|+|-|(|,|=|!= %varid%", iteratorid))
                    invalidIteratorError(tok2, invalidIterator, tok2->strAt(1));
                if (Token::Match(tok2, "%varid% ++|--|+|-", iteratorid))
                    invalidIteratorError(tok2, invalidIterator, tok2->str());
            }
        }
    }
}


// Error message for bad iterator usage..
void CheckStl::invalidIteratorError(const Token *tok, const std::string &func, const std::string &iterator_name)
{
    reportError(tok, Severity::error, "invalidIterator2", "After " + func + ", the iterator '" + iterator_name + "' may be invalid");
}


// Error message for bad iterator usage..
void CheckStl::invalidPointerError(const Token *tok, const std::string &pointer_name)
{
    reportError(tok, Severity::error, "invalidPointer", "Invalid pointer '" + pointer_name + "' after push_back / push_front");
}




void CheckStl::stlBoundries()
{
    // containers (not the vector)..
    static const char STL_CONTAINER_LIST[] = "bitset|deque|list|map|multimap|multiset|priority_queue|queue|set|stack|hash_map|hash_multimap|hash_set";

    for (const Token *tok = _tokenizer->tokens(); tok; tok = tok->next())
    {
        // Declaring iterator..
        const std::string checkStr = (std::string(STL_CONTAINER_LIST) + " <");
        if (Token::Match(tok, checkStr.c_str()))
        {
            const std::string container_name(tok->strAt(0));
            while (tok && tok->str() != ">")
                tok = tok->next();
            if (!tok)
                break;

            if (Token::Match(tok, "> :: iterator|const_iterator %var% =|;"))
            {
                const unsigned int iteratorid(tok->tokAt(3)->varId());
                if (iteratorid == 0)
                    continue;

                // Using "iterator < ..." is not allowed
                unsigned int indentlevel = 0;
                for (const Token *tok2 = tok; tok2; tok2 = tok2->next())
                {
                    if (tok2->str() == "{")
                        ++indentlevel;
                    else if (tok2->str() == "}")
                    {
                        if (indentlevel == 0)
                            break;
                        --indentlevel;
                    }
                    else if (Token::Match(tok2, "!!* %varid% <", iteratorid))
                    {
                        stlBoundriesError(tok2, container_name);
                    }
                }
            }
        }
    }
}

// Error message for bad boundry usage..
void CheckStl::stlBoundriesError(const Token *tok, const std::string &container_name)
{
    reportError(tok, Severity::error, "stlBoundries", container_name + " range check should use != and not < since the order of the pointers isn't guaranteed");
}



void CheckStl::if_find()
{
    if (!_settings->_checkCodingStyle)
        return;
    for (const Token *tok = _tokenizer->tokens(); tok; tok = tok->next())
    {
        if (Token::Match(tok, "if ( !| %var% . find ( %any% ) )"))
        {
            // goto %var%
            tok = tok->tokAt(2);
            if (!tok->isName())
                tok = tok->next();

            const unsigned int varid = tok->varId();
            if (varid > 0)
            {
                // Is the variable a std::string or STL container?
                const Token * decl = Token::findmatch(_tokenizer->tokens(), "%varid%", varid);
                while (decl && !Token::Match(decl, "[;{}(,]"))
                    decl = decl->previous();

                decl = decl->next();

                // stl container
                if (Token::Match(decl, "const| std :: %var% < %type% > &|*| %varid%", varid))
                    if_findError(tok, false);
                else if (Token::Match(decl, "const| std :: string &|*| %varid%", varid))
                    if_findError(tok, true);
            }
        }

        if (Token::Match(tok, "if ( !| std :: find|find_if ("))
        {
            // goto '(' for the find
            tok = tok->tokAt(4);
            if (tok->isName())
                tok = tok->next();

            // check that result is checked properly
            if (Token::simpleMatch(tok->link(), ") )"))
            {
                if_findError(tok, false);
            }
        }
    }
}


void CheckStl::if_findError(const Token *tok, bool str)
{
    if (str)
        reportError(tok, Severity::warning, "stlIfStrFind", "Suspicious condition. string::find will return 0 if the string is found at position 0. If this is what you want to check then string::compare is a faster alternative because it doesn't scan through the string.");
    else
        reportError(tok, Severity::warning, "stlIfFind", "Suspicious condition. The result of find is an iterator, but it is not properly checked.");
}



bool CheckStl::isStlContainer(const Token *tok)
{
    // check if this token is defined
    if (tok->varId())
    {
        // find where this token is defined
        const Token *type = Token::findmatch(_tokenizer->tokens(), "%varid%", tok->varId());

        // find where this tokens type starts
        while (type->previous() && !Token::Match(type->previous(), "[;{,(]"))
            type = type->previous();

        // ignore "const"
        if (type->str() == "const")
            type = type->next();

        // discard namespace if supplied
        if (Token::simpleMatch(type, "std ::"))
            type = type->next()->next();

        // all possible stl containers
        static const char STL_CONTAINER_LIST[] = "bitset|deque|list|map|multimap|multiset|priority_queue|queue|set|stack|hash_map|hash_multimap|hash_set|vector";

        // container template string
        const std::string checkStr(std::string(STL_CONTAINER_LIST) + " <");

        // check if it's an stl template
        if (Token::Match(type, checkStr.c_str()))
            return true;
    }

    return false;
}

void CheckStl::size()
{
    if (!_settings->_checkCodingStyle || !_settings->inconclusive)
        return;

    for (const Token *tok = _tokenizer->tokens(); tok; tok = tok->next())
    {
        if (Token::Match(tok, "%var% . size ( )"))
        {
            if (Token::Match(tok->tokAt(5), "==|!=|> 0"))
            {
                if (isStlContainer(tok))
                    sizeError(tok);
            }
            else if ((tok->tokAt(5)->str() == ")" ||
                      tok->tokAt(5)->str() == "&&" ||
                      tok->tokAt(5)->str() == "||" ||
                      tok->tokAt(5)->str() == "!") &&
                     (tok->tokAt(-1)->str() == "(" ||
                      tok->tokAt(-1)->str() == "&&" ||
                      tok->tokAt(-1)->str() == "||" ||
                      tok->tokAt(-1)->str() == "!"))
            {
                if (tok->tokAt(-1)->str() == "(" &&
                    tok->tokAt(5)->str() == ")")
                {
                    // check for passing size to function call
                    if (Token::Match(tok->tokAt(-2), "if|while"))
                    {
                        if (isStlContainer(tok))
                            sizeError(tok);
                    }
                }
                else if (isStlContainer(tok))
                    sizeError(tok);
            }
        }
    }
}

void CheckStl::sizeError(const Token *tok)
{
    const std::string varname(tok ? tok->str().c_str() : "list");
    reportError(tok, Severity::performance, "stlSize",
                "Possible inefficient checking for '" + varname + "' emptiness.\n"
                "Using " + varname + ".empty() instead of " + varname + ".size() can be faster. " +
                varname + ".size() can take linear time but " + varname + ".empty() is "
                "guaranteed to take constant time.");
}

void CheckStl::redundantCondition()
{
    const char pattern[] = "if ( %var% . find ( %any% ) != %var% . end ( ) ) "
                           "{|{|"
                           "    %var% . remove ( %any% ) ; "
                           "}|}|";
    const Token *tok = Token::findmatch(_tokenizer->tokens(), pattern);
    while (tok)
    {
        bool b(tok->tokAt(15)->str() == "{");

        // Get tokens for the fields %var% and %any%
        const Token *var1 = tok->tokAt(2);
        const Token *any1 = tok->tokAt(6);
        const Token *var2 = tok->tokAt(9);
        const Token *var3 = tok->tokAt(b ? 16 : 15);
        const Token *any2 = tok->tokAt(b ? 20 : 19);

        // Check if all the "%var%" fields are the same and if all the "%any%" are the same..
        if (var1->str() == var2->str() &&
            var2->str() == var3->str() &&
            any1->str() == any2->str())
        {
            redundantIfRemoveError(tok);
        }

        tok = Token::findmatch(tok->next(), pattern);
    }
}

void CheckStl::redundantIfRemoveError(const Token *tok)
{
    reportError(tok, Severity::style, "redundantIfRemove", "Redundant checking of STL container element.\n"
                "The remove method in the STL will not do anything if element doesn't exist");
}

void CheckStl::missingComparison()
{
    for (const Token *tok = _tokenizer->tokens(); tok; tok = tok->next())
    {
        if (Token::simpleMatch(tok, "for ("))
        {
            for (const Token *tok2 = tok->tokAt(2); tok2; tok2 = tok2->next())
            {
                if (tok2->str() == ";")
                    break;

                if (!Token::Match(tok2, "%var% = %var% . begin ( ) ; %var% != %var% . end ( ) ; ++| %var% ++| ) {"))
                    continue;

                // same iterator name
                if (tok2->str() != tok2->strAt(8))
                    continue;

                // same container
                if (tok2->strAt(2) != tok2->strAt(10))
                    continue;

                // increment iterator
                if (!Token::simpleMatch(tok2->tokAt(16), ("++ " + tok2->str() + " )").c_str()) &&
                    !Token::simpleMatch(tok2->tokAt(16), (tok2->str() + " ++ )").c_str()))
                {
                    continue;
                }

                const unsigned int &iteratorId(tok2->varId());
                if (iteratorId == 0)
                    continue;

                const Token *incrementToken = 0;
                unsigned int indentlevel = 0;
                // Parse loop..
                for (const Token *tok3 = tok2->tokAt(20); tok3; tok3 = tok3->next())
                {
                    if (tok3->str() == "{")
                        ++indentlevel;
                    else if (tok3->str() == "}")
                    {
                        if (indentlevel == 0)
                            break;
                        --indentlevel;
                    }
                    else if (tok3->varId() == iteratorId && Token::simpleMatch(tok3->next(), "++"))
                        incrementToken = tok3;
                    else if (tok3->str() == "++" && tok3->next() && tok3->next()->varId() == iteratorId)
                        incrementToken = tok3;
                    else if (tok3->varId() == iteratorId && Token::Match(tok3->next(), "!=|=="))
                        incrementToken = 0;
                    else if (tok3->str() == "break")
                        incrementToken = 0;
                }
                if (incrementToken)
                    missingComparisonError(incrementToken, tok2->tokAt(16));
            }
        }
    }
}

void CheckStl::missingComparisonError(const Token *incrementToken1, const Token *incrementToken2)
{
    std::ostringstream errmsg;
    errmsg << "Missing bounds check for extra iterator increment in loop.\n"
           << "The iterator incrementing is suspicious - it is incremented at line "
           << incrementToken1->linenr() << " and then at line " << incrementToken2->linenr()
           << " The loop might unintentionally skip an element in the container. "
           << "There is no comparison between these increments to prevent that the iterator is "
           << "incremented beyond the end.";

    reportError(incrementToken1, Severity::warning, "StlMissingComparison", errmsg.str());
}


void CheckStl::string_c_str()
{
    // Try to detect common problems when using string::c_str()
    for (const Token *tok = _tokenizer->tokens(); tok; tok = tok->next())
    {
        // Locate executable scopes:
        if (Token::Match(tok, ") const| {"))
        {
            std::set<unsigned int> localvar;
            std::set<unsigned int> pointers;

            // scan through this executable scope:
            unsigned int indentlevel = 0;
            while (NULL != (tok = tok->next()))
            {
                if (tok->str() == "{")
                    ++indentlevel;
                else if (tok->str() == "}")
                {
                    if (indentlevel <= 1)
                        break;
                    --indentlevel;
                }

                // Variable declarations..
                else if (Token::Match(tok->previous(), "[;{}] std :: %type% %var% ;"))
                    localvar.insert(tok->tokAt(3)->varId());
                else if (Token::Match(tok->previous(), "[;{}] %type% %var% ;"))
                    localvar.insert(tok->next()->varId());
                else if (Token::Match(tok->previous(), "[;{}] %type% * %var% ;"))
                    pointers.insert(tok->tokAt(2)->varId());
                else if (Token::Match(tok->previous(), "[;{}] %type% %type% * %var% ;"))
                    pointers.insert(tok->tokAt(3)->varId());

                // Invalid usage..
                else if (Token::Match(tok, "throw %var% . c_str ( ) ;") &&
                         tok->next()->varId() > 0 &&
                         localvar.find(tok->next()->varId()) != localvar.end())
                {
                    string_c_strError(tok);
                }
                else if (Token::Match(tok, "[;{}] %var% = %var% . str ( ) . c_str ( ) ;") &&
                         tok->next()->varId() > 0 &&
                         pointers.find(tok->next()->varId()) != pointers.end())
                {
                    string_c_strError(tok);
                }
                else if (Token::Match(tok, "[;{}] %var% = %var% (") &&
                         Token::Match(tok->tokAt(4)->link(), ") . c_str ( ) ;") &&
                         tok->next()->varId() > 0 &&
                         pointers.find(tok->next()->varId()) != pointers.end() &&
                         Token::findmatch(_tokenizer->tokens(), ("std :: string " + tok->strAt(3) + " (").c_str()))
                {
                    string_c_strError(tok);
                }
            }
        }
    }
}

void CheckStl::string_c_strError(const Token *tok)
{
    reportError(tok, Severity::error, "stlcstr", "Dangerous usage of c_str()");
}

