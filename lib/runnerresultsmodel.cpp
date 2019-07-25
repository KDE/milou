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

#include "runnerresultsmodel.h"

#include <QAction>
#include <QSet>

#include <KRunner/RunnerManager>

#include "resultsmodel.h"

using namespace Milou;
using namespace Plasma;

RunnerResultsModel::RunnerResultsModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_manager(new RunnerManager(this))
{
    connect(m_manager, &RunnerManager::matchesChanged, this, &RunnerResultsModel::onMatchesChanged);
    connect(m_manager, &RunnerManager::queryFinished, this, [this] {
        setQuerying(false);
    });

    m_resetTimer.setSingleShot(true);
    m_resetTimer.setInterval(500);
    connect(&m_resetTimer, &QTimer::timeout, this, [this] {
        // Clear the old matches if any
        if (!m_hasMatches) {
            clear();
        }
    });
}

RunnerResultsModel::~RunnerResultsModel() = default;

Plasma::QueryMatch RunnerResultsModel::fetchMatch(const QModelIndex &idx) const
{
    const QString category = m_categories.value(idx.internalId() - 1);
    return m_matches.value(category).value(idx.row());
}

void RunnerResultsModel::onMatchesChanged(const QList<Plasma::QueryMatch> &matches)
{
    // We clear the model ourselves in the reset timer, ignore any empty matchset
    if (matches.isEmpty() && m_resetTimer.isActive() && !m_hasMatches) {
        return;
    }

    // Build the list of new categories and matches
    QSet<QString> newCategories;
    // here we use QString as key since at this point we don't care about the order
    // of categories but just what matches we have for each one.
    // Below when we populate the actual m_matches we'll make sure to keep the order
    // of existing categories to avoid pointless model changes.
    QHash<QString /*category*/, QVector<Plasma::QueryMatch>> newMatches;
    for (const auto &match : matches) {
        const QString category = match.matchCategory();
        newCategories.insert(category);
        newMatches[category].append(match);
    }

    // Get rid of all categories that are no longer present
    auto it = m_categories.begin();
    while (it != m_categories.end()) {
        const int categoryNumber = std::distance(m_categories.begin(), it);

        if (!newCategories.contains(*it)) {
            beginRemoveRows(QModelIndex(), categoryNumber, categoryNumber);
            m_matches.remove(*it);
            it = m_categories.erase(it);
            endRemoveRows();
        } else {
            ++it;
        }
    }

    // Update the existing categories by adding/removing new/removed rows and
    // updating changed ones
    for (auto it = m_categories.constBegin(), end = m_categories.constEnd(); it != end; ++it) {
        Q_ASSERT(newCategories.contains(*it));

        const int categoryNumber = std::distance(m_categories.constBegin(), it);
        const QModelIndex categoryIdx = index(categoryNumber, 0);

        // don't use operator[] as to not insert an empty list
        // TODO why? shouldn't m_categories and m_matches be in sync?
        auto oldCategoryIt = m_matches.find(*it);
        Q_ASSERT(oldCategoryIt != m_matches.end());

        auto &oldMatchesInCategory =  *oldCategoryIt;
        const auto newMatchesInCategory = newMatches.value(*it);

        Q_ASSERT(!oldMatchesInCategory.isEmpty());
        Q_ASSERT(!newMatches.isEmpty());

        // Emit a change for all existing matches if any of them changed
        // TODO only emit a change for the ones that changed
        bool signalDataChanged = false;

        const int oldCount = oldMatchesInCategory.count();
        const int newCount = newMatchesInCategory.count();

        const int endOfUpdateableRange = qMin(oldCount, newCount) - 1;

        for (int i = 0; i <= endOfUpdateableRange; ++i) {
            if (oldMatchesInCategory.at(i) != newMatchesInCategory.at(i)) {
                signalDataChanged = true;
                break;
            }
        }

        // Signal insertions for any new items
        if (newCount > oldCount) {
            beginInsertRows(categoryIdx, oldCount, newCount - 1);
            oldMatchesInCategory = newMatchesInCategory;
            endInsertRows();
        } else if (newCount < oldCount) {
            beginRemoveRows(categoryIdx, newCount, oldCount - 1);
            oldMatchesInCategory = newMatchesInCategory;
            endRemoveRows();
        } else {
            // Important to still update the matches, even if the count hasn't changed :)
            oldMatchesInCategory = newMatchesInCategory;
        }

        // Now that the source data has been updated, emit the data changes we noted down earlier
        if (signalDataChanged) {
            emit dataChanged(index(0, 0, categoryIdx), index(endOfUpdateableRange, 0, categoryIdx));
        }

        // Remove it from the "new" categories so in the next step we can add all genuinely new categories in one go
        newCategories.remove(*it);
    }

    // Finally add all the new categories
    if (!newCategories.isEmpty()) {
        beginInsertRows(QModelIndex(), m_categories.count(), m_categories.count() + newCategories.count() - 1);

        for (const QString &newCategory : newCategories) {
            const auto matchesInNewCategory = newMatches.value(newCategory);

            m_matches[newCategory] = matchesInNewCategory;
            m_categories.append(newCategory);
        }

        endInsertRows();
    }

    Q_ASSERT(m_categories.count() == m_matches.count());

    m_hasMatches = !m_matches.isEmpty();
}

