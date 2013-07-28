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

#ifndef RESULTLISTMODEL_H
#define RESULTLISTMODEL_H

#include "nepomuk_finder_export.h"

#include <QAbstractProxyModel>

namespace Nepomuk2 {

/**
 * This takes a ResultsModel as a source and converts it into a list model
 * which can easily be used via QML
 */
class NEPOMUK_FINDER_EXPORT ResultListModel : public QAbstractProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString queryString READ queryString WRITE setQueryString)
    Q_PROPERTY(int queryLimit READ queryLimit WRITE setQueryLimit)

public:
    ResultListModel(QObject* parent = 0);

    enum Roles {
        TypeRole = Qt::UserRole + 100
    };

    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    virtual QModelIndex mapFromSource(const QModelIndex& sourceIndex) const;
    virtual QModelIndex mapToSource(const QModelIndex& proxyIndex) const;

    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;

    virtual QModelIndex parent(const QModelIndex& child) const;
    virtual QVariant data(const QModelIndex& proxyIndex, int role = Qt::DisplayRole) const;

    virtual void setSourceModel(QAbstractItemModel* sourceModel);

    QString queryString();
    int queryLimit();

public slots:
    void setQueryString(const QString& string);
    void setQueryLimit(int limit);

signals:
    void listingFinished(const QString& queryString);
    void listingStarted(const QString& queryString);

private slots:
    void onSourceRowsInserted(const QModelIndex &parent, int start, int end);
    void onSourceRowsRemoved(const QModelIndex &parent, int start, int end);
    void onSourceLayoutChanged();

    void onSourceModelAboutToBeReset();
    void onSourceModelReset();
};
}

#endif // RESULTLISTMODEL_H
