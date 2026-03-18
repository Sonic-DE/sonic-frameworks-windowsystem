/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2003 Luboš Luňák <l.lunak@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kusertimestamp.h"

#include "kwindowsystem.h"

#include <private/qtx11extras_p.h>

#include <netwm.h>

unsigned long KUserTimestamp::userTimestamp()
{
    return QX11Info::appUserTime();
}

void KUserTimestamp::updateUserTimestamp(unsigned long time)
{
    if (time == 0) { // get current X timestamp
        time = QX11Info::getTimestamp();
    }

    if (QX11Info::appUserTime() == 0 || NET::timestampCompare(time, QX11Info::appUserTime()) > 0) { // time > appUserTime
        QX11Info::setAppUserTime(time);
    }
    if (QX11Info::appTime() == 0 || NET::timestampCompare(time, QX11Info::appTime()) > 0) { // time > appTime
        QX11Info::setAppTime(time);
    }
}