QString RunnerResultsModel::queryString() const
{
    return m_queryString;
}

void RunnerResultsModel::setQueryString(const QString &queryString)
{
    if (m_queryString.trimmed() == queryString.trimmed()) {
        return;
    }

    m_queryString = queryString;
    m_hasMatches = false;
    if (queryString.isEmpty()) {
        clear();
    } else {
        m_resetTimer.start();
        m_manager->launchQuery(queryString);
        setQuerying(true);
    }
    emit queryStringChanged();
}

bool RunnerResultsModel::querying() const
{
    return m_querying;
}

void RunnerResultsModel::setQuerying(bool querying)
{
    if (m_querying != querying) {
        m_querying = querying;
        emit queryingChanged();
    }
}

QString RunnerResultsModel::runner() const
{
    return m_runner;
}

void RunnerResultsModel::setRunner(const QString &runner)
{
    if (m_runner == runner) {
        return;
    }

    m_runner = runner;
    m_manager->setSingleModeRunnerId(runner);
    m_manager->setSingleMode(!runner.isEmpty());
    emit runnerChanged();
}

QString RunnerResultsModel::runnerName() const
{
    if (auto *singleRunner = m_manager->singleModeRunner()) {
        return singleRunner->name();
    }
    return QString();
}

QIcon RunnerResultsModel::runnerIcon() const
{
    if (auto *singleRunner = m_manager->singleModeRunner()) {
        return singleRunner->icon();
    }
    return QIcon();
}

void RunnerResultsModel::clear()
{
    setQuerying(false);

    beginResetModel();
    m_hasMatches = false;
    m_categories.clear();
    m_matches.clear();
    m_manager->reset();
    m_manager->matchSessionComplete();
    endResetModel();
}

bool RunnerResultsModel::run(const QModelIndex &idx)
{
    Plasma::QueryMatch match = fetchMatch(idx);
    if (!match.isValid()) {
        return false;
    }

    if (match.type() == Plasma::QueryMatch::InformationalMatch) {
        QString info = match.data().toString();
        int editPos = info.length();

        if (!info.isEmpty()) {
            // FIXME: pretty lame way to decide if this is a query prototype
            // Copied from kde4 krunner interface.cpp
            if (!match.runner()) {
                // lame way of checking to see if this is a Help Button generated match!
                int index = info.indexOf(QStringLiteral(":q:"));

                if (index != -1) {
                    editPos = index;
                    info.replace(QStringLiteral(":q:"), QString());
                }
            }

            emit queryStringChangeRequested(info, editPos);
            return false;
        }
    }

    m_manager->run(match);
    return true;
}

