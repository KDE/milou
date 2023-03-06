/*
 * SPDX-FileCopyrightText: 2019 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <KRunner/ResultsModel>

namespace Milou
{
class ResultsModel : public KRunner::ResultsModel
{
    Q_OBJECT
public:
    explicit ResultsModel(QObject *parent = nullptr);
    ~ResultsModel() override;
};

} // namespace Milou
