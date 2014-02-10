/*
 * This file is part of the KDE Milou Project
 * Copyright (C) 2013  Vishesh Handa <me@vhanda.in>
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

#include "match.h"
#include <KUrl>
#include <QDir>

using namespace Milou;

Match::Match(AbstractSource* source)
    : m_source(source)
{
}

Match::~Match()
{

}

AbstractSource* Match::source() const
{
    return m_source;
}

void Match::setType(MatchType* type)
{
    m_type = type;
}

MatchType* Match::type() const
{
    return m_type;
}

void Match::setText(const QString& text)
{
    m_text = text;
}

QString Match::text() const
{
    return m_text;
}

void Match::setIcon(const QString& iconName)
{
    m_icon = iconName;
}

QString Match::icon() const
{
    return m_icon;
}

void Match::setData(const QVariant& data)
{
    m_data = data;
}

QVariant Match::data() const
{
    return m_data;
}

QString Match::previewType() const
{
    return m_previewType;
}

QString Match::previewUrl() const
{
    return m_previewUrl;
}

void Match::setPreviewType(const QString& type)
{
    m_previewType = type;
}

void Match::setPreviewUrl(const QString& url)
{
    m_previewUrl = url;
}

QString Match::previewLabel()
{
    return m_previewLabel;
}

void Match::setPreviewLabel(const QString& label)
{
    m_previewLabel = label;
}

//
// Match Type
//

MatchType::MatchType()
{

}

MatchType::MatchType(const QString& name, const QString& icon)
    : m_name(name), m_icon(icon)
{

}

QString MatchType::name()
{
    return m_name;
}

void MatchType::setName(const QString& n)
{
    m_name = n;
}

QString MatchType::icon()
{
    return m_icon;
}

void MatchType::setIcon(const QString& icon)
{
    m_icon = icon;
}

