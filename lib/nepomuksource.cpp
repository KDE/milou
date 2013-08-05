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

#include "nepomuksource.h"

#include <QThreadPool>

#include <KIcon>
#include <KDebug>
#include <KMimeType>

#include <Soprano/QueryResultIterator>

#include <Nepomuk2/Query/FileQuery>
#include <Nepomuk2/Query/QueryParser>
#include <Nepomuk2/Query/LiteralTerm>
#include <Nepomuk2/Query/ResourceTypeTerm>

#include <Nepomuk2/Resource>
#include <Nepomuk2/Variant>
#include <Nepomuk2/ResourceManager>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/NFO>
#include <Nepomuk2/Vocabulary/NMM>
#include <Nepomuk2/Vocabulary/NMO>

#include <Soprano/Model>
#include <Soprano/Vocabulary/NAO>

using namespace Nepomuk2;
using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

NepomukSource::NepomukSource(QObject* parent): AbstractSource(parent)
{

}

void NepomukSource::query(const QString& text)
{
    if( text.length() < 4 ) {
        return;
    }

    kDebug() << text;

    QStringList strList = text.split(' ');
    QString searchString;
    foreach(const QString& str, strList) {
        searchString += str + "* ";
    }

    Query::LiteralTerm literalTerm(searchString);
    Query::Query query(literalTerm);

    QList<Nepomuk2::Query::Query::RequestProperty> properties;
    properties << Nepomuk2::Query::Query::RequestProperty(NIE::url(), true);
    query.setRequestProperties(properties);

    //query.setLimit(m_queryLimit);

    QueryRunnable* task = new QueryRunnable(query);
    connect(task, SIGNAL(queryResult(Nepomuk2::QueryRunnable*,Nepomuk2::Query::Result)),
            this, SLOT(slotQueryResult(Nepomuk2::QueryRunnable*,Nepomuk2::Query::Result)));
    connect(task, SIGNAL(finished(Nepomuk2::QueryRunnable*)),
            this, SLOT(slotQueryFinished(Nepomuk2::QueryRunnable*)));

    QThreadPool::globalInstance()->start(task);
}

void NepomukSource::slotQueryFinished(Nepomuk2::QueryRunnable* runnable)
{
    Q_UNUSED(runnable);
}

void NepomukSource::slotQueryResult(Nepomuk2::QueryRunnable* runnable, const Nepomuk2::Query::Result& result)
{
    Q_UNUSED(runnable);

    Nepomuk2::Resource res(result.resource());

    Match match;
    match.url = result.requestProperty(NIE::url()).uri();

    QList<QUrl> types = res.types();
    if (types.contains(NFO::Audio())) {
        match.type = "Audio";
    }
    else if (types.contains(NFO::Document())) {
        match.type = "Document";
    }
    else if (types.contains(NFO::Image())) {
        match.type = "Image";
    }
    else if (types.contains(NFO::Video())) {
        match.type = "Video";
    }
    else if (types.contains(NMO::Email())) {
        match.type = "Email";
    }
    else {
        return;
    }

    match.nepomukUri = res.uri();
    if (match.url.isLocalFile()) {
        match.displayLabel = KUrl(match.url).fileName();

        KMimeType::Ptr mime = KMimeType::findByFileContent(match.url.toLocalFile());
        if (!mime.isNull()) {
            match.icon = mime->iconName();
        }
    }
    else {
        // TODO: Special handling for emails. Only fetch required properties!
        match.displayLabel = res.genericLabel();
        match.icon = res.genericIcon();
    }

    addMatch(match);
}

