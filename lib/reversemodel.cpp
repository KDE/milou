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

#include "reversemodel.h"
#include "sourcesmodel.h" // For run

ReverseModel::ReverseModel(QObject* parent)
    : QAbstractProxyModel(parent)
    , m_reversed(true)
{

}


void ReverseModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    connect(sourceModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(onSourceRowsInserted(QModelIndex,int,int)));
    connect(sourceModel, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(onSourceRowsRemoved(QModelIndex,int,int)));
    connect(sourceModel, SIGNAL(layoutChanged()),
            this, SLOT(onSourceLayoutChanged()));
    connect(sourceModel, SIGNAL(modelAboutToBeReset()),
            this, SLOT(onSourceModelAboutToBeReset()));
    connect(sourceModel, SIGNAL(modelReset()),
            this, SLOT(onSourceModelReset()));

    QAbstractProxyModel::setSourceModel(sourceModel);

    QHash<int, QByteArray> roles = sourceModel->roleNames();
    setRoleNames(roles);
}

int ReverseModel::columnCount(const QModelIndex& parent) const
{
    return sourceModel()->columnCount(parent);
}

int ReverseModel::rowCount(const QModelIndex& parent) const
{
    return sourceModel()->rowCount(parent);
}

QModelIndex ReverseModel::parent(const QModelIndex& child) const
{
    return sourceModel()->parent(child);
}

QModelIndex ReverseModel::index(int row, int column, const QModelIndex& parent) const
{
    return sourceModel()->index(row, column, parent);
}

QModelIndex ReverseModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    return sourceModel()->index(con(sourceIndex.row()), sourceIndex.column(), sourceIndex.parent());
}

QModelIndex ReverseModel::mapToSource(const QModelIndex& proxyIndex) const
{
    return sourceModel()->index(con(proxyIndex.row()), proxyIndex.column(), proxyIndex.parent());
}

void ReverseModel::onSourceRowsInserted(const QModelIndex& parent, int start, int end)
{
    beginInsertRows(parent, con(start), con(end));
    endInsertRows();
}

void ReverseModel::onSourceRowsRemoved(const QModelIndex& parent, int start, int end)
{
    // The (end-start+1) is because the rowCount has already been decreased
    const int s = rowCount() + (end-start+1);
    beginRemoveRows(parent, con(s, start), con(s, end));
    endRemoveRows();
}

void ReverseModel::onSourceModelAboutToBeReset()
{
    beginResetModel();
}

void ReverseModel::onSourceModelReset()
{
    endResetModel();
}

void ReverseModel::onSourceLayoutChanged()
{
    layoutAboutToBeChanged();
    layoutChanged();
}

bool ReverseModel::reversed() const
{
    return m_reversed;
}

void ReverseModel::setReversed(bool isReversed)
{
    m_reversed = isReversed;
    beginResetModel();
    endResetModel();
}

int ReverseModel::con(int x) const
{
    return con(rowCount(), x);
}

int ReverseModel::con(int size, int x) const
{
    if (m_reversed)
        return size - (x+1);
    else
        return x;
}

void ReverseModel::run(int index)
{
    SourcesModel* m = qobject_cast<SourcesModel*>(sourceModel());
    if (m)
        m->run(con(index));
}





