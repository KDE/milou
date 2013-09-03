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
#include <KLocalizedString>

#include <QLabel>
#include <QVBoxLayout>
#include <QListWidgetItem>

ConfigWidget::ConfigWidget(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f)
{
    m_listWidget = new QListWidget(this);
    m_listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listWidget->setDragDropMode(QAbstractItemView::InternalMove);
    m_listWidget->setAlternatingRowColors(true);

    connect(m_listWidget, SIGNAL(itemChanged(QListWidgetItem*)),
            this, SIGNAL(changed()));
    connect(m_listWidget->model(), SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)),
            this, SIGNAL(changed()));

    KConfig config("milourc");
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

    QLabel* label = new QLabel(i18n("Only the selected components are shown in the search results"));
    label->setAlignment(Qt::AlignHCenter);

    QLabel* label2 = new QLabel(i18n("Drag categories to change the order in which results appear"));
    QFont fo = label2->font();
    fo.setBold(true);
    label2->setFont(fo);
    label2->setAlignment(Qt::AlignHCenter);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(label2);
    layout->addWidget(m_listWidget);
}

ConfigWidget::~ConfigWidget()
{
}

void ConfigWidget::saveSettings()
{
    KConfig config("milourc");

    int size = m_listWidget->count();
    for(int i=0; i<size; i++) {
        QListWidgetItem* item = m_listWidget->item(i);

        KConfigGroup group = config.group("Type-" + QString::number(i));
        group.writeEntry("Name", item->text());
        group.writeEntry("Icon", item->icon().name());
        group.writeEntry("Enabled", (item->checkState() == Qt::Checked));
    }
}
