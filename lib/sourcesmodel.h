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

#include <QTimer>
#include <QAbstractListModel>

#include <KRunner/QueryMatch>
#include <KRunner/RunnerManager>

#include "milou_export.h"

namespace Milou {

class MILOU_EXPORT SourcesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString queryString READ queryString WRITE setQueryString)
    Q_PROPERTY(int queryLimit READ queryLimit WRITE setQueryLimit)
    Q_PROPERTY(QString runner READ runner WRITE setRunner NOTIFY runnerChanged)

    Q_PROPERTY(QString runnerName READ runnerName NOTIFY runnerChanged)
    Q_PROPERTY(QIcon runnerIcon READ runnerIcon NOTIFY runnerChanged)

public:
    explicit SourcesModel(QObject* parent = 0);
    virtual ~SourcesModel();

    enum Roles {
        TypeRole = Qt::UserRole + 1,
        SubtextRole,
        ActionsRole,
        DuplicateRole,
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

    QString runnerName() const;
    QIcon runnerIcon() const;

    QString queryString() const;
    int queryLimit() const;

    virtual QHash<int, QByteArray> roleNames() const;

signals:
    /**
     * This signal is emitted when a an InformationalMatch is run, and it is advised
     * to update the search term.
     * Eg - Calculator runner
     */
    void updateSearchTerm(const QString& text, int pos);

    void runnerChanged();

public slots:
    void reloadConfiguration();
    void setQueryString(const QString& str);
    void setQueryLimit(int limit);
    void clear();

    bool run(int index);
    bool runAction(int index, int actionIndex);

    Q_INVOKABLE QString getType(int index) const {
        return data(createIndex(index, 0), TypeRole).toString();
    }

    Q_INVOKABLE QMimeData *getMimeData(int index) const;

private slots:
    void slotMatchesChanged(const QList<Plasma::QueryMatch>& list);
    void slotMatchAdded(const Plasma::QueryMatch& match);
    void slotResetTimeout();

public:
    // A list of all the types that are being shown
    QList<QString> m_types;

    struct TypeData {
        QList<Plasma::QueryMatch> shown;
        QList<Plasma::QueryMatch> hidden;
    };
    QHash<QString, TypeData> m_matches;
    int m_size;

    /// Counts the number of results for each visible Plasma::QueryMatch::text
    /// We use this to show additional info when there are multiple visible
    /// results with the same text
    QHash<QString, int> m_duplicates;

    QString m_queryString;
    int m_queryLimit;
    QString m_runner;

    Plasma::RunnerManager* m_manager;
    bool m_modelPopulated;
    QTimer m_resetTimer;

    Plasma::QueryMatch fetchMatch(int row) const;
};

}

#endif // SOURCESMODEL_H
