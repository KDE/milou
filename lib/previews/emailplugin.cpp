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

#include "emailplugin.h"

#include <KUrl>
#include <KMime/Message>

#include <Akonadi/ItemFetchScope>

#include <QTextDocument>
#include <QTextCursor>
#include <QTextEdit>
#include <QTextTable>

EmailPlugin::EmailPlugin(QObject* parent, const QVariantList& )
    : PreviewPlugin(parent)
{

}

void EmailPlugin::generatePreview()
{
    if (url().scheme() != QLatin1String("akonadi")) {
        kError() << "We only support akonadi urls";
        return;
    }

    Akonadi::ItemFetchJob* job = new Akonadi::ItemFetchJob(Akonadi::Item::fromUrl(url()));
    job->fetchScope().fetchFullPayload();
    connect(job, SIGNAL(itemsReceived(Akonadi::Item::List)),
            this, SLOT(slotItemsReceived(Akonadi::Item::List)));
}

void EmailPlugin::slotItemsReceived(const Akonadi::Item::List& itemList)
{
    if (itemList.empty()) {
        return;
    }

    Akonadi::Item item = itemList.first();

    KMime::Message msg;
    //kDebug() << item.payloadData();
    msg.setContent(item.payloadData());
    msg.parse();

    KMime::Headers::Subject *subject = msg.subject();
    KMime::Headers::From *from = msg.from();
    KMime::Headers::To *to = msg.to(false);
    KMime::Headers::Date* date = msg.date(false);
    QString dateString = KGlobal::locale()->formatDateTime(date->dateTime(),
                                                           KLocale::FancyShortDate, KLocale::Seconds);
    KMime::Content *textContent = msg.textContent();

    QTextCharFormat greyCharFormat;
    greyCharFormat.setForeground(QBrush(Qt::gray));

    QTextCharFormat normalCharFormat;

    QTextCharFormat boldCharFormat;
    QFont f = boldCharFormat.font();
    f.setBold(true);
    boldCharFormat.setFont(f);

    QTextBlockFormat rightAlignment;
    rightAlignment.setAlignment(Qt::AlignRight);

    QTextBlockFormat leftAlignment;
    leftAlignment.setAlignment(Qt::AlignLeft);
    leftAlignment.setLeftMargin(3);

    QTextDocument* doc = new QTextDocument(this);
    QTextCursor cursor(doc);

    QTextTable* table = cursor.insertTable(4, 2);
    table->cellAt(0, 0).firstCursorPosition().setBlockFormat(rightAlignment);
    table->cellAt(0, 0).firstCursorPosition().insertText("From:", greyCharFormat);
    table->cellAt(0, 1).firstCursorPosition().setBlockFormat(leftAlignment);
    table->cellAt(0, 1).firstCursorPosition().insertText(from->asUnicodeString(), normalCharFormat);

    table->cellAt(1, 0).firstCursorPosition().setBlockFormat(rightAlignment);
    table->cellAt(1, 0).firstCursorPosition().insertText("To:", greyCharFormat);
    table->cellAt(1, 1).firstCursorPosition().setBlockFormat(leftAlignment);
    table->cellAt(1, 1).firstCursorPosition().insertText(to->asUnicodeString(), normalCharFormat);

    table->cellAt(2, 0).firstCursorPosition().setBlockFormat(rightAlignment);
    table->cellAt(2, 0).firstCursorPosition().insertText("Date:", greyCharFormat);
    table->cellAt(2, 1).firstCursorPosition().setBlockFormat(leftAlignment);
    table->cellAt(2, 1).firstCursorPosition().insertText(dateString, normalCharFormat);

    table->cellAt(3, 0).firstCursorPosition().setBlockFormat(rightAlignment);
    table->cellAt(3, 0).firstCursorPosition().insertText("Subject:", greyCharFormat);
    table->cellAt(3, 1).firstCursorPosition().setBlockFormat(leftAlignment);
    table->cellAt(3, 1).firstCursorPosition().insertText(subject->asUnicodeString(), boldCharFormat);

    QTextTableFormat tableFormat;
    tableFormat.setBorder(0);
    table->setFormat(tableFormat);

    cursor = table->lastCursorPosition();
    cursor.setPosition(cursor.position()+1);
    cursor.insertText("\n\n");
    insertEmailBody(cursor, textContent->decodedText(true, true));

    QTextEdit* edit = new QTextEdit();
    edit->setDocument(doc);
    edit->setReadOnly(true);
    // Maybe the height should be reduced based on the contents? Also maybe the width
    // could be increased for html emails
    edit->resize(384, 384);

    highlight(doc);
    emit previewGenerated(edit);
}

// Inserts quoted text in green
void EmailPlugin::insertEmailBody(QTextCursor& cursor, const QString& body)
{
    QTextCharFormat greenFormat;
    greenFormat.setForeground(QBrush(Qt::darkGreen));

    QString b(body);
    QTextStream stream(&b, QIODevice::ReadOnly);
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        if (line.startsWith('>')) {
            cursor.insertText(line, greenFormat);
            cursor.insertText("\n");
        }
        else {
            cursor.insertText(line, QTextCharFormat());
            cursor.insertText("\n");
        }
    }
}

MILOU_EXPORT_PREVIEW(EmailPlugin, "milouemailplugin")
