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

#include <KApplication>
#include <KCmdLineArgs>

#include <QTimer>
#include <QDebug>
#include <KTempDir>

#include "modeltest.h"
#include "../sourcesmodel.h"

using namespace Milou;

class TestObject : public QObject {
    Q_OBJECT
public slots:
    void fire() {
        i++;
        if (i > queryString.size()) {
            timer.stop();
            QCoreApplication::instance()->exit();
            return;
        }

        const QString str = queryString.mid(0, i);
        qDebug() << "Setting" << str;
        qDebug() << "Setting" << str;
        qDebug() << "Setting" << str;
        qDebug() << "Setting" << str;
        qDebug() << "Setting" << str;
        qDebug() << "Setting" << str;
        model->setQueryString(str);
    }

public:
    TestObject(const QString& str) {
        queryString = str;
        i = 0;

        timer.setInterval(3000);
        connect(&timer, SIGNAL(timeout()), this, SLOT(fire()));
    }

    SourcesModel* model;
    QString queryString;
    int i;

    QTimer timer;
};

int main(int argc, char** argv)
{
    KCmdLineArgs::init(argc, argv, "modeltest", "modeltest", ki18n("modeltest"), "0.1");
    KApplication app;

    SourcesModel* model = new SourcesModel();
    ModelTest* mt = new ModelTest(model);
    model->setQueryLimit(20);

    TestObject obj("Summer");
    obj.model = model;
    obj.timer.start();

    return app.exec();
}

#include "test.moc"
