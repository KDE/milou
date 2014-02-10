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

namespace Milou {

class AbstractSource;
class MatchType;

/**
 * @class Match match.h
 *
 * @short A match returned by an AbstractSource
 */
class Match
{
public:
    explicit Match(AbstractSource* source = 0);
    ~Match();

    AbstractSource* source() const;

    /**
     * Return a type which categorizes the result. This type
     * should ideally be registered in the AbstractSource
     */
    MatchType* type() const;
    void setType(MatchType* type);

    /**
     * Returns the text which will be displayed to the user
     */
    QString text() const;
    void setText(const QString& text);

    QString icon() const;
    void setIcon(const QString& iconName);

    QVariant data() const;
    void setData(const QVariant& data);

    QString previewType() const;
    void setPreviewType(const QString& type);

    QString previewUrl() const;
    void setPreviewUrl(const QString& url);

    /**
     * This different from the normal text. It is shown
     * when a preview is being display, and can therefore
     * be much longer.
     *
     * It's by default the folder path if the preview url
     * is local, otherwise it returns the text
     */
    QString previewLabel();
    void setPreviewLabel(const QString& label);
private:
    AbstractSource* m_source;
    MatchType* m_type;

    QString m_text;
    QString m_icon;
    QVariant m_data;

    QString m_previewType;
    QString m_previewUrl;
    QString m_previewLabel;
};


class MatchType {
public:
    MatchType();
    MatchType(const QString& name, const QString& icon);

    QString name();
    void setName(const QString& n);

    QString icon();
    void setIcon(const QString& icon);

private:
    QString m_name;
    QString m_icon;
};

}

#endif // MATCH_H
