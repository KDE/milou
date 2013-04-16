#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>

#include <Soprano/Statement>
#include <Soprano/Node>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>
#include <Soprano/StatementIterator>
#include <Soprano/NodeIterator>
#include <Soprano/PluginManager>

#include <Nepomuk2/Resource>
#include <Nepomuk2/ResourceWatcher>
#include <Nepomuk2/ResourceManager>
#include <Nepomuk2/Variant>
#include <Nepomuk2/File>
#include <Nepomuk2/Tag>

#include <Nepomuk2/Types/Property>
#include <Nepomuk2/Types/Class>

#include <Nepomuk2/Query/Query>
#include <Nepomuk2/Query/FileQuery>
#include <Nepomuk2/Query/ComparisonTerm>
#include <Nepomuk2/Query/LiteralTerm>
#include <Nepomuk2/Query/ResourceTerm>
#include <Nepomuk2/Query/QueryServiceClient>
#include <Nepomuk2/Query/ResourceTypeTerm>
#include <Nepomuk2/Query/QueryParser>
#include <Nepomuk2/Query/Result>
#include <Nepomuk2/Query/StandardQuery>

#include <KDebug>
#include <KStandardDirs>

// Vocabularies
#include <Soprano/Vocabulary/RDF>
#include <Soprano/Vocabulary/RDFS>
#include <Soprano/Vocabulary/NRL>
#include <Soprano/Vocabulary/NAO>

#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/NFO>
#include <Nepomuk2/Vocabulary/NMM>
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NCAL>

#include <KUrl>
#include <KJob>
#include <KTemporaryFile>
#include <KConfigGroup>
#include <Soprano/Parser>

#include <nepomuk2/datamanagement.h>
#include <nepomuk2/simpleresource.h>
#include <nepomuk2/simpleresourcegraph.h>
#include <nepomuk2/storeresourcesjob.h>

#include <iostream>
#include <QtCore/QThread>
#include <QDirIterator>
#include <QtDBus>
#include <qglobal.h>
#include <QTextDocument>

using namespace Soprano::Vocabulary;
using namespace Nepomuk2::Vocabulary;
using namespace Nepomuk2;

class TestObject : public QObject {
    Q_OBJECT

public slots:
    void main();

public:
    TestObject() {
        QTimer::singleShot( 0, this, SLOT(main()) );
    }
};


int main( int argc, char ** argv ) {
    KComponentData component( QByteArray("nepomuk-test") );
    QCoreApplication app( argc, argv );


    TestObject a;
    app.exec();
}

void TestObject::main()
{
    QCoreApplication::instance()->quit();
}


#include "main.moc"
