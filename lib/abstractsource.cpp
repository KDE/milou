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

#include "abstractsource.h"
#include <KGlobal>

static const KCatalogLoader loader("plasma_applet_milou_applet");

using namespace Milou;

AbstractSource::AbstractSource(QObject* parent): QObject(parent)
{

}

AbstractSource::~AbstractSource()
{
    qDeleteAll(m_types);
}

void AbstractSource::run(const Match&)
{
}

void AbstractSource::addMatch(const Match& match)
{
    emit matchAdded(match);
}

int AbstractSource::queryLimit()
{
    return m_queryLimit;
}

void AbstractSource::setQueryLimit(int limit)
{
    m_queryLimit = limit;
}

void AbstractSource::setTypes(const QList< MatchType* > types)
{
    m_types = types;
}

QList< MatchType* > AbstractSource::types()
{
    return m_types;
}

void AbstractSource::stop()
{
}

