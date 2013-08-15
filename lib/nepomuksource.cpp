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
#include "asyncnepomukresourceretriever.h"

#include <QDesktopServices>

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
#include <Soprano/Vocabulary/RDF>

using namespace Nepomuk2;
using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

NepomukSource::NepomukSource(QObject* parent): AbstractSource(parent)
{
    m_resourceRetriver = new AsyncNepomukResourceRetriever(QVector<QUrl>() << RDF::type(), this);
    connect(m_resourceRetriver, SIGNAL(resourceReceived(QUrl,Nepomuk2::Resource)),
            this, SLOT(slotResourceReceived(QUrl,Nepomuk2::Resource)));

    // FIXME: Find better icons!
    m_audioType = new MatchType("Audio", "audio");
    m_videoType = new MatchType("Video", "video");
    m_imageType = new MatchType("Image", "image");
    m_documentType = new MatchType("Document", "application-pdf");
    m_folderType = new MatchType("Folder", "folder");
    m_emailType = new MatchType("Email", "mail-message");

    QList<MatchType*> types;
    types << m_audioType << m_videoType << m_imageType << m_documentType
          << m_folderType << m_emailType;

    setTypes(types);

    m_threadPool = new QThreadPool(this);
    m_threadPool->setMaxThreadCount(types.size());
}

NepomukSource::~NepomukSource()
{
    m_resourceRetriver->cancelAll();
}

void NepomukSource::stop()
{
    QHash<QueryRunnable*, MatchType*>::iterator it = m_queries.begin();
    for (; it != m_queries.end(); it++) {
        it.key()->stop();
    }
    m_queries.clear();
}

void NepomukSource::query(const QString& text)
{
    stop();
    if(text.length() < 4) {
        return;
    }

    kDebug() << text;

    QStringList strList = text.split(' ');
    QString searchString;
    foreach(const QString& str, strList) {
        searchString += str + "* ";
    }

    Query::LiteralTerm literalTerm(searchString);

    foreach(MatchType* type, types()) {
        if (!type->shown())
            continue;

        // FIXME: Create customized queries for each of these
        Query::ResourceTypeTerm typeTerm(fetchTypeFromName(type->name()));
        Query::Query query(typeTerm && literalTerm);
        query.setLimit(queryLimit());

        QList<Query::Query::RequestProperty> requestProperties;
        requestProperties << Query::Query::RequestProperty(NIE::url(), false);
        query.setRequestProperties(requestProperties);

        QueryRunnable* runnable = createQueryRunnable(query);
        m_queries.insert(runnable, type);
        // TODO: Take type priority into account?
        m_threadPool->start(runnable);
    }
}

void NepomukSource::slotQueryFinished(Nepomuk2::QueryRunnable* runnable)
{
    m_queries.remove(runnable);
}


void NepomukSource::slotQueryResult(Nepomuk2::QueryRunnable* runnable, const Nepomuk2::Query::Result& result)
{
    if (!m_queries.contains(runnable)) {
        return;
    }

    MatchType* type = m_queries.value(runnable);
    if (type->name() != "Email") {
        KUrl url = result.requestProperty(NIE::url()).uri();

        Match match(this);
        match.setText(url.fileName());
        match.setType(type);
        match.setData(QUrl(url));

        KMimeType::Ptr mime = KMimeType::findByUrl(url);
        if (!mime.isNull()) {
            match.setIcon(mime->iconName());
        }

        addMatch(match);
    }
    else {
        m_resourceRetriver->requestResource(result.resource().uri());
    }
}

void NepomukSource::slotResourceReceived(const QUrl&, const Nepomuk2::Resource& res)
{
    const KUrl url = res.property(NIE::url()).toUrl();

    Match match(this);
    match.setData(QUrl(url));

    QList<QUrl> types = res.types();
    if (types.contains(NMO::Email())) {
        match.setType(m_emailType);
    }
    else {
        return;
    }

    // TODO: Special handling for emails. Only fetch required properties!
    match.setText(res.genericLabel());
    match.setIcon(res.genericIcon());

    addMatch(match);
}

void NepomukSource::run(const Match& match)
{
    QUrl url = match.data().toUrl();
    if (!url.isEmpty()) {
        QDesktopServices::openUrl(url);
    }
}

QueryRunnable* NepomukSource::createQueryRunnable(const Query::Query& query)
{
    QueryRunnable* queryRunnable = new QueryRunnable(query);
    connect(queryRunnable, SIGNAL(queryResult(Nepomuk2::QueryRunnable*,Nepomuk2::Query::Result)),
            this, SLOT(slotQueryResult(Nepomuk2::QueryRunnable*,Nepomuk2::Query::Result)));
    connect(queryRunnable, SIGNAL(finished(Nepomuk2::QueryRunnable*)),
            this, SLOT(slotQueryFinished(Nepomuk2::QueryRunnable*)));

    return queryRunnable;
}

QUrl NepomukSource::fetchTypeFromName(const QString& name)
{
    if (name == "Audio")
        return NFO::Audio();
    if (name == "Video")
        return NFO::Video();
    if (name == "Image")
        return NFO::Image();
    if (name == "Document")
        return NFO::Document();
    if (name == "Email")
        return NMO::Email();
    if (name == "Folder")
        return NFO::Folder();

    return QUrl();
}

