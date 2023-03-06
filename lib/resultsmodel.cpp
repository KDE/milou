/*
 * SPDX-FileCopyrightText: 2019 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "resultsmodel.h"

using namespace Milou;

ResultsModel::ResultsModel(QObject *parent)
    : KRunner::ResultsModel(parent)
{
}

ResultsModel::~ResultsModel() = default;
