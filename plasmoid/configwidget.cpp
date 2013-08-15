/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2013  Vishesh Handa <me@vhanda.in>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "configwidget.h"
#include <KIcon>
#include <KConfig>
#include <KConfigGroup>

#include <QVBoxLayout>
#include <QListWidgetItem>

// FIXME: Allow the list items to be rearranged thereby giving each item
//        a different priority
ConfigWidget::ConfigWidget(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f)
{
    m_listWidget = new QListWidget(this);
    connect(m_listWidget, SIGNAL(itemChanged(QListWidgetItem*)),
            this, SIGNAL(changed()));

    KConfig config("nepomukfinderrc");
    KConfigGroup generalGroup = config.group("General");
    int numTypes = generalGroup.readEntry("NumTypes", 0);

    for (int i=0; i<numTypes; i++) {
        KConfigGroup group = config.group("Type-" + QString::number(i));

        QString name = group.readEntry("Name", QString());
        QString icon = group.readEntry("Icon", QString());
        bool enabled = group.readEntry("Enabled", true);

        QListWidgetItem* item = new QListWidgetItem(name, m_listWidget);
        item->setCheckState(enabled ? Qt::Checked : Qt::Unchecked);
        item->setIcon(KIcon(icon));
    }

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_listWidget);
}

ConfigWidget::~ConfigWidget()
{
}

void ConfigWidget::saveSettings()
{
    KConfig config("nepomukfinderrc");

    int size = m_listWidget->count();
    for(int i=0; i<size; i++) {
        QListWidgetItem* item = m_listWidget->item(i);

        KConfigGroup group = config.group("Type-" + QString::number(i));
        group.writeEntry("Enabled", (item->checkState() == Qt::Checked));
    }
}