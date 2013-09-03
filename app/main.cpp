/*
   This file is part of the Nepomuk KDE project.
   Copyright (C) 2013 Vishesh Handa <handa.vish@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) version 3, or any
   later version accepted by the membership of KDE e.V. (or its
   successor approved by the membership of KDE e.V.), which shall
   act as a proxy defined in Section 6 of version 3 of the license.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <KComponentData>
#include <KUniqueApplication>
#include <KDebug>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    KAboutData aboutData("milou", 0, ki18n("Milou"),
                         "0.1",
                         ki18n("Milou is an application for finding stuff"),
                         KAboutData::License_LGPL_V2,
                         ki18n("(C) 2013, Vishesh Handa"));
    aboutData.addAuthor(ki18n("Vishesh Handa"), ki18n("Maintainer"), "me@vhanda.in");

    KCmdLineArgs::init(argc, argv, &aboutData);

    if (!KUniqueApplication::start()) {
        kError() << "Already running";
        return 0;
    }

    KUniqueApplication app;

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
