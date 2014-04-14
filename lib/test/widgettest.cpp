/*
 * This file is part of the KDE Baloo Project
 * Copyright (C) 2014  Vishesh Handa <me@vhanda.in>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QTimer>
#include <QApplication>

#include <QWidget>
#include <QListView>
#include <QHBoxLayout>
#include <qlineedit.h>

#include "../sourcesmodel.h"

using namespace Milou;

class TestObject : public QWidget {
    Q_OBJECT
public slots:
    void main();

public:
    explicit TestObject(QWidget* parent = 0, Qt::WindowFlags f = 0) {
        QTimer::singleShot(0, this, SLOT(main()));
    }
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    TestObject obj;
    obj.show();

    return app.exec();
}

void TestObject::main()
{
    SourcesModel* smodel = new SourcesModel(this);
    smodel->setQueryLimit(20);

    QListView* view = new QListView(this);
    view->setModel(smodel);
    view->setAlternatingRowColors(true);

    QLineEdit* edit = new QLineEdit(this);
    connect(edit, SIGNAL(textChanged(QString)),
            smodel, SLOT(setQueryString(QString)));

    QVBoxLayout* l = new QVBoxLayout(this);
    l->addWidget(edit);
    l->addWidget(view);
}

#include "widgettest.moc"
