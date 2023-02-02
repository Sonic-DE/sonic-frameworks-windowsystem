/*
    SPDX-FileCopyrightText: 2023 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef KWAYLANDEXTRASINTERFACE_P_H
#define KWAYLANDEXTRASINTERFACE_P_H

#include "kwindowsystem_export.h"

#include <QObject>
#include <QWindow>

#define KWaylandExtrasPluginInterface_iid "org.kde.kwindowsystem.KWaylandExtrasPluginInterface"

class KWINDOWSYSTEM_EXPORT KWaylandExtrasPluginInterface : public QObject
{
    Q_OBJECT
public:
    explicit KWaylandExtrasPluginInterface(QObject *parent = nullptr);
    ~KWaylandExtrasPluginInterface() override;

    virtual void requestXdgActivationToken(QWindow *win, uint32_t serial, const QString &app_id) = 0;
    virtual quint32 lastInputSerial(QWindow *window) = 0;

Q_SIGNALS:
    void xdgActivationTokenArrived(int serial, const QString &token);
};

Q_DECLARE_INTERFACE(KWaylandExtrasPluginInterface, KWaylandExtrasPluginInterface_iid)

#endif
