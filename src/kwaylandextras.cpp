/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2021 Aleix Pol <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2023 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kwaylandextras.h"

#include "kwindowsystem.h"
#include "kwindowsystem_p.h"

#include <QTimer>

KWaylandExtras::KWaylandExtras()
    : QObject()
{
}

KWaylandExtras::~KWaylandExtras() = default;

KWaylandExtras *KWaylandExtras::self()
{
    static KWaylandExtras instance;
    return &instance;
}

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(6, 19)
void KWaylandExtras::requestXdgActivationToken(QWindow *window, uint32_t serial, const QString &app_id)
{
    // Stub: no-op because Wayland is not available
    Q_UNUSED(window);
    Q_UNUSED(serial);
    Q_UNUSED(app_id);
}
#endif

quint32 KWaylandExtras::lastInputSerial(QWindow *window)
{
    // Stub: return 0 because Wayland is not available
    Q_UNUSED(window);
    return 0;
}

#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(6, 27)
void KWaylandExtras::exportWindow(QWindow *window)
{
    // Stub: no-op because Wayland is not available
    Q_UNUSED(window);
}

void KWaylandExtras::unexportWindow(QWindow *window)
{
    // Stub: no-op because Wayland is not available
    Q_UNUSED(window);
}
#endif

QFuture<QString> KWaylandExtras::exportToplevel(QWindow *window)
{
    if (auto dv5 = dynamic_cast<KWindowSystemPrivateV5 *>(KWindowSystem::d_func())) {
        return dv5->exportToplevel(window);
    }
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(6, 27)
    exportWindow(window);
    QPromise<QString> promise;
    promise.start();
    auto future = promise.future();
    auto connection = std::make_unique<QMetaObject::Connection>();
    auto &connectionRef = *connection;
    connectionRef =
        connect(self(),
                &KWaylandExtras::windowExported,
                window,
                [wanted = window, promise = std::move(promise), connection = std::move(connection)](QWindow *window, const QString &handle) mutable {
                    if (window != wanted) {
                        return;
                    }
                    promise.addResult(handle);
                    promise.finish();
                    QObject::disconnect(*connection);
                });

    return future;
#else
    return QtFuture::makeReadyValueFuture(QString());
#endif
}

void KWaylandExtras::unexportToplevel(QWindow *window)
{
    if (auto dv5 = dynamic_cast<KWindowSystemPrivateV5 *>(KWindowSystem::d_func())) {
        dv5->unexportToplevel(window);
    }
#if KWINDOWSYSTEM_BUILD_DEPRECATED_SINCE(6, 27)
    else {
        unexportWindow(window);
    }
#endif
}

QFuture<QString> KWaylandExtras::xdgActivationToken(QWindow *window, uint32_t serial, const QString &appId)
{
    // Stub: return empty future because Wayland is not available
    Q_UNUSED(window);
    Q_UNUSED(serial);
    Q_UNUSED(appId);
    return QtFuture::makeReadyValueFuture(QString());
}

QFuture<QString> KWaylandExtras::xdgActivationToken(QWindow *window, const QString &appId)
{
    // Stub: return empty future because Wayland is not available
    Q_UNUSED(window);
    Q_UNUSED(appId);
    return QtFuture::makeReadyValueFuture(QString());
}

void KWaylandExtras::setXdgToplevelTag(QWindow *window, const QString &tag)
{
    // Stub: no-op because Wayland is not available
    Q_UNUSED(window);
    Q_UNUSED(tag);
}

void KWaylandExtras::setXdgToplevelDescription(QWindow *window, const QString &description)
{
    // Stub: no-op because Wayland is not available
    Q_UNUSED(window);
    Q_UNUSED(description);
}

#include "moc_kwaylandextras.cpp"
