/*
 * Cppcheck - A tool for static C/C++ code analysis
 * Copyright (C) 2007-2011 Daniel Marjam�ki and Cppcheck team.
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

#include <QtTest>
#include <QObject>
#include "testxmlreportv1.h"
#include "xmlreportv1.h"
#include "erroritem.h"

void TestXmlReportV1::readXml()
{
    const QString filepath("xmlfiles/xmlreport_v1.xml");
    XmlReportV1 report(filepath);
    QVERIFY(report.Open());
    QList<ErrorItem> errors = report.Read();
    QCOMPARE(6, errors.size());

    ErrorItem item = errors[0];
    QCOMPARE(item.file, QString("test.cxx"));
    QCOMPARE(item.lines[0], (unsigned int)11);
    QCOMPARE(item.id, QString("unreadVariable"));
    QCOMPARE(item.severity, QString("Style"));
    QCOMPARE(item.summary, QString("Variable 'a' is assigned a value that is never used"));
    QCOMPARE(item.message, QString("Variable 'a' is assigned a value that is never used"));
}
