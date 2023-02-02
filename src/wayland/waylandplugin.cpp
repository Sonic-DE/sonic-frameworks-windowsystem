/*
    SPDX-FileCopyrightText: 2023 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "waylandplugin.h"

#include <QGuiApplication>

#include <private/qwaylanddisplay_p.h>
#include <private/qwaylandinputdevice_p.h>
#include <private/qwaylandwindow_p.h>
#include <qpa/qplatformnativeinterface.h>

void KWaylandExtrasWaylandPlugin::requestXdgActivationToken(QWindow *window, uint32_t serial, const QString &app_id)
{
    wl_surface *wlSurface = [](QWindow *window) -> wl_surface * {
        if (!window) {
            return nullptr;
        }

        QPlatformNativeInterface *native = qGuiApp->platformNativeInterface();
        if (!native) {
            return nullptr;
        }
        window->create();
        return reinterpret_cast<wl_surface *>(native->nativeResourceForWindow(QByteArrayLiteral("surface"), window));
    }(window);

    if (!activation.isActive()) {
        // Ensure that xdgActivationTokenArrived is always emitted asynchronously
        QTimer::singleShot(0, [this, serial] {
            Q_EMIT xdgActivationTokenArrived(serial, {});
        });
        return;
    }

    auto waylandWindow = window ? dynamic_cast<QtWaylandClient::QWaylandWindow *>(window->handle()) : nullptr;
    auto seat = waylandWindow ? waylandWindow->display()->defaultInputDevice()->wl_seat() : nullptr;
    auto tokenReq = activation.requestXdgActivationToken(seat, wlSurface, serial, app_id);
    connect(tokenReq, &WaylandXdgActivationTokenV1::failed, this, [this, serial, app_id]() {
        Q_EMIT xdgActivationTokenArrived(serial, {});
    });
    connect(tokenReq, &WaylandXdgActivationTokenV1::done, this, [this, serial](const QString &token) {
        Q_EMIT xdgActivationTokenArrived(serial, token);
    });
}

quint32 KWaylandExtrasWaylandPlugin::lastInputSerial(QWindow *window)
{
    auto waylandWindow = window ? dynamic_cast<QtWaylandClient::QWaylandWindow *>(window->handle()) : nullptr;
    if (!waylandWindow) {
        // Should never get here
        return 0;
    }
    return waylandWindow->display()->lastInputSerial();
}
