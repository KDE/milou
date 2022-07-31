/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2019 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

#include "resultsmodel.h"

#include "runnerresultsmodel.h"

#include <QIdentityProxyModel>

#include <KDescendantsProxyModel>
#include <KModelIndexProxyMapper>

#include <KRunner/AbstractRunner>
#include <cmath>

using namespace Milou;

/**
 * Sorts the matches and categories by their type and relevance
 *
 * A category gets type and relevance of the highest
 * scoring match within.
 */
class SortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    SortProxyModel(QObject *parent)
        : QSortFilterProxyModel(parent)
    {
        setDynamicSortFilter(true);
        sort(0, Qt::DescendingOrder);
    }
    ~SortProxyModel() override = default;

    void setQueryString(const QString &queryString)
    {
        const QStringList words = queryString.split(QLatin1Char(' '), Qt::SkipEmptyParts);
        if (m_words != words) {
            m_words = words;
            invalidate();
        }
    }

protected:
    bool lessThan(const QModelIndex &sourceA, const QModelIndex &sourceB) const override
    {
        const int typeA = sourceA.data(ResultsModel::TypeRole).toInt();
        const int typeB = sourceB.data(ResultsModel::TypeRole).toInt();

        if (typeA != typeB) {
            return typeA < typeB;
        }

        const qreal relevanceA = sourceA.data(ResultsModel::RelevanceRole).toReal();
        const qreal relevanceB = sourceB.data(ResultsModel::RelevanceRole).toReal();

        if (!qFuzzyCompare(relevanceA, relevanceB)) {
            return relevanceA < relevanceB;
        }

        return QSortFilterProxyModel::lessThan(sourceA, sourceB);
    }

private:
    QStringList m_words;
};

/**
 * Distributes the number of matches shown per category
 *
 * Each category may occupy a maximum of 1/(n+1) of the given @c limit,
 * this means the further down you get, the less matches there are.
 * There is at least one match shown per category.
 *
 * This model assumes the results to already be sorted
 * descending by their relevance/score.
 */
class CategoryDistributionProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    CategoryDistributionProxyModel(QObject *parent)
        : QSortFilterProxyModel(parent)
    {
    }
    ~CategoryDistributionProxyModel() override = default;

    void setSourceModel(QAbstractItemModel *sourceModel) override
    {
        if (this->sourceModel()) {
            disconnect(this->sourceModel(), nullptr, this, nullptr);
        }

        QSortFilterProxyModel::setSourceModel(sourceModel);

        if (sourceModel) {
            const auto invalidate = [this, sourceModel]() {
                invalidateFilter();
                // Aggregate all the matches that migh be displayed so that we can given each category
                // a reasonable amount while most effectively filling out the limit
                m_matchesCount = 0;
                m_balancedMatchesCount = 0;
                for (int i = 0; i <= sourceModel->rowCount(); ++i) {
                    const QModelIndex idx = sourceModel->index(i, 0);
                    if (idx.internalId() == 0) { // Ignore values that do not represent a category
                        continue;
                    }
                    // If a category contains more entries than the total amount, we ignore the ones that exeed the limit
                    const int categoryMatchCount = sourceModel->rowCount(idx);
                    m_matchesCount += categoryMatchCount;
                    m_balancedMatchesCount += qMin(categoryMatchCount, m_limit);
                };

                // Check how many matches we are getting when we take the weighing into account
                int totalMatchesWithWeighing = 0;
                for (int i = 0; i <= sourceModel->rowCount(); ++i) {
                    const QModelIndex idx = sourceModel->index(i, 0);
                    if (idx.internalId() == 0) { // Ignore values that do not represent a category
                        continue;
                    }
                    const int matchesInCurrentCategory = sourceModel->rowCount(idx);
                    const int balancedCategoryMatchCount = qMin(matchesInCurrentCategory, m_limit);
                    const float weighingFactor = (float)m_balancedMatchesCount / m_limit;
                    totalMatchesWithWeighing += ceil(balancedCategoryMatchCount / weighingFactor);
                };
                // When we know how many matches we exceed the limit, the categories with the most matches should display one match less
                if (totalMatchesWithWeighing > (m_limit + 1)) {
                    QVector<int> categoryCounts;
                    for (int i = 0; i <= sourceModel->rowCount(); ++i) {
                        const QModelIndex idx = sourceModel->index(i, 0);
                        if (idx.internalId() == 0) { // Ignore values that do not represent a category
                            continue;
                        }
                        const int matchesInCurrentCategory = sourceModel->rowCount(idx);
                        if (matchesInCurrentCategory > 3) {
                            categoryCounts.append(matchesInCurrentCategory);
                        }
                    };
                    std::sort(categoryCounts.begin(), categoryCounts.end(), std::greater<int>{});
                    const int maxListSize = totalMatchesWithWeighing - m_limit;
                    if (categoryCounts.size() > maxListSize) {
                        categoryCounts.resize(maxListSize);
                    }
                    const auto maxIt = std::min_element(categoryCounts.cbegin(), categoryCounts.cend());
                    m_maxMatchesBeforeShrinking = maxIt != categoryCounts.cend() ? *maxIt : m_limit;
                }
            };
            connect(sourceModel, &QAbstractItemModel::rowsInserted, this, invalidate);
            connect(sourceModel, &QAbstractItemModel::rowsMoved, this, invalidate);
            connect(sourceModel, &QAbstractItemModel::rowsRemoved, this, invalidate);
        }
    }

    int limit() const
    {
        return m_limit;
    }

    void setLimit(int limit)
    {
        if (m_limit == limit) {
            return;
        }
        m_limit = limit;
        invalidateFilter();
        Q_EMIT limitChanged();
    }

