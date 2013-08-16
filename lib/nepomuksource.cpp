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
#include <Nepomuk2/Query/ComparisonTerm>

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
    m_audioType = new MatchType(i18n("Audio"), "audio");
    m_videoType = new MatchType(i18n("Videos"), "video");
    m_imageType = new MatchType(i18n("Images"), "image");
    m_documentType = new MatchType(i18n("Documents"), "application-pdf");
    m_folderType = new MatchType(i18n("Folders"), "folder");
    m_emailType = new MatchType(i18n("Emails"), "mail-message");

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

        QueryRunnable* runnable = fetchQueryForType(text, type);
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
    KUrl url = result.requestProperty(NIE::url()).uri();

    Match match(this);
    match.setType(type);
    match.setData(QUrl(url));

    if (type == m_emailType) {
        QString subject = result.requestProperty(NMO::messageSubject()).literal().toString();
        match.setText(subject);
        match.setIcon(QLatin1String("internet-mail"));
    }
    else {
        match.setText(url.fileName());

        KMimeType::Ptr mime = KMimeType::findByUrl(url);
        if (!mime.isNull()) {
            match.setIcon(mime->iconName());
        }
    }

    addMatch(match);
    //m_resourceRetriver->requestResource(result.resource().uri());
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

QueryRunnable* NepomukSource::createQueryRunnable(const QString& sparql, const Nepomuk2::Query::RequestPropertyMap& map)
{
    QueryRunnable* queryRunnable = new QueryRunnable(sparql, map);
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
    if (name == "Videos")
        return NFO::Video();
    if (name == "Images")
        return NFO::Image();
    if (name == "Documents")
        return NFO::Document();
    if (name == "Emails")
        return NMO::Email();
    if (name == "Folders")
        return NFO::Folder();

    return QUrl();
}

namespace {
    QString createContainsPattern(const QString& var, const QString& text) {
        QStringList strList = text.split(' ');

        QStringList termList;
        QStringList regexList;
        foreach(const QString& term, strList) {
            if (term.size() < 4) {
                regexList << QString::fromLatin1("REGEX(%1, '%2.*')").arg(var, term);
                continue;
            }

            termList << QString::fromLatin1("'%1*'").arg(term);
        }

        if (regexList.isEmpty()) {
            return QString::fromLatin1("FILTER(bif:contains(%1, \"%2\")).")
                   .arg(var, termList.join(" AND "));
        }
        else {
            return QString::fromLatin1("FILTER(bif:contains(?o, \"%1\") && %2).")
                   .arg(termList.join(" AND "),
                        regexList.join(" && "));
        }
    }
}

QueryRunnable* NepomukSource::fetchQueryForType(const QString& text, MatchType* type)
{
    if (type == m_imageType) {
        QString query = QString::fromLatin1("select distinct ?r ?url where { ?r a nfo:Image ; nie:lastModified ?m . "
                                            " ?r nie:url ?url . "
                                            " ?r ?p ?o . %2 "
                                            " } ORDER BY DESC(?m) LIMIT %1")
                        .arg(QString::number(queryLimit()),
                             createContainsPattern("?o", text));

        Nepomuk2::Query::RequestPropertyMap map;
        map.insert("url", NIE::url());

        return createQueryRunnable(query, map);
    }

    if (type == m_emailType) {
        // TODO: Maybe ?p in the first union could be replaced by
        //       FILTER(?p in (plainText, messageSubject)). Need to check with the virtuoso optimizer
        //
        QString query = QString::fromLatin1("select distinct ?r ?url ?sub where { ?r a nmo:Email ; nmo:sentDate ?m . "
                                            " ?r nie:url ?url ; nmo:messageSubject ?sub ."
                                            " { ?r ?p ?o . %2 } UNION"
                                            " { ?r ?p ?r2 . ?r2 ?p2 ?o . %2 }"
                                            " } ORDER BY DESC(?m) LIMIT %1")
                        .arg(QString::number(queryLimit()),
                             createContainsPattern("?o", text));

        Nepomuk2::Query::RequestPropertyMap map;
        map.insert("url", NIE::url());
        map.insert("sub", NMO::messageSubject());

        return createQueryRunnable(query, map);
    }

    if (type == m_folderType) {
        QString query = QString::fromLatin1("select distinct ?r ?url where { ?r a nfo:Folder ; nie:lastModified ?m . "
                                            " ?r nie:url ?url . "
                                            " ?r nfo:fileName ?o . %2 "
                                            " } ORDER BY DESC(?m) LIMIT %1")
                        .arg(QString::number(queryLimit()),
                             createContainsPattern("?o", text));

        Nepomuk2::Query::RequestPropertyMap map;
        map.insert("url", NIE::url());

        return createQueryRunnable(query, map);
    }


    QString query = QString::fromLatin1("select distinct ?r ?url where { ?r a %3 ; nie:lastModified ?m . "
                                        " ?r nie:url ?url . "
                                        " { ?r ?p ?o . %2 } UNION"
                                        " { ?r ?p ?r2 . ?r2 ?p2 ?o . %2 }"
                                        " } ORDER BY DESC(?m) LIMIT %1")
                    .arg(QString::number(queryLimit()),
                         createContainsPattern("?o", text),
                         Soprano::Node::resourceToN3(fetchTypeFromName(type->name())));

    Nepomuk2::Query::RequestPropertyMap map;
    map.insert("url", NIE::url());

    return createQueryRunnable(query, map);

    /*
    QStringList strList = text.split(' ');
    QString searchString;
    foreach(const QString& str, strList) {
        searchString += str + "* ";
    }
    Query::LiteralTerm literalTerm(searchString);

    Query::ComparisonTerm ct(NIE::lastModified(), Query::Term());
    ct.setSortWeight(1, Qt::DescendingOrder);

    Query::ResourceTypeTerm typeTerm(fetchTypeFromName(type->name()));
    Query::Query query(typeTerm && literalTerm && ct);
    query.setLimit(queryLimit());

    QList<Query::Query::RequestProperty> requestProperties;
    requestProperties << Query::Query::RequestProperty(NIE::url(), false);
    query.setRequestProperties(requestProperties);

    return createQueryRunnable(query);
    */
}


