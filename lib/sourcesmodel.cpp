/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2013  Vishesh Handa <me@vhanda.in>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "sourcesmodel.h"
#include "nepomuksource.h"

SourcesModel::SourcesModel(QObject* parent)
: QAbstractListModel(parent)
{
    AbstractSource* source = new NepomukSource(this);
    connect(source, SIGNAL(matchAdded(Match)), this, SLOT(slotMatchAdded(Match)));

    m_sources << source;

    QHash<int, QByteArray> roles = roleNames();
    roles.insert(UrlRole, "url");
    roles.insert(TypeRole, "type");

    setRoleNames(roles);
}

SourcesModel::~SourcesModel()
{
}

QVariant SourcesModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_matches.size())
        return QVariant();

    Match m = m_matches[index.row()];
    switch(role) {
        case Qt::DisplayRole:
            return m.displayLabel;

        case Qt::DecorationRole:
            return m.icon;

        case TypeRole:
            return m.type;

        case UrlRole:
            return m.url;
    }

    return QVariant();
}

int SourcesModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return m_matches.size();
}

QString SourcesModel::queryString()
{
    return m_queryString;
}

void SourcesModel::setQueryString(const QString& str)
{
    if (str.trimmed() == m_queryString.trimmed()) {
        return;
    }
    m_queryString = str;

    beginResetModel();
    m_matches.clear();
    endResetModel();

    foreach (AbstractSource* source, m_sources) {
        source->query(str);
    }
}

void SourcesModel::slotMatchAdded(const Match& m)
{
    beginInsertRows(QModelIndex(), m_matches.size(), m_matches.size());
    m_matches << m;
    endInsertRows();
}

void SourcesModel::clear()
{
    beginResetModel();
    m_matches.clear();
    endResetModel();
}