Q_SIGNALS:
    void limitChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override
    {
        if (m_limit <= 0) {
            return true;
        }

        if (!sourceParent.isValid()) {
            return true;
        }

        const int categoryCount = sourceModel()->rowCount();

        // If we only have one category, we can check the limit directly
        if (categoryCount == 1) {
            return sourceRow < m_limit;
        }

        // If all the matches are less than the limit, we do not need any balancing between the categories
        if (m_matchesCount <= m_limit) {
            return true;
        }

        /*
         * If we have multiple categories and the amount of matches exceeds the limit, we need to balance it.
         * For this we use the aggregated match counts and calculate the factors in which the categories need to shrink down to fit the limit.
         * To avoid a category getting too many results matches, the maximum number of matches we consider in a
         * category is limited to the total limit.
         * For example if runner 1 has 200 matches and runner 2 has 10 matches, we only consider a total of 25 matches for calculating the factor.
         */
        const int matchesInCurrentCategory = sourceModel()->rowCount(sourceParent);
        const int limitedCategoryMatchCount = qMin(matchesInCurrentCategory, m_limit);
        const float weighingFactor = (float)m_balancedMatchesCount / m_limit;
        int weighedMaxResultsForCategory = ceil(limitedCategoryMatchCount / weighingFactor);

        if (sourceRow < weighedMaxResultsForCategory) {
            // If we know this category to exceed the limit, we skip the last item
            if (m_maxMatchesBeforeShrinking <= matchesInCurrentCategory
                // And we have the last item of it, we want to skip it
                && (sourceRow + 1) == weighedMaxResultsForCategory) {
                // With out this: 18 matches
                return false;
            }
            return true;
        }
        // The item is not in the limit of the current category
        return false;
    }

private:
    // if you change this, update the default in resetLimit()
    int m_limit = 0;
    int m_matchesCount = 0;
    int m_balancedMatchesCount = 0;
    int m_maxMatchesBeforeShrinking = 0;
};

/**
 * This model hides the root items of data originally in a tree structure
 *
 * KDescendantsProxyModel collapses the items but keeps all items in tact.
 * The root items of the RunnerMatchesModel represent the individual cateories
 * which we don't want in the resulting flat list.
 * This model maps the items back to the given @c treeModel and filters
 * out any item with an invalid parent, i.e. "on the root level"
 */
class HideRootLevelProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    HideRootLevelProxyModel(QObject *parent)
        : QSortFilterProxyModel(parent)
    {
    }
    ~HideRootLevelProxyModel() override = default;

    QAbstractItemModel *treeModel() const
    {
        return m_treeModel;
    }
    void setTreeModel(QAbstractItemModel *treeModel)
    {
        m_treeModel = treeModel;
        invalidateFilter();
    }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override
    {
        KModelIndexProxyMapper mapper(sourceModel(), m_treeModel);
        const QModelIndex treeIdx = mapper.mapLeftToRight(sourceModel()->index(sourceRow, 0, sourceParent));
        return treeIdx.parent().isValid();
    }

private:
    QAbstractItemModel *m_treeModel = nullptr;
};

/**
 * Populates the IsDuplicateRole of an item
 *
 * The IsDuplicateRole returns true for each item if there is two or more
 * elements in the model with the same DisplayRole as the item.
 */
class DuplicateDetectorProxyModel : public QIdentityProxyModel
{
    Q_OBJECT

public:
    DuplicateDetectorProxyModel(QObject *parent)
        : QIdentityProxyModel(parent)
    {
    }
    ~DuplicateDetectorProxyModel() override = default;

    QVariant data(const QModelIndex &index, int role) const override
    {
        if (role != ResultsModel::DuplicateRole) {
            return QIdentityProxyModel::data(index, role);
        }

        int duplicatesCount = 0;
        const QString display = index.data(Qt::DisplayRole).toString();

        for (int i = 0; i < sourceModel()->rowCount(); ++i) {
            if (sourceModel()->index(i, 0).data(Qt::DisplayRole) == display) {
                ++duplicatesCount;

                if (duplicatesCount == 2) {
                    return true;
                }
            }
        }

        return false;
    }
};

class Q_DECL_HIDDEN ResultsModel::Private
{
public:
    Private(ResultsModel *q);

    ResultsModel *q;

    QPointer<Plasma::AbstractRunner> runner = nullptr;

    RunnerResultsModel *resultsModel;
    SortProxyModel *sortModel;
    CategoryDistributionProxyModel *distributionModel;
    KDescendantsProxyModel *flattenModel;
    HideRootLevelProxyModel *hideRootModel;
    DuplicateDetectorProxyModel *duplicateDetectorModel;
};

ResultsModel::Private::Private(ResultsModel *q)
    : q(q)
    , resultsModel(new RunnerResultsModel(q))
    , sortModel(new SortProxyModel(q))
    , distributionModel(new CategoryDistributionProxyModel(q))
    , flattenModel(new KDescendantsProxyModel(q))
    , hideRootModel(new HideRootLevelProxyModel(q))
    , duplicateDetectorModel(new DuplicateDetectorProxyModel(q))
{
}

ResultsModel::ResultsModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , d(new Private(this))
{
    connect(d->resultsModel, &RunnerResultsModel::queryStringChanged, this, &ResultsModel::queryStringChanged);
    connect(d->resultsModel, &RunnerResultsModel::queryingChanged, this, &ResultsModel::queryingChanged);
    connect(d->resultsModel, &RunnerResultsModel::queryStringChangeRequested, this, &ResultsModel::queryStringChangeRequested);

    // The matches for the old query string remain on display until the first set of matches arrive for the new query string.
    // Therefore we must not update the query string inside RunnerResultsModel exactly when the query string changes, otherwise it would
    // re-sort the old query string matches based on the new query string.
    // So we only make it aware of the query string change at the time when we receive the first set of matches for the new query string.
    connect(d->resultsModel, &RunnerResultsModel::matchesChanged, this, [this]() {
        d->sortModel->setQueryString(queryString());
    });

    connect(d->distributionModel, &CategoryDistributionProxyModel::limitChanged, this, &ResultsModel::limitChanged);

    // The data flows as follows:
    // - RunnerResultsModel
    //   - SortProxyModel
    //     - CategoryDistributionProxyModel
    //       - KDescendantsProxyModel
    //         - HideRootLevelProxyModel
    //           - DuplicateDetectorProxyModel

    d->sortModel->setSourceModel(d->resultsModel);

    d->distributionModel->setSourceModel(d->sortModel);

    d->flattenModel->setSourceModel(d->distributionModel);

    d->hideRootModel->setSourceModel(d->flattenModel);
    d->hideRootModel->setTreeModel(d->resultsModel);

    d->duplicateDetectorModel->setSourceModel(d->hideRootModel);

    setSourceModel(d->duplicateDetectorModel);
}

