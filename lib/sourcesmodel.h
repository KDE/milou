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

#ifndef SOURCESMODEL_H
#define SOURCESMODEL_H

#include <QAbstractItemModel>
#include <QVector>

#include <KRunner/RunnerManager>
#include <KRunner/QueryMatch>

#include "milou_export.h"

namespace Milou {

class MILOU_EXPORT SourcesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString queryString READ queryString WRITE setQueryString)
    Q_PROPERTY(int queryLimit READ queryLimit WRITE setQueryLimit)
    Q_PROPERTY(QString runner READ runner WRITE setRunner)

public:
    explicit SourcesModel(QObject* parent = 0);
    virtual ~SourcesModel();

    enum Roles {
        TypeRole = Qt::UserRole + 1,
        PreviewTypeRole,
        PreviewUrlRole,
        PreviewLabelRole
    };

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual bool hasChildren(const QModelIndex&) const {
        return false;
    }

    QString runner() const;
    void setRunner(const QString& runner);

    QString queryString() const;
    int queryLimit() const;

    virtual QHash<int, QByteArray> roleNames() const;

public slots:
    void reloadConfiguration();
    void setQueryString(const QString& str);
    void setQueryLimit(int limit);
    void clear();

    void run(int index);

    Q_INVOKABLE QString getType(int index) const {
        return data(createIndex(index, 0), TypeRole).toString();
    }


private slots:
    void slotMatchesChanged(const QList<Plasma::QueryMatch>& list);
    void slotMatchAdded(const Plasma::QueryMatch& match);
    void stopSuppressingSignals();

public:
    // A list of all the types that are being shown
    QList<QString> m_types;

    // Each type has a priority based on the results, on how high it should
    // be on the list
    QHash<QString, int> m_typePriority;

    struct TypeData {
        QList<Plasma::QueryMatch> shown;
        QList<Plasma::QueryMatch> hidden;
    };
    QHash<QString, TypeData> m_matches;
    int m_size;

    QString m_queryString;
    int m_queryLimit;
    QString m_runner;

    bool m_supressSignals;
    Plasma::RunnerManager* m_manager;

    /// Returns the number of visible rows before \p type
    int fetchRowCount(const QString& type) const;
    Plasma::QueryMatch fetchMatch(int row) const;
};

}

#endif // SOURCESMODEL_H
