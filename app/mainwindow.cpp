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

#include "mainwindow.h"
#include "resultsmodel.h"
#include "sidebar.h"
#include <KIcon>

#include <QVBoxLayout>


MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    setWindowIcon(KIcon("nepomuk"));

    m_lineEdit = new QLineEdit(this);
    connect(m_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged(QString)));

    m_model = new Nepomuk2::ResultsModel(this);
    m_sortProxyModel = new QSortFilterProxyModel(this);
    m_sortProxyModel->setSourceModel(m_model);
    m_sortProxyModel->setDynamicSortFilter(true);

    m_view = new QListView(this);
    m_view->setModel(m_sortProxyModel);
    m_view->setWordWrap(true);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(m_lineEdit);
    layout->addWidget(m_view);

    m_sidebar = new Sidebar(this);
    connect(m_sidebar, SIGNAL(sortOrderChanged()), this, SLOT(slotSortOrderChanged()));
    connect(m_sidebar, SIGNAL(dateFilterChanged()), this, SLOT(slotDateFilterChanged()));

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addItem(layout);
    hLayout->addWidget(m_sidebar);

    QWidget* mainWidget = new QWidget();
    mainWidget->setLayout(hLayout);
    setCentralWidget(mainWidget);
}

MainWindow::~MainWindow()
{
}

void MainWindow::slotTextChanged(const QString& text)
{
    m_model->setQueryString(text);
}

void MainWindow::slotSortOrderChanged()
{
    Sidebar::SortOrder order = m_sidebar->sortOrder();

    if (order == Sidebar::SortModified) {
        m_sortProxyModel->setSortRole(Nepomuk2::ResultsModel::ModifiedRole);
    }
    else if (order == Sidebar::SortCreated) {
        m_sortProxyModel->setSortRole(Nepomuk2::ResultsModel::CreatedRole);
    }
    else {
        // FIXME: This sorts by name. Not relevance!!
        m_sortProxyModel->setSortRole(Qt::DisplayRole);
    }

    m_sortProxyModel->sort(0);
}

void MainWindow::slotDateFilterChanged()
{

}

