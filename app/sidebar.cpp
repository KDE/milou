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

#include "sidebar.h"

#include <KLocalizedString>

#include <QGroupBox>
#include <QVBoxLayout>

Sidebar::Sidebar(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f)
{
    QGroupBox* m_sortGroup = new QGroupBox(i18n("Sort By"));
    QGroupBox* m_dateGroup = new QGroupBox(i18n("Filter By"));

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->addWidget(m_sortGroup);
    vLayout->addWidget(m_dateGroup);

    m_sortRelevanceButton = new QRadioButton(i18n("Relevance"), m_sortGroup);
    m_sortModifiedButton = new QRadioButton(i18n("Modified"), m_sortGroup);
    m_sortCreatedButton = new QRadioButton(i18n("Created"), m_sortGroup);

    connect(m_sortRelevanceButton, SIGNAL(toggled(bool)), this, SIGNAL(sortOrderChanged()));
    connect(m_sortModifiedButton, SIGNAL(toggled(bool)), this, SIGNAL(sortOrderChanged()));
    connect(m_sortCreatedButton, SIGNAL(toggled(bool)), this, SIGNAL(sortOrderChanged()));

    QVBoxLayout* sortLayout = new QVBoxLayout(m_sortGroup);
    sortLayout->addWidget(m_sortRelevanceButton);
    sortLayout->addWidget(m_sortModifiedButton);
    sortLayout->addWidget(m_sortCreatedButton);

    m_dateAnythingButton = new QRadioButton(i18n("Any time"), m_dateGroup);
    m_dateLast7DaysButton = new QRadioButton(i18n("Last 7 days"), m_dateGroup);
    m_dateLast30DaysButton = new QRadioButton(i18n("Last 30 days"), m_dateGroup);
    m_dateLastYearButton = new QRadioButton(i18n("Last year"), m_dateGroup);

    connect(m_dateAnythingButton, SIGNAL(toggled(bool)), this, SIGNAL(dateFilterChanged()));
    connect(m_dateLast7DaysButton, SIGNAL(toggled(bool)), this, SIGNAL(dateFilterChanged()));
    connect(m_dateLast30DaysButton, SIGNAL(toggled(bool)), this, SIGNAL(dateFilterChanged()));
    connect(m_dateLastYearButton, SIGNAL(toggled(bool)), this, SIGNAL(dateFilterChanged()));

    QVBoxLayout* dateLayout = new QVBoxLayout(m_dateGroup);
    dateLayout->addWidget(m_dateAnythingButton);
    dateLayout->addWidget(m_dateLast7DaysButton);
    dateLayout->addWidget(m_dateLast30DaysButton);
    dateLayout->addWidget(m_dateLastYearButton);
}

Sidebar::SortOrder Sidebar::sortOrder()
{
    if (m_sortRelevanceButton->isChecked()) {
        return SortRelevance;
    }
    else if (m_sortModifiedButton->isChecked()) {
        return SortModified;
    }
    else if (m_sortCreatedButton->isChecked()) {
        return SortCreated;
    }
    else {
        return SortRelevance;
    }
}

Sidebar::FilterDate Sidebar::filterDate()
{
    if (m_dateAnythingButton->isChecked()) {
        return FilterDateAnything;
    }
    else if(m_dateLast7DaysButton->isChecked()) {
        return FilterDateWeek;
    }
    else if(m_dateLast30DaysButton->isChecked()) {
        return FilterDateMonth;
    }
    else if(m_dateLastYearButton->isChecked()) {
        return FilterDateYear;
    }
    else {
        return FilterDateAnything;
    }
}
