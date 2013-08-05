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

using namespace Nepomuk2;
using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

NepomukSource::NepomukSource(QObject* parent): AbstractSource(parent)
{
    m_size = 0;
    m_queryTask = 0;
}

void NepomukSource::query(const QString& text)
{
    if (m_queryTask) {
        m_queryTask->stop();
        m_queryTask = 0;
        m_size = 0;
    }

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

    query.setLimit(queryLimit()*5); // 5 is for the number of types we show!

    m_queryTask = new QueryRunnable(query);
    connect(m_queryTask, SIGNAL(queryResult(Nepomuk2::QueryRunnable*,Nepomuk2::Query::Result)),
            this, SLOT(slotQueryResult(Nepomuk2::QueryRunnable*,Nepomuk2::Query::Result)));
    connect(m_queryTask, SIGNAL(finished(Nepomuk2::QueryRunnable*)),
            this, SLOT(slotQueryFinished(Nepomuk2::QueryRunnable*)));

    QThreadPool::globalInstance()->start(m_queryTask);
}

void NepomukSource::slotQueryFinished(Nepomuk2::QueryRunnable* runnable)
{
    Q_UNUSED(runnable);
}

void NepomukSource::slotQueryResult(Nepomuk2::QueryRunnable* runnable, const Nepomuk2::Query::Result& result)
{
    Q_UNUSED(runnable);

    // The *2 is arbitrary. We're taking the assumption that we get the results with the types
    // jumbled up and not all of one type after another. Otherwise this would have to be 5*.
    if (m_size >= queryLimit()*2) {
        return;
    }

    Nepomuk2::Resource res(result.resource());
    const KUrl url = result.requestProperty(NIE::url()).uri();

    Match match(this);
    match.setData(QUrl(url));

    QList<QUrl> types = res.types();
    if (types.contains(NFO::Audio())) {
        match.setType("Audio");
    }
    else if (types.contains(NFO::Document())) {
        match.setType("Document");
    }
    else if (types.contains(NFO::Image())) {
        match.setType("Image");
    }
    else if (types.contains(NFO::Video())) {
        match.setType("Video");
    }
    else if (types.contains(NMO::Email())) {
        match.setType("Email");
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


