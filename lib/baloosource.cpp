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

#include "baloosource.h"

#include <QDesktopServices>

#include <KIcon>
#include <KDebug>
#include <KMimeType>
#include <KLocalizedString>

BalooSource::BalooSource(QObject* parent)
    : AbstractSource(parent)
    , m_runnable(0)
{
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

    m_typeHash.insert(m_audioType, QLatin1String("File/Audio"));
    m_typeHash.insert(m_videoType, QLatin1String("File/Video"));
    m_typeHash.insert(m_imageType, QLatin1String("File/Image"));
    m_typeHash.insert(m_documentType, QLatin1String("File/Document"));
    m_typeHash.insert(m_folderType, QLatin1String("File/Folder"));
    m_typeHash.insert(m_emailType, QLatin1String("Akonadi/Email"));

    m_threadPool = new QThreadPool(this);
    m_threadPool->setMaxThreadCount(types.size());
    m_threadPool->expiryTimeout();
}

BalooSource::~BalooSource()
{
}

void BalooSource::stop()
{
    if (m_runnable)
        m_runnable->stop();
}

void BalooSource::query(const Context& context)
{
    stop();

    const QString text = context.query();

    if (text.isEmpty())
        return;

    QList<MatchType*> matchTypes;
    foreach(MatchType* type, context.types()) {
        if (!types().contains(type))
            continue;

        matchTypes << type;
    }

    m_runnable = new Milou::BalooRunnable(text, m_typeHash, matchTypes, queryLimit());
    connect(m_runnable, SIGNAL(queryResult(MatchType*, Baloo::Result)),
            this, SLOT(slotQueryResult(MatchType*, Baloo::Result)), Qt::QueuedConnection);
    connect(m_runnable, SIGNAL(queryFinished()),
            this, SLOT(slotQueryFinished()), Qt::QueuedConnection);

    m_threadPool->start(m_runnable);
}

void BalooSource::slotQueryFinished()
{
    m_runnable = 0;
}

void BalooSource::slotQueryResult(MatchType* type, const Baloo::Result& result)
{
    KUrl url = result.url();

    Match match(this);
    match.setType(type);
    match.setData(QUrl(url));
    match.setPreviewUrl(url.url());
    match.setText(result.text());

    if (type == m_emailType) {
        match.setPreviewLabel(match.text());
        match.setIcon(QLatin1String("internet-mail"));
        match.setPreviewType(QLatin1String("message/rfc822"));
    }
    else {
        KMimeType::Ptr mime = KMimeType::findByUrl(url);
        if (!mime.isNull()) {
            match.setIcon(mime->iconName());
            match.setPreviewType(mime->name());
        }
    }

    addMatch(match);
}

void BalooSource::run(const Match& match)
{
    QUrl url = match.data().toUrl();
    if (!url.isEmpty()) {
        QDesktopServices::openUrl(url);
    }
}

