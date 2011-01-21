/*
 * Cppcheck - A tool for static C/C++ code analysis
 * Copyright (C) 2007-2011 Daniel Marjamäki and Cppcheck team.
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
#ifndef CPPCHECK_LIBRARY_VERSION_H_F844618C72C64C11B739E6759CC90245
#define CPPCHECK_LIBRARY_VERSION_H_F844618C72C64C11B739E6759CC90245

#define CPPCHECK_LIBRARY_VERSION_MAJOR 1
#define CPPCHECK_LIBRARY_VERSION_MINOR 47
#define CPPCHECK_LIBRARY_VERSION_PATCH 0

#define CPPCHECK_LIBRARY_VERSION_CONVERT_TO_STRING(X) #X

#define CPPCHECK_LIBRARY_VERSION_STRING(MAJOR, MINOR, PATCH) CPPCHECK_LIBRARY_VERSION_CONVERT_TO_STRING(MAJOR) \
        "." CPPCHECK_LIBRARY_VERSION_CONVERT_TO_STRING(MINOR) \
        "." CPPCHECK_LIBRARY_VERSION_CONVERT_TO_STRING(PATCH)

#endif
