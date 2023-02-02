/*
    SPDX-FileCopyrightText: 2023 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "kwaylandextras_interface_p.h"

#include "waylandxdgactivationv1_p.h"

class KWaylandExtrasWaylandPlugin : public KWaylandExtrasPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KWaylandExtrasPluginInterface_iid FILE "waylandplugin.json")
    Q_INTERFACES(KWaylandExtrasPluginInterface)

public:
    void requestXdgActivationToken(QWindow *win, uint32_t serial, const QString &app_id) override;
    quint32 lastInputSerial(QWindow *window) override;

private:
    WaylandXdgActivationV1 activation;
};
