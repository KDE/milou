/*
 * SPDX-FileCopyrightText: 2019 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <KConfigWatcher>
#include <KRunner/ResultsModel>

#include <qqmlregistration.h>

#include <QMimeData>

namespace Milou
{
class ResultsModel : public KRunner::ResultsModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QStringList favoriteIds READ favoriteIds WRITE setFavoriteIds NOTIFY favoriteIdsChanged RESET resetFavoriteIds)
public:
    explicit ResultsModel(QObject *parent = nullptr);

    void setFavoriteIds(const QStringList &ids)
    {
        m_favoritesExplicitlySet = true;
        KRunner::ResultsModel::setFavoriteIds(ids);
    }
    void resetFavoriteIds()
    {
        m_favoritesExplicitlySet = false;
        KRunner::ResultsModel::setFavoriteIds(m_configFavoriteIds);
    }

    // Workaround for https://bugs.kde.org/show_bug.cgi?id=511228
    Q_INVOKABLE QMimeData *getMimeData(const QModelIndex &idx) const
    {
        return KRunner::ResultsModel::getMimeData(idx);
    }

private:
    KConfigWatcher::Ptr m_configWatcher;
    QStringList m_configFavoriteIds;
    bool m_favoritesExplicitlySet = false;
};

} // namespace Milou
