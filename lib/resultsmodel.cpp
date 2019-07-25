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

#include "resultsmodel.h"

#include "runnerresultsmodel.h"

#include <QIdentityProxyModel>

#include <KDescendantsProxyModel>
#include <KModelIndexProxyMapper>

#include <cmath>

using namespace Milou;

class SortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    SortProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
    {
        setDynamicSortFilter(true);
        sort(0, Qt::DescendingOrder);
    }
    ~SortProxyModel() override = default;

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
};

class CategoryDistributionProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    CategoryDistributionProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
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
            connect(sourceModel, &QAbstractItemModel::rowsInserted, this, &CategoryDistributionProxyModel::invalidateFilter);
            connect(sourceModel, &QAbstractItemModel::rowsMoved, this, &CategoryDistributionProxyModel::invalidateFilter);
            connect(sourceModel, &QAbstractItemModel::rowsRemoved, this, &CategoryDistributionProxyModel::invalidateFilter);
        }
    }

    int limit() const { return m_limit; }
    void setLimit(int limit) {
        if (m_limit == limit) {
            return;
        }
        m_limit = limit;
        invalidateFilter();
        emit limitChanged();
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

        // One category may take up to half the available space
        // Each category at least gets 1 result in any case
        // TODO don't limit if there is just one category?
        int maxItemsInCategory = 1;
        int itemsBefore = 0;
        for (int i = 0; i <= sourceParent.row(); ++i) {
            const int itemsInCategory = sourceModel()->rowCount(sourceModel()->index(i, 0));

            maxItemsInCategory = std::max(1.0, std::ceil((m_limit - itemsBefore) / 2.0));

            itemsBefore += std::min(itemsInCategory, maxItemsInCategory);
        }

        if (sourceRow >= maxItemsInCategory) {
            return false;
        }

        return true;
    }

private:
    // if you change this, update the default in resetLimit()
    int m_limit = 0;
};

class HideRootLevelProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    HideRootLevelProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
    {

    }
    ~HideRootLevelProxyModel() override = default;

    QAbstractItemModel *treeModel() const {
        return m_treeModel;
    }
    void setTreeModel(QAbstractItemModel *treeModel) {
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

class DuplicateDetectorProxyModel : public QIdentityProxyModel
{
    Q_OBJECT

public:
    DuplicateDetectorProxyModel(QObject *parent) : QIdentityProxyModel(parent)
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

    QString runner;

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
    connect(d->resultsModel, &RunnerResultsModel::runnerChanged, this, &ResultsModel::runnerChanged);
    connect(d->resultsModel, &RunnerResultsModel::queryStringChangeRequested, this, &ResultsModel::queryStringChangeRequested);

    connect(d->distributionModel, &CategoryDistributionProxyModel::limitChanged, this, &ResultsModel::limitChanged);

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
    d->resultsModel->setQueryString(queryString);
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
    return d->resultsModel->runner();
}

void ResultsModel::setRunner(const QString &runner)
{
    d->resultsModel->setRunner(runner);
}

QString ResultsModel::runnerName() const
{
    return d->resultsModel->runnerName();
}

QIcon ResultsModel::runnerIcon() const
{
    return d->resultsModel->runnerIcon();
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
    return names;
}

void ResultsModel::clear()
{

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

#include "resultsmodel.moc"
