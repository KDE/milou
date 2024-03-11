/*
 * SPDX-FileCopyrightText: 2019 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "resultsmodel.h"

using namespace Milou;

using namespace Qt::StringLiterals;

ResultsModel::ResultsModel(QObject *parent)
    : KRunner::ResultsModel(parent)
{
    const auto config = KSharedConfig::openConfig(u"krunnerrc"_s);
    m_configWatcher = KConfigWatcher::create(config);
    const auto assignFavoriteIds = [this, config]() {
        const KConfigGroup grp = config->group(u"General"_s).parent().group(u"Plugins"_s).group(u"Favorites"_s);
        m_configFavoriteIds = grp.readEntry("plugins", QStringList(u"krunner_services"_s));
        if (!m_favoritesExplicitlySet) {
            KRunner::ResultsModel::setFavoriteIds(m_configFavoriteIds);
        }
    };
    connect(m_configWatcher.data(), &KConfigWatcher::configChanged, this, [assignFavoriteIds](const KConfigGroup &group) {
        const QLatin1String pluginsGrp("Plugins");
        if (group.name() == QLatin1String("Favorites") && group.parent().name() == pluginsGrp) {
            assignFavoriteIds();
        }
    });
    assignFavoriteIds();
}

#include "moc_resultsmodel.cpp"
