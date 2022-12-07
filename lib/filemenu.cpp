/**
 * SPDX-FileCopyrightText: 2022 Kai Uwe Broulik <kde@broulik.de>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "filemenu.h"

#include <QClipboard>
#include <QDir>
#include <QGuiApplication>
#include <QIcon>
#include <QMenu>
#include <QMimeData>
#include <QQuickWindow>

#include <KDesktopFile>
#include <KDesktopFileActions>
#include <KFileItem>
#include <KFileItemActions>
#include <KFileItemListProperties>
#include <KLocalizedString>
#include <KNotificationJobUiDelegate>
#include <KProtocolManager>
#include <KUrlMimeData>

#include <KIO/ApplicationLauncherJob>
#include <KIO/OpenFileManagerWindowJob>

using namespace Milou;

FileMenu::FileMenu(QObject *parent)
    : QObject(parent)
{
}

FileMenu::~FileMenu() = default;

QUrl FileMenu::url() const
{
    return m_url;
}

void FileMenu::setUrl(const QUrl &url)
{
    bool valid = url.isValid();

    QUrl resolvedUrl = url;
    if (url.scheme() == QLatin1String("applications")) {
        if (KService::Ptr service = KService::serviceByStorageId(url.path())) {
            QString path = service->entryPath();
            if (!QDir::isAbsolutePath(path)) {
                path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kservices5/") + path);
            }

            resolvedUrl = QUrl::fromLocalFile(path);
        } else {
            valid = false;
        }
    }

    if (m_url != resolvedUrl) {
        if (KDesktopFile::isDesktopFile(resolvedUrl.toLocalFile())) {
            // Don't offer any menu for applications without actions.
            KDesktopFile df(resolvedUrl.toLocalFile());
            if (df.readActions().isEmpty()) {
                valid = false;
            }
        }

        m_url = resolvedUrl;
        Q_EMIT urlChanged(m_url);
    }

    if (m_valid != valid) {
        m_valid = valid;
        Q_EMIT validChanged(valid);
    }
}

QQuickItem *FileMenu::visualParent() const
{
    return m_visualParent.data();
}

void FileMenu::setVisualParent(QQuickItem *visualParent)
{
    if (m_visualParent.data() == visualParent) {
        return;
    }

    if (m_visualParent) {
        disconnect(m_visualParent.data(), nullptr, this, nullptr);
    }
    m_visualParent = visualParent;
    if (m_visualParent) {
        connect(m_visualParent.data(), &QObject::destroyed, this, &FileMenu::visualParentChanged);
    }
    Q_EMIT visualParentChanged();
}

bool FileMenu::valid() const
{
    return m_valid;
}

bool FileMenu::visible() const
{
    return m_visible;
}

void FileMenu::open(int x, int y)
{
    if (!m_valid || !m_visualParent || !m_visualParent->window()) {
        return;
    }

    const bool isDesktopFile = KDesktopFile::isDesktopFile(m_url.toLocalFile());

    QMenu *menu = new QMenu();
    menu->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(menu, &QMenu::triggered, this, &FileMenu::actionTriggered);
    connect(menu, &QMenu::aboutToHide, this, [this] {
        m_visible = false;
        Q_EMIT visibleChanged();
    });

    KFileItem fileItem(m_url);

    if (!isDesktopFile && KProtocolManager::supportsListing(m_url)) {
        QAction *openContainingFolderAction = menu->addAction(QIcon::fromTheme(QStringLiteral("folder-open")), i18nc("@action:incontextmenu", "Open Containing Folder"));
        connect(openContainingFolderAction, &QAction::triggered, this, [this] {
            KIO::highlightInFileManager({m_url});
        });
    }

    if (isDesktopFile) {
        // TODO recent documents

        KService::Ptr service = KService::serviceByDesktopPath(fileItem.localPath());
        if (service) {
            const auto actions = KDesktopFileActions::userDefinedServices(*service, true);
            for (const auto &action : actions) {
                auto *menuAction = menu->addAction(QIcon::fromTheme(action.icon()), action.text());
                connect(menuAction, &QAction::triggered, this, [this, action] {
                    auto *job = new KIO::ApplicationLauncherJob(action, this);
                    job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
                    job->start();
                });
            }
        }
    } else {
        KFileItemActions *actions = new KFileItemActions(menu);
        KFileItemListProperties itemProperties(KFileItemList({fileItem}));
        actions->setItemListProperties(itemProperties);
        actions->setParentWidget(menu);

        actions->insertOpenWithActionsTo(nullptr, menu, QStringList());

        menu->addSeparator();

        QAction *copyAction = menu->addAction(QIcon::fromTheme(QStringLiteral("edit-copy")), i18nc("@action:incontextmenu", "&Copy"));
        connect(copyAction, &QAction::triggered, [fileItem] {
            // inspired by KDirModel::mimeData()
            QMimeData *data = new QMimeData(); // who cleans it up?
            KUrlMimeData::setUrls({fileItem.url()}, {fileItem.mostLocalUrl()}, data);
            qApp->clipboard()->setMimeData(data);
        });

        QAction *copyPathAction = menu->addAction(QIcon::fromTheme(QStringLiteral("edit-copy-path")), i18nc("@action:incontextmenu", "Copy Location"));
        connect(copyPathAction, &QAction::triggered, [fileItem] {
            QString path = fileItem.localPath();
            if (path.isEmpty()) {
                path = fileItem.url().toDisplayString();
            }
            qApp->clipboard()->setText(path);
        });

        menu->addSeparator();

        // TODO trash/delete actions?

        // menu->addSeparator();

        actions->addActionsTo(menu);

        // TODO file properties dialog?
    }

    menu->adjustSize();

    QPoint pos;
    if (x == -1 && y == -1) { // align "bottom left of visualParent"
        menu->adjustSize();

        pos = m_visualParent->mapToGlobal(QPointF(0, m_visualParent->height())).toPoint();

        if (!qApp->isRightToLeft()) {
            pos.rx() += m_visualParent->width();
            pos.rx() -= menu->width();
        }
    } else {
        pos = m_visualParent->mapToGlobal(QPointF(x, y)).toPoint();
    }

    menu->setAttribute(Qt::WA_TranslucentBackground);
    menu->winId();
    menu->windowHandle()->setTransientParent(m_visualParent->window());
    menu->popup(pos);

    m_visible = true;
    Q_EMIT visibleChanged();
}
