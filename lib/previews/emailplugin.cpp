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
    KMime::Content *textContent = msg.textContent();

    QTextCharFormat greyCharFormat;
    greyCharFormat.setForeground(QBrush(Qt::gray));

    QTextCharFormat normalCharFormat;

    QTextCharFormat boldCharFormat;
    QFont f = boldCharFormat.font();
    f.setBold(true);
    boldCharFormat.setFont(f);

    QTextDocument* doc = new QTextDocument(this);
    QTextCursor cursor(doc);
    cursor.insertText("To: ", greyCharFormat);
    cursor.insertText(to->asUnicodeString() + "\n", normalCharFormat);

    cursor.insertText("Subject: ", greyCharFormat);
    cursor.insertText(subject->asUnicodeString() + "\n", boldCharFormat);

    cursor.insertText("Date: ", greyCharFormat);
    cursor.insertText(date->asUnicodeString() + "\n", normalCharFormat);

    cursor.insertText("From: ", greyCharFormat);
    cursor.insertText(from->asUnicodeString() + "\n", normalCharFormat);

    cursor.insertText("\n");
    cursor.insertText(textContent->decodedText(true, true), normalCharFormat);

    QTextEdit* edit = new QTextEdit();
    edit->setDocument(doc);
    edit->setReadOnly(true);

    emit previewGenerated(edit);
}

MILOU_EXPORT_PREVIEW(EmailPlugin, "milouemailplugin")
