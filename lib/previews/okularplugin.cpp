/*
 * This file is part of the KDE Milou Project
 * Copyright (C) 2013  Vishesh Handa <me@vhanda.in>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "okularplugin.h"

#include <KParts/ReadOnlyPart>
#include <KDebug>

OkularPlugin::OkularPlugin(QObject* parent, const QVariantList& ): PreviewPlugin(parent)
{
}

void OkularPlugin::generatePreview()
{
    // FIXME: You will need to create your own config file, so that the last accessed page
    //        is not opened
    KService::Ptr service = KService::serviceByDesktopName("okular_part");
    if (service) {
        QVariantList args;
        args << QLatin1String("ViewerWidget");

        KParts::ReadOnlyPart* part = service->createInstance<KParts::ReadOnlyPart>(this, args);
        part->openUrl(url());

        QWidget* widget = part->widget();
        widget->resize(384, 384);
        emit previewGenerated(widget);
    }
    else {
        kError() << "Could not load okular service!";
    }
}


MILOU_EXPORT_PREVIEW(OkularPlugin, "milouokularplugin", "milou")
