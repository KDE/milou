/*
 * This file is part of the KDE Milou Project
 * Copyright (C) 2013  Vishesh Handa <me@vhanda.in>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef MILOU_REVERSEMODEL_H
#define MILOU_REVERSEMODEL_H

#include "milou_export.h"
#include <QIdentityProxyModel>

namespace Milou {

class MILOU_EXPORT ReverseModel : public QAbstractProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* sourceModel READ sourceModel WRITE setSourceModel)
    Q_PROPERTY(bool reversed READ reversed WRITE setReversed)

public:
    explicit ReverseModel(QObject* parent = 0);

    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& child) const;

    //virtual QVariant data(const QModelIndex& proxyIndex, int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;

    virtual QModelIndex mapFromSource(const QModelIndex& sourceIndex) const;
    virtual QModelIndex mapToSource(const QModelIndex& proxyIndex) const;

    virtual void setSourceModel(QAbstractItemModel* sourceModel);

    bool reversed() const;
    void setReversed(bool isReversed);

public slots:
    void run(int index);

private slots:
    void onSourceRowsInserted(const QModelIndex &parent, int start, int end);
    void onSourceRowsRemoved(const QModelIndex &parent, int start, int end);
    void onSourceLayoutChanged();

    void onSourceModelAboutToBeReset();
    void onSourceModelReset();

private:
    bool m_reversed;

    // Converts x to the actual coordinates depending on if reversed
    int con(int x) const;
    int con(int size, int x) const;
};

}

#endif // MILOU_REVERSEMODEL_H
