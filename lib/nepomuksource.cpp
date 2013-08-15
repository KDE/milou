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

#include <QThreadPool>
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
    m_size = 0;
    m_queryTask = 0;

    m_resourceRetriver = new AsyncNepomukResourceRetriever(QVector<QUrl>() << RDF::type(), this);
    connect(m_resourceRetriver, SIGNAL(resourceReceived(QUrl,Nepomuk2::Resource)),
            this, SLOT(slotResourceReceived(QUrl,Nepomuk2::Resource)));

    // FIXME: Find better icons!
    m_audioType = new MatchType("Audio", "audio");
    m_videoType = new MatchType("Video", "videon");
    m_imageType = new MatchType("Image", "image");
    m_documentType = new MatchType("Document", "document");
    m_folderType = new MatchType("Folder", "folder");
    m_emailType = new MatchType("Email", "mail-message");

    QList<MatchType*> types;
    types << m_audioType << m_videoType << m_imageType << m_documentType
          << m_folderType << m_emailType;

    setTypes(types);
}

NepomukSource::~NepomukSource()
{
    m_resourceRetriver->cancelAll();
}

void NepomukSource::query(const QString& text)
{
    if (m_queryTask) {
        m_queryTask->stop();
        m_resourceRetriver->cancelAll();
        m_queryTask = 0;
        m_size = 0;
    }

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
    Query::Query query(literalTerm);
    query.setLimit(queryLimit() * types().size());

    m_size = 0;
    m_queryTask = new QueryRunnable(query);
    connect(m_queryTask, SIGNAL(queryResult(Nepomuk2::QueryRunnable*,Nepomuk2::Query::Result)),
            this, SLOT(slotQueryResult(Nepomuk2::QueryRunnable*,Nepomuk2::Query::Result)));
    connect(m_queryTask, SIGNAL(finished(Nepomuk2::QueryRunnable*)),
            this, SLOT(slotQueryFinished(Nepomuk2::QueryRunnable*)));

    QThreadPool::globalInstance()->start(m_queryTask);
}

void NepomukSource::slotQueryFinished(Nepomuk2::QueryRunnable* runnable)
{
    Q_ASSERT(runnable == m_queryTask);
    m_queryTask = 0;
}


void NepomukSource::slotQueryResult(Nepomuk2::QueryRunnable* runnable, const Nepomuk2::Query::Result& result)
{
    if (!m_queryTask)
        return;

    Q_ASSERT(runnable == m_queryTask);

    // The *2 is arbitrary. We're taking the assumption that we get the results with the types
    // jumbled up and not all of one type after another. Otherwise this would have to be 5*.
    if (m_size >= queryLimit()*2) {
        m_queryTask->stop();
        m_queryTask = 0;
        return;
    }

    m_resourceRetriver->requestResource(result.resource().uri());
}

void NepomukSource::slotResourceReceived(const QUrl&, const Nepomuk2::Resource& res)
{
    const KUrl url = res.property(NIE::url()).toUrl();

    Match match(this);
    match.setData(QUrl(url));

    QList<QUrl> types = res.types();
    if (m_audioType->isShown() && types.contains(NFO::Audio())) {
        match.setType(m_audioType);
    }
    else if (m_documentType->isShown() && types.contains(NFO::Document())) {
        match.setType(m_documentType);
    }
    else if (m_imageType->isShown() && types.contains(NFO::Image())) {
        match.setType(m_imageType);
    }
    else if (m_videoType->isShown() && types.contains(NFO::Video())) {
        match.setType(m_videoType);
    }
    else if (m_folderType->isShown() && types.contains(NFO::Folder())) {
        match.setType(m_folderType);
    }
    else if (m_emailType->isShown() && types.contains(NMO::Email())) {
        match.setType(m_emailType);
    }
    else {
        return;
    }

    if (url.isLocalFile()) {
        match.setText(url.fileName());

        KMimeType::Ptr mime = KMimeType::findByFileContent(url.toLocalFile());
        if (!mime.isNull()) {
            match.setIcon(mime->iconName());
        }
    }
    else {
        // TODO: Special handling for emails. Only fetch required properties!
        match.setText(res.genericLabel());
        match.setIcon(res.genericIcon());
    }

    m_size++;
    addMatch(match);
}

void NepomukSource::run(const Match& match)
{
    QUrl url = match.data().toUrl();
    if (!url.isEmpty()) {
        QDesktopServices::openUrl(url);
    }
}


