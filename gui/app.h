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

#ifndef GUI_APPLICATION_H
#define GUI_APPLICATION_H

#include <QApplication>
#include <QStandardItemModel>
#include "analysis_results_model.h"

#ifdef USE_CXX_MAKE_UNIQUE
#include <memory>
#else
#include <QScopedPointer>
#endif

namespace analysis
{
namespace GUI
{
class application : public QApplication
{
#ifdef USE_CXX_MAKE_UNIQUE
    std::unique_ptr<QStandardItemModel> sim;
    std::unique_ptr<results_model> rm;
#else
    QScopedPointer<QStandardItemModel> sim;
    QScopedPointer<results_model> rm;
#endif

    bool use_analysis_results_model = true;

public:
    application(int& argc, char** argv);

    /**
    * @brief Manage another model for analysis results.
    *
    */
    void add_analysis_results_model(void);

    /**
    * @brief Manage an other model.
    * @param rm_ A model for analysis results.
    *
    */
    void use_model(results_model* rm_);
    void use_model(QStandardItemModel* sim_);

    /**
    * @brief Determine the used model instance.
    *
    */
    results_model* get_analysis_results_model(void) const
    {
    return
#ifdef USE_CXX_MAKE_UNIQUE
    rm.get()
#else
    rm.data()
#endif
    ;
    }

    /**
    * @brief Determine the used model instance.
    *
    */
    QStandardItemModel* get_standard_item_model(void) const
    {
    return use_analysis_results_model
    ?
#ifdef USE_CXX_MAKE_UNIQUE
    rm.get()
#else
    rm.data()
#endif
    :
#ifdef USE_CXX_MAKE_UNIQUE
    sim.get()
#else
    sim.data()
#endif
    ;
    }
};

}
}
#endif
