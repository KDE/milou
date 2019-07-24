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

#include <QIcon>
#include <QSortFilterProxyModel>
#include <QScopedPointer>

#include "milou_export.h"

namespace Milou {

class MILOU_EXPORT ResultsModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(QString queryString READ queryString WRITE setQueryString NOTIFY queryStringChanged)
    Q_PROPERTY(int limit READ limit WRITE setLimit RESET resetLimit NOTIFY limitChanged)
    Q_PROPERTY(bool querying READ querying NOTIFY queryingChanged)

    Q_PROPERTY(QString runner READ runner WRITE setRunner NOTIFY runnerChanged)
    // FIXME rename to singleModeRunnerName or something
    Q_PROPERTY(QString runnerName READ runnerName NOTIFY runnerChanged)
    Q_PROPERTY(QIcon runnerIcon READ runnerIcon NOTIFY runnerChanged)

public:
    explicit ResultsModel(QObject *parent = nullptr);
    ~ResultsModel() override;

    enum Roles {
        IdRole = Qt::UserRole + 1,
        TypeRole,
        RelevanceRole,
        EnabledRole,
        CategoryRole,
        SubtextRole,
        DuplicateRole,
        ActionsRole
    };
    Q_ENUM(Roles)

    QString queryString() const;
    void setQueryString(const QString &queryString);
    Q_SIGNAL void queryStringChanged();

    int limit() const;
    void setLimit(int limit);
    void resetLimit();
    Q_SIGNAL void limitChanged();

    bool querying() const;
    Q_SIGNAL void queryingChanged();

    QString runner() const;
    void setRunner(const QString &runner);
    Q_SIGNAL void runnerChanged();

    QString runnerName() const;
    QIcon runnerIcon() const;

    QHash<int, QByteArray> roleNames() const override;

    /**
     * Clears the model content and resets the runner context, i.e. no new items will appear.
     */
    Q_INVOKABLE void clear();

    Q_INVOKABLE bool run(const QModelIndex &idx);
    Q_INVOKABLE bool runAction(const QModelIndex &idx, int actionNumber);

    Q_INVOKABLE QMimeData *getMimeData(const QModelIndex &idx) const;

Q_SIGNALS:
    /**
     * This signal is emitted when a an InformationalMatch is run, and it is advised
     * to update the search term.
     * Eg - Calculator runner
     */
    void queryStringChangeRequested(const QString &queryString, int pos);

private:
    class Private;
    QScopedPointer<Private> d;

};

} // namespace Milou
