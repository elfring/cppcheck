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

#include <QTextCodec>
#include "app.h"

namespace analysis
{
namespace GUI
{
application::application(int& argc, char** argv)
: QApplication(argc, argv)
#ifndef USE_CXX_MAKE_UNIQUE
  , rm(Q_NULLPTR), sim(Q_NULLPTR)
#endif
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

#if QT_VERSION < 0x050000
    // Set codecs so that UTF-8 strings in sources are handled correctly.
    // This is ONLY needed for Qt versions 4.x.
    // Qt 5.x assumes UTF-8 by default.
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif

    setOrganizationName("Cppcheck");
    setApplicationName("Cppcheck-GUI");
}

void application::add_analysis_results_model(void)
{
    rm.reset(new results_model);
}

void application::use_model(results_model* rm_)
{
    rm.reset(rm_);
    use_analysis_results_model = true;
}

void application::use_model(QStandardItemModel* sim_)
{
    sim.reset(sim_);
    use_analysis_results_model = false;
}

} // end of namespace "GUI"
} // end of namespace "analysis"
