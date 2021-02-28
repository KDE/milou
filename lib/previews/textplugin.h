/*
 * This file is part of the KDE Milou Project
 * SPDX-FileCopyrightText: 2013 Vishesh Handa <me@vhanda.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 *
 */

#ifndef TEXTPLUGIN_H
#define TEXTPLUGIN_H

#include "previewplugin.h"

class TextPlugin : public Milou::PreviewPlugin
{
    Q_OBJECT
public:
    explicit TextPlugin(QObject *parent, const QVariantList &);

    QStringList mimetypes() override
    {
        return QStringList() << QLatin1String("text/");
    }
    void generatePreview() override;
};

#endif // TEXTPLUGIN_H