ResultsModel::~ResultsModel() = default;

QString ResultsModel::queryString() const
{
    return d->resultsModel->queryString();
}

void ResultsModel::setQueryString(const QString &queryString)
{
    d->resultsModel->setQueryString(queryString, runner());
}

int ResultsModel::limit() const
{
    return d->distributionModel->limit();
}

void ResultsModel::setLimit(int limit)
{
    d->distributionModel->setLimit(limit);
}

void ResultsModel::resetLimit()
{
    setLimit(0);
}

bool ResultsModel::querying() const
{
    return d->resultsModel->querying();
}

QString ResultsModel::runner() const
{
    return d->runner ? d->runner->id() : QString();
}

void ResultsModel::setRunner(const QString &runnerId)
{
    if (runnerId == runner()) {
        return;
    }
    if (runnerId.isEmpty()) {
        d->runner = nullptr;
    } else {
        d->runner = runnerManager()->runner(runnerId);
    }
    Q_EMIT runnerChanged();
}

QString ResultsModel::runnerName() const
{
    return d->runner ? d->runner->name() : QString();
}

QIcon ResultsModel::runnerIcon() const
{
    return d->runner ? d->runner->icon() : QIcon();
}

QHash<int, QByteArray> ResultsModel::roleNames() const
{
    auto names = QAbstractItemModel::roleNames();
    names[IdRole] = QByteArrayLiteral("matchId"); // "id" is QML-reserved
    names[EnabledRole] = QByteArrayLiteral("enabled");
    names[TypeRole] = QByteArrayLiteral("type");
    names[RelevanceRole] = QByteArrayLiteral("relevance");
    names[CategoryRole] = QByteArrayLiteral("category");
    names[SubtextRole] = QByteArrayLiteral("subtext");
    names[DuplicateRole] = QByteArrayLiteral("isDuplicate");
    names[ActionsRole] = QByteArrayLiteral("actions");
    names[MultiLineRole] = QByteArrayLiteral("multiLine");
    return names;
}

void ResultsModel::clear()
{
    d->resultsModel->clear();
}

bool ResultsModel::run(const QModelIndex &idx)
{
    KModelIndexProxyMapper mapper(this, d->resultsModel);
    const QModelIndex resultsIdx = mapper.mapLeftToRight(idx);
    if (!resultsIdx.isValid()) {
        return false;
    }
    return d->resultsModel->run(resultsIdx);
}

bool ResultsModel::runAction(const QModelIndex &idx, int actionNumber)
{
    KModelIndexProxyMapper mapper(this, d->resultsModel);
    const QModelIndex resultsIdx = mapper.mapLeftToRight(idx);
    if (!resultsIdx.isValid()) {
        return false;
    }
    return d->resultsModel->runAction(resultsIdx, actionNumber);
}

QMimeData *ResultsModel::getMimeData(const QModelIndex &idx) const
{
    KModelIndexProxyMapper mapper(this, d->resultsModel);
    const QModelIndex resultsIdx = mapper.mapLeftToRight(idx);
    if (!resultsIdx.isValid()) {
        return nullptr;
    }
    return d->resultsModel->mimeData({resultsIdx});
}

Plasma::RunnerManager *Milou::ResultsModel::runnerManager() const
{
    return d->resultsModel->runnerManager();
}

#include "resultsmodel.moc"
