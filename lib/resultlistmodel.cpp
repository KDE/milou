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

#include "resultlistmodel.h"
#include "resultsmodel.h"
#include <KDebug>

using namespace Nepomuk2;

ResultListModel::ResultListModel(QObject* parent): QAbstractProxyModel(parent)
{
    QHash<int, QByteArray> roles = roleNames();
    roles.insert(TypeRole, "type");

    setRoleNames(roles);
    setSourceModel(new Nepomuk2::ResultsModel(this));
}

void ResultListModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    // Only allow a ResultsModel to be the source
    Q_ASSERT(qobject_cast<Nepomuk2::ResultsModel*>(sourceModel));

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
}

int ResultListModel::columnCount(const QModelIndex& parent) const
{
    return 1;

    /*
    kDebug() << parent;
    if (parent.isValid())
        return 0;

    return sourceModel()->columnCount(parent);*/
}

int ResultListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    int count = 0;

    QAbstractItemModel* model = sourceModel();
    for (int i=0; i<model->rowCount(); i++) {
        count += model->rowCount(model->index(i, 0));
    }

    return count;
}

QModelIndex ResultListModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    kDebug() << sourceIndex;
    if (!sourceIndex.isValid())
        return QModelIndex();

    if (!sourceIndex.parent().isValid())
        return QModelIndex();

    QModelIndex parent = sourceIndex.parent();

    QAbstractItemModel* model = sourceModel();
    int count = 0;
    for (int i=0; i<parent.row(); i++) {
        count += model->rowCount(model->index(i, 0));
    }

    return createIndex(count + sourceIndex.row(), 0);
}


QModelIndex ResultListModel::mapToSource(const QModelIndex& proxyIndex) const
{
    if (!proxyIndex.isValid())
        return QModelIndex();

    if (proxyIndex.row() >= rowCount())
        return QModelIndex();

    QAbstractItemModel* model = sourceModel();

    int row = proxyIndex.row();
    for (int i=0; i<model->rowCount(); i++) {
        QModelIndex parentIndex = model->index(i, 0);
        int thisRowCount = model->rowCount(parentIndex);

        if (row < thisRowCount) {
            return model->index(row, 0, parentIndex);
        }

        row -= thisRowCount;
    }

    return QModelIndex();
}

QModelIndex ResultListModel::index(int row, int column, const QModelIndex& parent) const
{
    if (column != 0)
        return QModelIndex();

    if (parent.isValid())
        return QModelIndex();

    if (row <= rowCount())
        return createIndex(row, 0);

    return QModelIndex();
}


QModelIndex ResultListModel::parent(const QModelIndex&) const
{
    return QModelIndex();
}

QVariant ResultListModel::data(const QModelIndex& proxyIndex, int role) const
{
    if (!proxyIndex.isValid())
        return QVariant();

    if( role == TypeRole ) {
        QModelIndex sourceIndex = mapToSource(proxyIndex);
        if (!sourceIndex.isValid())
            return QVariant();

        return sourceIndex.parent().data();
    }

    return QAbstractProxyModel::data(proxyIndex, role);
}

void ResultListModel::onSourceRowsInserted(const QModelIndex &parent, int start, int end)
{
    if (!parent.isValid())
        return;

    //FUCK: This is going to be complex!!
    beginResetModel();
    endResetModel();
    //FIXME!!
    /*
    beginInsertRows(mapFromSource(parent), start, end);
    endInsertRows();
    */
}

void ResultListModel::onSourceRowsRemoved(const QModelIndex &parent, int start, int end)
{
    beginResetModel();
    endResetModel();
    /*
    beginRemoveRows(mapFromSource(parent), start, end);
    endRemoveRows();
    */
}

void ResultListModel::onSourceLayoutChanged()
{
    layoutAboutToBeChanged();
    layoutChanged();
}

void ResultListModel::onSourceModelAboutToBeReset()
{
    beginResetModel();
}

void ResultListModel::onSourceModelReset()
{
    endResetModel();
}

int ResultListModel::queryLimit()
{
    return qobject_cast<Nepomuk2::ResultsModel*>(sourceModel())->queryLimit();
}

void ResultListModel::setQueryLimit(int limit)
{
    qobject_cast<Nepomuk2::ResultsModel*>(sourceModel())->setQueryLimit(limit);
}

QString ResultListModel::queryString()
{
    return qobject_cast<Nepomuk2::ResultsModel*>(sourceModel())->queryString();
}

void ResultListModel::setQueryString(const QString& string)
{
    qobject_cast<Nepomuk2::ResultsModel*>(sourceModel())->setQueryString(string);
}






