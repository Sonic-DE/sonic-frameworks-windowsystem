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
    auto dv2 = dynamic_cast<KWindowSystemPrivateV2 *>(KWindowSystem::d_func());
    if (!dv2) {
        // Ensure that xdgActivationTokenArrived is always emitted asynchronously
        QTimer::singleShot(0, [serial] {
            Q_EMIT KWaylandExtras::self()->xdgActivationTokenArrived(serial, {});
        });

        return;
    }
    dv2->requestToken(window, serial, app_id);
}
#endif

quint32 KWaylandExtras::lastInputSerial(QWindow *window)
{
    auto dv2 = dynamic_cast<KWindowSystemPrivateV2 *>(KWindowSystem::d_func());
    if (!dv2) {
        return 0;
    }
    return dv2->lastInputSerial(window);
}

void KWaylandExtras::exportWindow(QWindow *window)
{
    if (auto dv2 = dynamic_cast<KWindowSystemPrivateV2 *>(KWindowSystem::d_func())) {
        dv2->exportWindow(window);
    }
}

void KWaylandExtras::unexportWindow(QWindow *window)
{
    if (auto dv2 = dynamic_cast<KWindowSystemPrivateV2 *>(KWindowSystem::d_func())) {
        dv2->unexportWindow(window);
    }
}

class KXdgActivationTokenRequest : public QObject
{
    Q_OBJECT

public:
    explicit KXdgActivationTokenRequest(QWindow *window, uint32_t serial, const QString &appId)
        : m_serial(serial)
    {
        m_promise.start();

        connect(KWaylandExtras::self(), &KWaylandExtras::xdgActivationTokenArrived, this, [this](int serial, const QString &token) {
            if (m_serial == uint32_t(serial)) {
                if (!m_promise.isCanceled()) {
                    m_promise.addResult(token);
                }
                m_promise.finish();
                delete this;
            }
        });

        if (auto platform = dynamic_cast<KWindowSystemPrivateV2 *>(KWindowSystem::d_func())) {
            platform->requestToken(window, serial, appId);
        } else {
            QTimer::singleShot(0, this, [serial] {
                Q_EMIT KWaylandExtras::self()->xdgActivationTokenArrived(serial, {});
            });
        }
    }

    QFuture<QString> future() const
    {
        return m_promise.future();
    }

private:
    QPromise<QString> m_promise;
    uint32_t m_serial;
};

QFuture<QString> KWaylandExtras::xdgActivationToken(QWindow *window, uint32_t serial, const QString &appId)
{
    auto request = new KXdgActivationTokenRequest(window, serial, appId);
    return request->future();
}

QFuture<QString> KWaylandExtras::xdgActivationToken(QWindow *window, const QString &appId)
{
    return xdgActivationToken(window, lastInputSerial(window), appId);
}

#include "kwaylandextras.moc"
#include "moc_kwaylandextras.cpp"
