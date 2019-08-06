/*
 * This file is part of the KDE Milou Project
 * Copyright (C) 2019 Kai Uwe Broulik <kde@broulik.de>
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

#pragma once

#include <QAbstractItemModel>
#include <QHash>
#include <QTimer>
#include <QString>

#include <KRunner/QueryMatch>

namespace Plasma {
class RunnerManager;
}

namespace Milou {

class RunnerResultsModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit RunnerResultsModel(QObject *parent = nullptr);
    ~RunnerResultsModel() override;

    QString queryString() const;
    void setQueryString(const QString &queryString);
    Q_SIGNAL void queryStringChanged(const QString &queryString);

    bool querying() const;
    Q_SIGNAL void queryingChanged();

    QString runner() const;
    void setRunner(const QString &runner);
    Q_SIGNAL void runnerChanged();

    QString runnerName() const;
    QIcon runnerIcon() const;

    /**
     * Clears the model content and resets the runner context, i.e. no new items will appear.
     */
    void clear();

    bool run(const QModelIndex &idx);
    bool runAction(const QModelIndex &idx, int actionNumber);

    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    QMimeData *mimeData(const QModelIndexList &indexes) const override;

Q_SIGNALS:
    void queryStringChangeRequested(const QString &queryString, int pos);

private:
    void setQuerying(bool querying);

    Plasma::QueryMatch fetchMatch(const QModelIndex &idx) const;

    void onMatchesChanged(const QList<Plasma::QueryMatch> &matches);

    Plasma::RunnerManager *m_manager;

    QString m_queryString;
    bool m_querying = false;

    QString m_runner;

    QTimer m_resetTimer;
    bool m_hasMatches = false;

    QStringList m_categories;
    QHash<QString /*category*/, QVector<Plasma::QueryMatch>> m_matches;

};

} // namespace Milou
