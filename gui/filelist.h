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

#ifndef FILELIST_H
#define FILELIST_H

#include <QList>
#include <QFileInfoList>
#include <QStringList>

/**
 * @brief A class for listing files and directories to check.
 * This class creates a list of files to check. If directory name is given then
 * all files in the directory matching the filter will be added. The directory
 * can be also added recursively when all files in subdirectories are added too.
 * The filenames are matched against the filter and only those files whose
 * filename extension is included in the filter list are added.
 *
 * This class also handles filtering of paths against ignore filters given. If
 * there is ignore filters then only paths not matching those filters are
 * returned.
 */
class FileList
{
public:

    /**
    * @brief Add filename to the list.
    * @param filepath Full path to the file.
    */
    void AddFile(const QString &filepath);

    /**
    * @brief Add files in the directory to the list.
    * @param directory Full pathname to directory to add.
    * @param recursive If true also files in subdirectories are added.
    */
    void AddDirectory(const QString &directory, bool recursive = false);

    /**
    * @brief Add list of filenames and directories to the list.
    * @param paths List of paths to add.
    */
    void AddPathList(const QStringList &paths);

    /**
    * @brief Return list of filenames (to check).
    * @return list of filenames to check.
    */
    QStringList GetFileList() const;

    /**
    * @brief Add list of paths to ignore list.
    * @param paths Paths to ignore.
    */
    void AddIngoreList(const QStringList &paths);

protected:

    /**
    * @brief Return list of default filename extensions included.
    * @return list of default filename extensions included.
    */
    static QStringList GetDefaultFilters();

    /**
    * @brief Test if filename matches the filename extensions filtering.
    * @param true if filename matches filtering.
    */
    bool FilterMatches(const QFileInfo &inf);

    /**
    * @brief Get filtered list of paths.
    * This method takes the list of paths and applies the ignore lists to
    * it. And then returns the list of paths that did not match the
    * ignore filters.
    * @return Filtered list of paths.
    */
    QStringList ApplyIgnoreList() const;

    /**
    * @brief Test if path matches any of the ignore filters.
    * @param path Path to test against filters.
    * @return true if any of the filters matches, false otherwise.
    */
    bool Match(const QString &path) const;

private:
    QFileInfoList mFileList;
    QStringList mIgnoredPaths;
};

#endif // FILELIST_H
