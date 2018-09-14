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

#include "analysis_results_model.h"
#include "xmlreport.h"
#include "xmlreportv2.h"

#ifdef USE_CXX_MAKE_UNIQUE
#include <memory>
#else
#include <QScopedPointer>
#endif

namespace analysis
{
void results_model::read_errors_XML(QString const file_name)
{
    int const version(XmlReport::determineVersion(file_name));
    switch (version)
    {
    case 0:
        throw report_read_failure();

    case 1:
        throw unsupported_report_format_version();

    default:
        {
#ifdef USE_CXX_MAKE_UNIQUE
            auto report = std::make_unique<XmlReportV2>(file_name);
#else
            QScopedPointer<XmlReportV2> report(new XmlReportV2(file_name));
#endif
            if (!report->open())
               throw report_read_failure();

            auto errors(report->read());
            ErrorItem item;
            foreach (item, errors) {
               appendRow(new error_item(item));
            }
        }
    }
}

void results_model::read_errors_XML(QString const file_name,
                                    const_callback_function2 call,
                                    void* data)
{
    int const version(XmlReport::determineVersion(file_name));
    switch (version)
    {
    case 0:
        throw report_read_failure();

    case 1:
        throw unsupported_report_format_version();

    default:
        {
#ifdef USE_CXX_MAKE_UNIQUE
            auto report = std::make_unique<XmlReportV2>(file_name);
#else
            QScopedPointer<XmlReportV2> report(new XmlReportV2(file_name));
#endif
            if (!report->open())
               throw report_read_failure();

            auto errors(report->read());
            ErrorItem item;
            auto e_i_call((error_item::const_callback_function2) call);
            error_item* e_i;
            foreach (item, errors) {
               e_i = new error_item(item);
               appendRow(e_i);
               e_i->act_on(e_i_call, data);
            }
        }
    }
}

results_model*
read_errors_XML(QString const file_name)
{
    auto rm(new results_model);
    rm->read_errors_XML(file_name);
    return rm;
}

results_model*
read_errors_XML(QString const file_name, const_callback_function2 call, void* data)
{
    auto rm(new results_model);
    rm->read_errors_XML(file_name, call, data);
    return rm;
}

results_model&
read_errors_XML(results_model& rm, QString const file_name)
{
    rm.read_errors_XML(file_name);
    return rm;
}

} // end of namespace "analysis"
