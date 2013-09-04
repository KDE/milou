/*
 *   Copyright (C) 2013 Vishesh Handa
 *
 *   Code adapted from kde-workspace/plasma/generic/runners/calculatorrunner.h
 *   Copyright (C) 2007 Barış Metin <baris@pardus.org.tr>
 *   Copyright (C) 2010 Matteo Agostinelli <agostinelli@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2 as
 *   published by the Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef CALCULATORSOURCE_H
#define CALCULATORSOURCE_H

#include <QMimeData>
#include <QRegExp>

#include "abstractsource.h"

/**
 * This class evaluates the basic expressions given in the interface.
 */
class CalculatorSource : public AbstractSource
{
    Q_OBJECT

    public:
        explicit CalculatorSource(QObject* parent);

        virtual void query(const QString& string);

    private:
        QString calculate(const QString& term);
        void userFriendlySubstitutions(QString& cmd);
        void powSubstitutions(QString& cmd);
        void hexSubstitutions(QString& cmd);

        QRegExp m_regExp;

        MatchType* m_calculatorType;
};

#endif
