/*
 * This file is part of the KDE Baloo Project
 * SPDX-FileCopyrightText: 2014 Vishesh Handa <me@vhanda.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

#include <QAbstractItemModelTester>
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

#include "../sourcesmodel.h"

using namespace Milou;

class TestObject : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void fire()
    {
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
    TestObject(const QString &str)
    {
        queryString = str;
        i = 0;

        timer.setInterval(3000);
        connect(&timer, SIGNAL(timeout()), this, SLOT(fire()));
    }

    SourcesModel *model;
    QString queryString;
    int i;

    QTimer timer;
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    SourcesModel *model = new SourcesModel();
    auto mt = new QAbstractItemModelTester(model);
    Q_UNUSED(mt);
    model->setQueryLimit(20);

    TestObject obj(QStringLiteral("Summer"));
    obj.model = model;
    obj.timer.start();

    return app.exec();
}

#include "test.moc"
