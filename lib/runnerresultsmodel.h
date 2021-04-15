/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2019 Kai Uwe Broulik <kde@broulik.de>
 * SPDX-FileCopyrightText: 2020 Alexander Lohnau <alexander.lohnau@gmx.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

#pragma once

#include <QAbstractItemModel>
#include <QHash>
#include <QQuickItem>
#include <QString>
#include <QTimer>

#include <KRunner/QueryMatch>

namespace Plasma
{
class RunnerManager;
}

namespace Milou
{
class RunnerResultsModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit RunnerResultsModel(QObject *parent = nullptr);
    ~RunnerResultsModel() override;

    QString queryString() const;
    void setQueryString(const QString &queryString, const QString &runner);
    Q_SIGNAL void queryStringChanged(const QString &queryString);

    bool querying() const;
    Q_SIGNAL void queryingChanged();

    /**
     * Clears the model content and resets the runner context, i.e. no new items will appear.
     */
    void clear();

    bool run(const QModelIndex &idx);
    bool runAction(const QModelIndex &idx, int actionNumber, QQuickItem *visualParent);

    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    QMimeData *mimeData(const QModelIndexList &indexes) const override;

    Plasma::RunnerManager *runnerManager() const;

Q_SIGNALS:
    void queryStringChangeRequested(const QString &queryString, int pos);

private:
    void setQuerying(bool querying);

    Plasma::QueryMatch fetchMatch(const QModelIndex &idx) const;

    void onMatchesChanged(const QList<Plasma::QueryMatch> &matches);

    Plasma::RunnerManager *m_manager;

    QString m_queryString;
    bool m_querying = false;

    QString m_prevRunner;

    QTimer m_resetTimer;
    bool m_hasMatches = false;

    QStringList m_categories;
    QHash<QString /*category*/, QVector<Plasma::QueryMatch>> m_matches;
};

} // namespace Milou
