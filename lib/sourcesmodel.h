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

#ifndef SOURCESMODEL_H
#define SOURCESMODEL_H

#include <QAbstractItemModel>
#include <QVector>

#include "abstractsource.h"
#include "milou_export.h"

class MILOU_EXPORT SourcesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString queryString READ queryString WRITE setQueryString)
    Q_PROPERTY(int queryLimit READ queryLimit WRITE setQueryLimit)
public:
    explicit SourcesModel(QObject* parent = 0);
    virtual ~SourcesModel();

    enum Roles {
        TypeRole = Qt::UserRole + 1,
        PreviewTypeRole,
        PreviewUrlRole,
        PreviewLabelRole
    };

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    QString queryString() const;
    int queryLimit() const;

public slots:
    void setQueryString(const QString& str);
    void setQueryLimit(int limit);
    void clear();

    void run(int index);
    void loadSettings();

private slots:
    void slotMatchAdded(const Match& m);

public:
    // The types are ordered based on the preference
    QVector<QString> m_types;

    struct TypeData {
        QList<Match> shown;
        QList<Match> hidden;
    };
    QHash<QString, TypeData> m_matches;
    int m_size;

    QString m_queryString;
    int m_queryLimit;

    QList<AbstractSource*> m_sources;

    /// Returns the number of visible rows before \p type
    int fetchRowCount(const QString& type) const;
    Match fetchMatch(int row) const;
};

#endif // SOURCESMODEL_H
