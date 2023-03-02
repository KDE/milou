/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2013 Vishesh Handa <me@vhanda.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

#ifndef SOURCESMODEL_H
#define SOURCESMODEL_H

#include <QAbstractListModel>
#include <QTimer>

#include <KRunner/QueryMatch>
#include <KRunner/RunnerManager>

#include "milou_export.h"

namespace Milou
{
class MILOU_EXPORT SourcesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString queryString READ queryString WRITE setQueryString)
    Q_PROPERTY(int queryLimit READ queryLimit WRITE setQueryLimit)
#if KRUNNER_ENABLE_DEPRECATED_SINCE(5, 81)
    Q_PROPERTY(QString runner READ runner WRITE setRunner NOTIFY runnerChanged)
    Q_PROPERTY(QString runnerName READ runnerName NOTIFY runnerChanged)
    Q_PROPERTY(QIcon runnerIcon READ runnerIcon NOTIFY runnerChanged)
#endif

public:
    explicit SourcesModel(QObject *parent = nullptr);
    ~SourcesModel() override;

    enum Roles {
        TypeRole = Qt::UserRole + 1,
        SubtextRole,
        ActionsRole,
        DuplicateRole,
        PreviewTypeRole,
        PreviewUrlRole,
        PreviewLabelRole,
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex &) const override
    {
        return false;
    }

#if KRUNNER_ENABLE_DEPRECATED_SINCE(5, 81)
    QString runner() const;
    void setRunner(const QString &runner);

    QString runnerName() const;
    QIcon runnerIcon() const;
#endif

    QString queryString() const;
    int queryLimit() const;

    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    /**
     * This signal is emitted when a an InformationalMatch is run, and it is advised
     * to update the search term.
     * Eg - Calculator runner
     */
    void updateSearchTerm(const QString &text, int pos);

    void runnerChanged();

public Q_SLOTS:
    void reloadConfiguration();
    void setQueryString(const QString &str);
    void setQueryLimit(int limit);
    /**
     * Clears the model content and resets the runner context, i.e. no new items will appear.
     */
    void clear();

    bool run(int index);
    bool runAction(int index, int actionIndex);

    Q_INVOKABLE QString getType(int index) const
    {
        return data(createIndex(index, 0), TypeRole).toString();
    }

    Q_INVOKABLE QMimeData *getMimeData(int index) const;

private Q_SLOTS:
    void slotMatchesChanged(const QList<KRunner::QueryMatch> &list);
    void slotMatchAdded(const KRunner::QueryMatch &match);
    void slotResetTimeout();
    void slotSettingsFileChanged(const QString &path);

public:
    // A list of all the types that are being shown
    QList<QString> m_types;

    struct TypeData {
        QList<KRunner::QueryMatch> shown;
        QList<KRunner::QueryMatch> hidden;
    };
    QHash<QString, TypeData> m_matches;
    int m_size;

    /// Counts the number of results for each visible KRunner::QueryMatch::text
    /// We use this to show additional info when there are multiple visible
    /// results with the same text
    QHash<QString, int> m_duplicates;

    QString m_queryString;
    int m_queryLimit;
    QString m_runner;

    KRunner::RunnerManager *m_manager;
    bool m_modelPopulated;
    QTimer m_resetTimer;

    KRunner::QueryMatch fetchMatch(int row) const;
};

}

#endif // SOURCESMODEL_H