bool RunnerResultsModel::runAction(const QModelIndex &idx, int actionNumber)
{
    Plasma::QueryMatch match = fetchMatch(idx);
    if (!match.isValid()) {
        return false;
    }

    const auto actions = m_manager->actionsForMatch(match);
    if (actionNumber < 0 || actionNumber >= actions.count()) {
        return false;
    }

    QAction *action = actions.at(actionNumber);
    match.setSelectedAction(action);
    m_manager->run(match);
    return true;
}

int RunnerResultsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

int RunnerResultsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0) {
        return 0;
    }

    if (!parent.isValid()) { // root level
        return m_categories.count();
    }

    if (parent.internalId()) {
        return 0;
    }

    const QString category = m_categories.value(parent.row());
    return m_matches.value(category).count();
}

QVariant RunnerResultsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.internalId()) { // runner match
        if (int(index.internalId() - 1) >= m_categories.count()) {
            return QVariant();
        }

        Plasma::QueryMatch match = fetchMatch(index);
        if (!match.isValid()) {
            return QVariant();
        }

        switch (role) {
        case Qt::DisplayRole:
            return match.text();
        case Qt::DecorationRole:
            if (!match.iconName().isEmpty()) {
                return match.iconName();
            }
            return match.icon();
        case ResultsModel::TypeRole:
            return match.type();
        case ResultsModel::RelevanceRole:
            return match.relevance();
        case ResultsModel::IdRole:
            return match.id();
        case ResultsModel::EnabledRole:
            return match.isEnabled();
        case ResultsModel::CategoryRole:
            return match.matchCategory();
        case ResultsModel::SubtextRole:
            return match.subtext();
        case ResultsModel::ActionsRole: {
            const auto actions = m_manager->actionsForMatch(match);
            if (actions.isEmpty()) {
                return QVariantList();
            }

            QVariantList actionsList;
            actionsList.reserve(actions.size());

            for (QAction *action : actions) {
                actionsList.append(QVariant::fromValue(action));
            }

            return actionsList;
        }

        }

        return QVariant();
    }

    // category
    if (index.row() >= m_categories.count()) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole:
        return m_categories.at(index.row());

    // Returns the highest type/role within the group
    case ResultsModel::TypeRole: {
        int highestType = 0;
        for (int i = 0; i < rowCount(index); ++i) {
            const int type = this->index(i, 0, index).data(ResultsModel::TypeRole).toInt();
            if (type > highestType) {
                highestType = type;
            }
        }
        return highestType;
    }
    case ResultsModel::RelevanceRole: {
        qreal highestRelevance = 0.0;
        for (int i = 0; i < rowCount(index); ++i) {
            const qreal relevance = this->index(i, 0, index).data(ResultsModel::RelevanceRole).toReal();
            if (relevance > highestRelevance) {
                highestRelevance = relevance;
            }
        }
        return highestRelevance;
    }

    }

    return QVariant();
}

QModelIndex RunnerResultsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column != 0) {
        return QModelIndex();
    }

    if (parent.isValid()) {
        const QString category = m_categories.value(parent.row());
        const auto matches = m_matches.value(category);
        if (row < matches.count()) {
            return createIndex(row, column, parent.row() + 1);
        }

        return QModelIndex();
    }

    if (row < m_categories.count()) {
        return createIndex(row, column, nullptr);
    }

    return QModelIndex();
}

QModelIndex RunnerResultsModel::parent(const QModelIndex &child) const
{
    if (child.internalId()) {
        return createIndex(child.internalId() - 1, 0, nullptr);
    }

    return QModelIndex();
}

QMimeData *RunnerResultsModel::mimeData(const QModelIndexList &indexes) const
{
    if (indexes.isEmpty()) {
        return nullptr;
    }

    Plasma::QueryMatch match = fetchMatch(indexes.first());
    if (!match.isValid()) {
        return nullptr;
    }

    return m_manager->mimeDataForMatch(match);
    return nullptr;
}