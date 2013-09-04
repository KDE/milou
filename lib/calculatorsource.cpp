/*
 *   Copyright (C) 2013 Vishesh Handa
 *
 *   Code adapted from kde-workspace/plasma/generic/runners/calculatorrunner.cpp
 *   Copyright (C) 2007 Barış Metin <baris@pardus.org.tr>
 *   Copyright (C) 2006 David Faure <faure@kde.org>
 *   Copyright (C) 2007 Richard Moore <rich@kde.org>
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

#include "calculatorsource.h"

#include <QScriptEngine>

#include <KIcon>
#include <KDebug>
#include <KGlobal>
#include <KLocalizedString>
#include <KLocale>

CalculatorSource::CalculatorSource(QObject* parent)
    : AbstractSource(parent),
      m_regExp("[a-zA-Z]")
{
    m_calculatorType = new MatchType(i18n("Calculator"), QString("accessories-calculator"));

    QList<MatchType*> types;
    types << m_calculatorType;

    setTypes(types);
}

void CalculatorSource::powSubstitutions(QString& cmd)
{
    if (cmd.contains("e+", Qt::CaseInsensitive)) {
        cmd = cmd.replace("e+", "*10^", Qt::CaseInsensitive);
    }

    if (cmd.contains("e-", Qt::CaseInsensitive)) {
        cmd = cmd.replace("e-", "*10^-", Qt::CaseInsensitive);
    }

    // the below code is scary mainly because we have to honor priority
    // honor decimal numbers and parenthesis.
    while (cmd.contains('^')) {
        int where = cmd.indexOf('^');
        cmd = cmd.replace(where, 1, ',');
        int preIndex = where - 1;
        int postIndex = where + 1;
        int count = 0;

        QChar decimalSymbol = KGlobal::locale()->decimalSymbol().at(0);
        //avoid out of range on weird commands
        preIndex = qMax(0, preIndex);
        postIndex = qMin(postIndex, cmd.length()-1);

        //go backwards looking for the beginning of the number or expression
        while (preIndex != 0) {
            QChar current = cmd.at(preIndex);
            QChar next = cmd.at(preIndex-1);
            //kDebug() << "index " << preIndex << " char " << current;
            if (current == ')') {
                count++;
            } else if (current == '(') {
                count--;
            } else {
                if (((next <= '9' ) && (next >= '0')) || next == decimalSymbol) {
                    preIndex--;
                    continue;
                }
            }
            if (count == 0) {
                //check for functions
                if (!((next <= 'z' ) && (next >= 'a'))) {
                    break;
                }
            }
            preIndex--;
        }

       //go forwards looking for the end of the number or expression
        count = 0;
        while (postIndex != cmd.size() - 1) {
            QChar current=cmd.at(postIndex);
            QChar next=cmd.at(postIndex + 1);

            //check for functions
            if ((count == 0) && (current <= 'z') && (current >= 'a')) {
                postIndex++;
                continue;
            }

            if (current == '(') {
                count++;
            } else if (current == ')') {
                count--;
            } else {
                if (((next <= '9' ) && (next >= '0')) || next == decimalSymbol) {
                    postIndex++;
                    continue;
                 }
            }
            if (count == 0) {
                break;
            }
            postIndex++;
        }

        preIndex = qMax(0, preIndex);
        postIndex = qMin(postIndex, cmd.length());

        cmd.insert(preIndex,"pow(");
        // +1 +4 == next position to the last number after we add 4 new characters pow(
        cmd.insert(postIndex + 1 + 4, ')');
        //kDebug() << "from" << preIndex << " to " << postIndex << " got: " << cmd;
    }
}

void CalculatorSource::hexSubstitutions(QString& cmd)
{
    if (cmd.contains("0x")) {
        //Append +0 so that the calculator can serve also as a hex converter
        cmd.append("+0");
        bool ok;
        int pos = 0;
        QString hex;

        while (cmd.contains("0x")) {
            hex.clear();
            pos = cmd.indexOf("0x", pos);

            for (int q = 0; q < cmd.size(); q++) {//find end of hex number
                QChar current = cmd[pos+q+2];
                if (((current <= '9' ) && (current >= '0')) || ((current <= 'F' ) && (current >= 'A')) || ((current <= 'f' ) && (current >= 'a'))) { //Check if valid hex sign
                    hex[q] = current;
                } else {
                    break;
                }
            }
            cmd = cmd.replace(pos, 2+hex.length(), QString::number(hex.toInt(&ok,16))); //replace hex with decimal
        }
    }
}

void CalculatorSource::userFriendlySubstitutions(QString& cmd)
{
    if (cmd.contains(KGlobal::locale()->decimalSymbol(), Qt::CaseInsensitive)) {
         cmd=cmd.replace(KGlobal::locale()->decimalSymbol(), QChar('.'), Qt::CaseInsensitive);
    }

    // the following substitutions are not needed with libqalculate
    #ifndef ENABLE_QALCULATE
    hexSubstitutions(cmd);
    powSubstitutions(cmd);

    if (cmd.contains(QRegExp("\\d+and\\d+"))) {
         cmd = cmd.replace(QRegExp("(\\d+)and(\\d+)"), "\\1&\\2");
    }
    if (cmd.contains(QRegExp("\\d+or\\d+"))) {
         cmd = cmd.replace(QRegExp("(\\d+)or(\\d+)"), "\\1|\\2");
    }
    if (cmd.contains(QRegExp("\\d+xor\\d+"))) {
         cmd = cmd.replace(QRegExp("(\\d+)xor(\\d+)"), "\\1^\\2");
    }
    #endif
}


void CalculatorSource::query(const QString& string)
{
    QString cmd = string;

    //no meanless space between friendly guys: helps simplify code
    cmd = cmd.trimmed().remove(' ');

    if (cmd.length() < 3) {
        return;
    }

    if (cmd.toLower() == "universe" || cmd.toLower() == "life") {
        Match match(this);
        match.setType(m_calculatorType);
        match.setIcon(QLatin1String("accessories-calculator"));
        match.setText("42");
        match.setData("42");
        addMatch(match);
        return;
    }

    bool toHex = cmd.startsWith(QLatin1String("hex="));
    bool startsWithEquals = !toHex && cmd[0] == '=';

    if (toHex || startsWithEquals) {
        cmd.remove(0, cmd.indexOf('=') + 1);
    } else if (cmd.endsWith('=')) {
        cmd.chop(1);
    }

    if (!toHex && cmd.contains(m_regExp)) {
        // not just numbers and symbols, so we return
        return;
    }

    if (cmd.isEmpty()) {
        return;
    }

    userFriendlySubstitutions(cmd);
    #ifndef ENABLE_QALCULATE
    cmd.replace(QRegExp("([a-zA-Z]+)"), "Math.\\1"); //needed for accessing math funktions like sin(),....
    #endif

    QString result = calculate(cmd);
    if (!result.isEmpty() && result != cmd) {
        if (toHex) {
            result = "0x" + QString::number(result.toInt(), 16).toUpper();
        }

        Match match(this);
        match.setType(m_calculatorType);
        match.setIcon(QLatin1String("accessories-calculator"));
        match.setText(result);
        match.setData(result);

        addMatch(match);
    }
}

QString CalculatorSource::calculate(const QString& term)
{
    #ifdef ENABLE_QALCULATE
    QString result;

    try {
        result = m_engine->evaluate(term);
    } catch(std::exception& e) {
        kDebug() << "qalculate error: " << e.what();
    }

    return result.replace('.', KGlobal::locale()->decimalSymbol(), Qt::CaseInsensitive);
    #else
    //kDebug() << "calculating" << term;
    QScriptEngine eng;
    QScriptValue result = eng.evaluate(" var result ="+term+"; result");

    if (result.isError()) {
        return QString();
    }

    const QString resultString = result.toString();
    if (resultString.isEmpty()) {
        return QString();
    }

    if (!resultString.contains('.')) {
        return resultString;
    }

    //ECMAScript has issues with the last digit in simple rational computations
    //This script rounds off the last digit; see bug 167986
    QString roundedResultString = eng.evaluate("var exponent = 14-(1+Math.floor(Math.log(Math.abs(result))/Math.log(10)));\
                                                var order=Math.pow(10,exponent);\
                                                (order > 0? Math.round(result*order)/order : 0)").toString();

    roundedResultString.replace('.', KGlobal::locale()->decimalSymbol(), Qt::CaseInsensitive);

    return roundedResultString;
    #endif
}
