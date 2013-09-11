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

#include "audioplugin.h"

#include <KGlobal>
#include <KStandardDirs>
#include <KDebug>

#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#include <QDateTime>

#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/NMM>
#include <Nepomuk2/Vocabulary/NFO>

using namespace Nepomuk2::Vocabulary;

AudioPlugin::AudioPlugin(QObject* parent, const QVariantList&): PreviewPlugin(parent)
{
    m_pool = new QThreadPool(this);
    m_pool->setMaxThreadCount(2);
}

void AudioPlugin::generatePreview()
{
    const QString query = QString::fromLatin1("select ?title ?per ?album ?duration where {"
                                              " ?r nie:url %1 . "
                                              " OPTIONAL { ?r nie:title ?title . }"
                                              " OPTIONAL { ?r nfo:duration ?duration . }"
                                              " OPTIONAL { ?r nmm:performer ?a . ?a nco:fullname ?per . }"
                                              " OPTIONAL { ?r nmm:musicAlbum ?al . ?al nie:title ?album . }"
                                              " } LIMIT 1")
                          .arg(Soprano::Node::resourceToN3(url()));

    Nepomuk2::Query::RequestPropertyMap map;
    map.insert("title", NIE::title());
    map.insert("per", NMM::performer());
    map.insert("album", NMM::musicAlbum());
    map.insert("duration", NFO::duration());

    Nepomuk2::QueryRunnable* runnable = new Nepomuk2::QueryRunnable(query, map);
    connect(runnable, SIGNAL(queryResult(Nepomuk2::QueryRunnable*,Nepomuk2::Query::Result)),
            this, SLOT(queryResult(Nepomuk2::QueryRunnable*,Nepomuk2::Query::Result)));

    m_pool->start(runnable);
}

void AudioPlugin::queryResult(Nepomuk2::QueryRunnable* , const Nepomuk2::Query::Result& results)
{
    QString qmlFile = KGlobal::dirs()->findResource("data", "plasma/plasmoids/org.kde.milou/contents/ui/previews/Audio.qml");

    QDeclarativeComponent* component = new QDeclarativeComponent(context()->engine(), qmlFile, this);
    if (component->status() == QDeclarativeComponent::Error) {
        kError() << component->errorString();
        return;
    }

    QDeclarativeItem* item = qobject_cast<QDeclarativeItem*>(component->create());
    item->setProperty("title", results.requestProperty(NIE::title()).literal().toString());

    QStringList keys;
    keys << i18n("Artist: ") << i18n("Album: ") << i18n("Duration: ");

    QStringList values;
    values << results.requestProperty(NMM::performer()).literal().toString();
    values << results.requestProperty(NMM::musicAlbum()).literal().toString();

    int duration = results.requestProperty(NFO::duration()).literal().toInt();
    QTime time;
    time = time.addSecs(duration);
    values << time.toString();

    item->setProperty("keys", QVariant::fromValue(keys));
    item->setProperty("values", QVariant::fromValue(values));
    item->setProperty("length", keys.length());

    emit previewGenerated(item);
}


MILOU_EXPORT_PREVIEW(AudioPlugin, "milouaudioplugin")
