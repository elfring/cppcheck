/*
 * Cppcheck - A tool for static C/C++ code analysis
 * Copyright (C) 2018 Cppcheck team.
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

#ifndef ANALYSIS_RESULTS_MODEL_H
#define ANALYSIS_RESULTS_MODEL_H

#include <exception>
#include <QObject>
#include <QString>
#include <QStandardItem>
#include <QStandardItemModel>
#include "erroritem.h"
#include "report.h"

namespace analysis
{
typedef void (*callback_function)(void*);
typedef void (*callback_function2)(void*, void*);
typedef void (*const_callback_function)(void const *);
typedef void (*const_callback_function2)(void const *, void*);

struct report_read_failure : public std::runtime_error
{
    report_read_failure(void)
    : runtime_error(QObject::tr("Failed to read the report.").toStdString())
    { }
};

struct unsupported_report_format_version : public std::runtime_error
{
    unsupported_report_format_version(void)
    : runtime_error(QObject::tr("XML format version is not supported.").toStdString())
    { }
};

struct unmapped_model_index : public std::runtime_error
{
    unmapped_model_index(void)
    : runtime_error(QObject::tr("An index did not provide data from a model.").toStdString())
    { }
};

template<typename decider, typename checking>
bool check(decider const & d, checking const & ch, int index)
{
    return ch(d, index);
}

template<typename decider, typename checking>
bool check(decider const & d, checking const & ch, int index, QModelIndex const & parent)
{
    return ch(d, index, parent);
}

class error_item : public QStandardItem
{
    ErrorItem item;

public:
    typedef void (*callback_function)(ErrorItem*);
    typedef void (*callback_function2)(ErrorItem*, void*);
    typedef void (*const_callback_function)(ErrorItem const *);
    typedef void (*const_callback_function2)(ErrorItem const *, void*);

    explicit error_item(ErrorItem const & ei)
    : item(ei)
    { }

    virtual int type() const
    { return QStandardItem::UserType; }

    bool error_path_contains_data(void) const
    { return !item.errorPath.isEmpty(); }

    QString get_file_from_first_error_path(void) const
    { return item.errorPath[0].file; }

    void set_error_item(error_item const & ei)
    { item = ei.item; }

    /**
    * @brief Output data from this class.
    * @param r A reporting interface.
    *
    */
    void output(Report& r) const
    { r.writeError(item); }

    /**
    * @brief Perform an action on data from this class.
    *
    */
    void act_on(callback_function call)
    { (*call)(&item); }

    /**
    * @brief Perform an action on data from this class.
    * @param data Pointer to additional data.
    *
    */
    void act_on(callback_function2 call, void* data)
    { (*call)(&item, data); }

    /**
    * @brief Perform an action on read-only data from this class.
    *
    */
    void act_on(const_callback_function call) const
    { (*call)(&item); }

    /**
    * @brief Perform an action on read-only data from this class.
    * @param data Pointer to additional data.
    *
    */
    void act_on(const_callback_function2 call, void* data) const
    { (*call)(&item, data); }
};

/**
* @brief Model for analysis results
*
*/
class results_model : public QStandardItemModel
{
public:
    template<typename decider, typename checking>
    struct checks_base
    {
        decider& dec;
        checking const & ch;

        checks_base(decider& d_, checking const & ch_)
        : dec(d_), ch(ch_)
        { }

        virtual ~checks_base()
        { }

        virtual bool operator() (int index) const
        { return ch(dec, index); }

        virtual bool operator() (int index, QModelIndex const & parent) const
        { return ch(dec, index, parent); }
    };

    /**
    * @brief Read errors from report XML file.
    * @param file_name Report file to read.
    * @param call Pointer to a function which will be called for each data record.
    * @param data Pointer to additional data.
    *
    */
    void read_errors_XML(QString const file_name,
                         analysis::const_callback_function2 call,
                         void* data);

    /**
    * @brief Read errors from report XML file.
    * @param file_name Report file to read.
    *
    */
    void read_errors_XML(QString const file_name);

    /**
    * @brief Check if data were stored in this model.
    *
    */
    bool contains_data(void) const
    { return rowCount() > 0; }

    /**
    * @brief Check if data were stored in an error path.
    *
    */
    bool first_error_path_contains_data(void) const
    {
    auto ei(static_cast<error_item*>(item(0)));
    return ei ? ei->error_path_contains_data() : false;
    }

    /**
    * @brief Determine a file name from an error path.
    *
    */
    QString get_file_from_first_error_path(void) const
    {
    auto ei(static_cast<error_item*>(item(0)));
    if (!ei)
//        throw std::runtime_error(tr("Failed to determine an error item pointer.").toStdString());
        throw unmapped_model_index();

    return ei->get_file_from_first_error_path();
    }

    /**
    * @brief Output data from this class.
    * @param r A reporting interface.
    *
    */
    void output(Report& r) const
    {
    for (int x = 0; x < rowCount(); ++x) {
        auto ei(static_cast<error_item*>(item(x)));
        if (!ei)
           throw unmapped_model_index();

        ei->output(r);
    }
    }

    /**
    * @brief Output data from this class.
    * @param r A reporting interface.
    *
    */
    template<typename decider, typename checking>
    void output_unhidden_records(Report& r,
                                checks_base<decider, checking> const & c) const
    {
        for (int fx = 0; fx < rowCount(); ++fx) {
            if (c(fx)) {
                auto const * file(item(fx));
                if (!file)
                    throw unmapped_model_index();

                {
                    QModelIndex const fmi(file->index());
                    for (int ex = 0; ex < file->rowCount(); ++ex) {
                        auto const * error(file->child(ex));
                        if (!error)
                            throw unmapped_model_index();

                        if (c(ex, fmi)) {
                            auto const * ei(static_cast<error_item const *>(error));
                            ei->output(r);
                        }
                    }
                }
            }
        }
    }
};

/**
* @brief Create a model from an error report XML file.
* @param file_name Report file to read.
*
*/
results_model*
read_errors_XML(QString const file_name);

/**
* @brief Create a model from an error report XML file and perform another
*        action on each new data record.
* @param file_name Report file to read.
*
*/
results_model*
read_errors_XML(QString const file_name,
                analysis::const_callback_function2 call,
                void* data);

/**
* @brief Add data to a model from an error report XML file.
* @param rm Reference to an existing analysis results model.
* @param file_name Report file to read.
*
*/
results_model&
read_errors_XML(results_model& rm, QString const file_name);
}
#endif
