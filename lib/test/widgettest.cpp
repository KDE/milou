/*
 * This file is part of the KDE Baloo Project
 * SPDX-FileCopyrightText: 2014 Vishesh Handa <me@vhanda.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

#include <QApplication>
#include <QTimer>
#include <QVBoxLayout>

#include <QListView>
#include <QWidget>
#include <qlineedit.h>

#include "../sourcesmodel.h"

using namespace Milou;

class TestObject : public QWidget
{
    Q_OBJECT
public Q_SLOTS:
    void main();

public:
    explicit TestObject(QWidget *parent = nullptr, Qt::WindowFlags f = {})
        : QWidget(parent, f)
    {
        QTimer::singleShot(0, this, SLOT(main()));
    }
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    TestObject obj;
    obj.show();

    return app.exec();
}

void TestObject::main()
{
    SourcesModel *smodel = new SourcesModel(this);
    smodel->setQueryLimit(20);

    QListView *view = new QListView(this);
    view->setModel(smodel);
    view->setAlternatingRowColors(true);

    QLineEdit *edit = new QLineEdit(this);
    connect(edit, SIGNAL(textChanged(QString)), smodel, SLOT(setQueryString(QString)));

    QVBoxLayout *l = new QVBoxLayout(this);
    l->addWidget(edit);
    l->addWidget(view);
}

#include "widgettest.moc"
