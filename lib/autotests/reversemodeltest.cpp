/*
 * Copyright (C) 2014  Vishesh Handa <me@vhanda.in>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "reversemodeltest.h"
#include "../reversemodel.h"

#include <QTest>
#include <QStringListModel>

void ReverseModelTest::testReverseModel()
{
    QStringList list;
    list << "One" << "Two" << "Three";

    QStringListModel model(list);
    Milou::ReverseModel rvm;
    rvm.setReversed(true);
    rvm.setSourceModel(&model);

    QCOMPARE(rvm.rowCount(), 3);
    QCOMPARE(rvm.columnCount(), 1);
    QCOMPARE(rvm.data(rvm.index(0, 0)).toString(), QString("Three"));
    QCOMPARE(rvm.data(rvm.index(1, 0)).toString(), QString("Two"));
    QCOMPARE(rvm.data(rvm.index(2, 0)).toString(), QString("One"));
}

QTEST_MAIN(ReverseModelTest)
