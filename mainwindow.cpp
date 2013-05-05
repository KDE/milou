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

#include "mainwindow.h"
#include <KDebug>
#include <Soprano/Model>
#include <QBoxLayout>

#include <Soprano/QueryResultIterator>

#include <Nepomuk2/Query/FileQuery>
#include <Nepomuk2/Query/QueryParser>
#include <Nepomuk2/Query/LiteralTerm>

#include <Nepomuk2/ResourceManager>

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , m_query(0)
{
    m_edit = new QLineEdit( this );
    connect( m_edit, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged(QString)) );

    m_model = new Nepomuk2::Utils::ResourceModel( this );
    m_view = new QListView( this );
    m_view->setModel( m_model );

    QWidget* mainWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout( mainWidget );
    layout->addWidget( m_edit );
    layout->addWidget( m_view );

    setCentralWidget( mainWidget );
}

void MainWindow::slotTextChanged(const QString& text)
{
    if( m_query ) {
        m_query->deleteLater();
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

    Soprano::Util::AsyncQuery* async = Soprano::Util::AsyncQuery::executeQuery( model, sparqlQuery,
                                                                                Soprano::Query::QueryLanguageSparql );

    connect( async, SIGNAL(nextReady(Soprano::Util::AsyncQuery*)), this, SLOT(slotNextReady(Soprano::Util::AsyncQuery*) ) );
    connect( async, SIGNAL(finished(Soprano::Util::AsyncQuery*)), this, SLOT(slotFinished(Soprano::Util::AsyncQuery*)) );

    m_model->clear();
}

void MainWindow::slotNextReady(Soprano::Util::AsyncQuery* query)
{
    if( query->next() ) {
        const QUrl uri = query->binding(0).uri();
        if( uri.scheme() == QLatin1String("http") )
            return;
        m_model->addResource( uri );
    }
}

void MainWindow::slotFinished(Soprano::Util::AsyncQuery* query)
{
    m_query = 0;
}
