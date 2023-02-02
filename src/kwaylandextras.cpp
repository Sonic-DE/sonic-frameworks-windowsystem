/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2021 Aleix Pol <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2023 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kwaylandextras.h"

#include <QGuiApplication>
#include <QWindow>
#include <private/qwaylanddisplay_p.h>
#include <private/qwaylandinputdevice_p.h>
#include <private/qwaylandwindow_p.h>
#include <qpa/qplatformnativeinterface.h>

#include "waylandxdgactivationv1_p.h"

class KWaylandExtrasPrivate
{
public:
    WaylandXdgActivationV1 activation;
};

KWaylandExtras::KWaylandExtras()
    : QObject()
    , d(new KWaylandExtrasPrivate)
{
}

KWaylandExtras::~KWaylandExtras() = default;

KWaylandExtras *KWaylandExtras::self()
{
    static KWaylandExtras instance;
    return &instance;
}

void KWaylandExtras::requestXdgActivationToken(QWindow *window, uint32_t serial, const QString &app_id)
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

    if (!self()->d->activation.isActive()) {
        // Ensure that xdgActivationTokenArrived is always emitted asynchronously
        QTimer::singleShot(0, [serial] {
            Q_EMIT self()->xdgActivationTokenArrived(serial, {});
        });
        return;
    }

    auto waylandWindow = window ? dynamic_cast<QtWaylandClient::QWaylandWindow *>(window->handle()) : nullptr;
    auto seat = waylandWindow ? waylandWindow->display()->defaultInputDevice()->wl_seat() : nullptr;
    auto tokenReq = self()->d->activation.requestXdgActivationToken(seat, wlSurface, serial, app_id);
    connect(tokenReq, &WaylandXdgActivationTokenV1::failed, self(), [serial, app_id]() {
        Q_EMIT self()->xdgActivationTokenArrived(serial, {});
    });
    connect(tokenReq, &WaylandXdgActivationTokenV1::done, self(), [serial](const QString &token) {
        Q_EMIT self()->xdgActivationTokenArrived(serial, token);
    });
}

quint32 KWaylandExtras::lastInputSerial(QWindow *window)
{
    auto waylandWindow = window ? dynamic_cast<QtWaylandClient::QWaylandWindow *>(window->handle()) : nullptr;
    if (!waylandWindow) {
        // Should never get here
        return 0;
    }
    return waylandWindow->display()->lastInputSerial();
}
