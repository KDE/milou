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

#ifndef MATCH_H
#define MATCH_H

#include <QString>
#include <QVariant>

class AbstractSource;

/**
 * @class Match match.h
 *
 * @short A match returned by an AbstractSource
 */
class Match
{
public:
    explicit Match(AbstractSource* source);
    ~Match();

    AbstractSource* source();

    /**
     * Return a string which categorizes the result. This string
     * should ideally be registered in the AbstractSource
     */
    QString type() const;
    void setType(const QString& type);

    /**
     * Returns the text which will be displayed to the user
     */
    QString text() const;
    void setText(const QString& text);

    QString icon() const;
    void setIcon(const QString& iconName);

    QVariant data() const;
    void setData(const QVariant& data);

private:
    AbstractSource* m_source;
    QString m_type;
    QString m_text;
    QString m_icon;
    QVariant m_data;
};

#endif // MATCH_H
