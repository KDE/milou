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

#include "finderapplet.h"
#include <Soprano/Model>

#include <QDeclarativeEngine>
#include <QDeclarativeContext>

#include <KDebug>
#include <KGlobal>
#include <KStandardDirs>
#include <KMimeType>
#include <KRun>

#include <Soprano/QueryResultIterator>

#include <Nepomuk2/Query/FileQuery>
#include <Nepomuk2/Query/QueryParser>
#include <Nepomuk2/Query/LiteralTerm>

#include <Nepomuk2/ResourceManager>
#include <Nepomuk2/File>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Variant>

using namespace Nepomuk2::Vocabulary;

FinderApplet::FinderApplet(QObject* parent, const QVariantList& args)
    : Plasma::Applet(parent, args)
    , m_query(0)
{
    m_declarative = new Plasma::DeclarativeWidget(this);
    m_model = new Nepomuk2::Utils::ResourceModel( this );

    setAutoFillBackground( true );
    setMinimumHeight( 300 );
    setMinimumWidth( 100 );
    /*
    m_view = new QListView( this );
    m_view->setModel( m_model );

    QWidget* mainWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout( mainWidget );
    layout->addWidget( m_edit );
    layout->addWidget( m_view );

    setCentralWidget( mainWidget );
    */
}

void FinderApplet::init()
{
    Plasma::Applet::init();

    kDebug() << "APPLET ID:" << id();

    if (m_declarative) {
        QString qmlFile = KGlobal::dirs()->findResource("data", "plasma/plasmoids/org.kde.nepomuk.finder/contents/ui/main.qml");
        kDebug() << "LOADING: " << qmlFile;
        m_declarative->setQmlPath(qmlFile);

        QDeclarativeContext* context = m_declarative->engine()->rootContext();
        context->setContextProperty( "finderApplet", this );
        context->setContextProperty( "resultsModel", m_model );

        //setSearchText( "Coldplay" );
    }
}

void FinderApplet::setSearchText(const QString& text)
{
    if( m_query ) {
        m_query->close();
        delete m_query;
        m_query = 0;
    }

    if( text.length() < 4 ) {
        m_model->clear();
        return;
    }

    kDebug() << text;

    Nepomuk2::Query::LiteralTerm literalTerm( text + "*" );
    Nepomuk2::Query::Query query( literalTerm );
    query.setLimit( 10 );
    QString sparqlQuery = query.toFileQuery().toSparqlQuery();

    kDebug() << sparqlQuery;
    Soprano::Model* model = Nepomuk2::ResourceManager::instance()->mainModel();

    m_query = Soprano::Util::AsyncQuery::executeQuery( model, sparqlQuery, Soprano::Query::QueryLanguageSparql );

    connect( m_query, SIGNAL(nextReady(Soprano::Util::AsyncQuery*)), this, SLOT(slotNextReady(Soprano::Util::AsyncQuery*) ) );
    connect( m_query, SIGNAL(finished(Soprano::Util::AsyncQuery*)), this, SLOT(slotFinished(Soprano::Util::AsyncQuery*)) );

    m_model->clear();
}

void FinderApplet::slotNextReady(Soprano::Util::AsyncQuery* query)
{
    if( query->next() ) {
        const QUrl uri = query->binding(0).uri();
        if( uri.scheme() == QLatin1String("http") )
            return;
        kDebug() << uri;
        m_model->addResource( uri );
    }
}

void FinderApplet::slotFinished(Soprano::Util::AsyncQuery* query)
{
    Q_ASSERT(m_query == query);
    m_query = 0;
}

void FinderApplet::run(int index)
{
    Nepomuk2::Resource res = m_model->resourceForIndex( m_model->index( index, 0 ) );

    const KUrl nieUrl = res.toFile().url();
    if( !nieUrl.isEmpty() ) {
        kDebug() << "Running!!!" << nieUrl;
        KRun::runUrl( nieUrl, res.property(NIE::mimeType()).toString(), 0 );
    }
}


#include "finderapplet.moc"

K_EXPORT_PLASMA_APPLET(org.kde.nepomuk.finder, FinderApplet)
