/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2021 Aleix Pol <aleixpol@kde.org>
    SPDX-FileCopyrightText: 2023 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kwaylandextras.h"

#include <QDir>
#include <QGuiApplication>
#include <QJsonArray>
#include <QPluginLoader>
#include <QStaticPlugin>
#include <QWindow>

#include "kwaylandextras_interface_p.h"

#include "kwindowsystem_debug.h"

class KWaylandExtrasPrivate
{
public:
    KWaylandExtrasPluginInterface *m_backend = nullptr;

    QString platform() const
    {
        QString platformName = QGuiApplication::platformName();

        if (platformName == QLatin1String("flatpak")) {
            // here we cannot know what is the actual windowing system, let's try it's env variable
            const auto flatpakPlatform = QString::fromLocal8Bit(qgetenv("QT_QPA_FLATPAK_PLATFORM"));
            if (!flatpakPlatform.isEmpty()) {
                platformName = flatpakPlatform;
            }
        }

        return platformName;
    }

    void loadBackend()
    {
        const QString platformName = platform();

        const QVector<QStaticPlugin> staticPlugins = QPluginLoader::staticPlugins();
        for (const QStaticPlugin &staticPlugin : staticPlugins) {
            const QJsonObject metadata = staticPlugin.metaData();
            if (metadata.value(QLatin1String("IID")) != QLatin1String(KWaylandExtrasPluginInterface_iid)) {
                continue;
            }

            if (checkPlatform(metadata, platformName)) {
                KWaylandExtrasPluginInterface *interface = qobject_cast<KWaylandExtrasPluginInterface *>(staticPlugin.instance());
                if (interface) {
                    qWarning() << "Loaded a static plugin for platform" << platformName;
                    m_backend = interface;
                    return;
                }
            }
        }

        const auto candidates = pluginCandidates();
        for (const QString &candidate : candidates) {
            if (!QLibrary::isLibrary(candidate)) {
                continue;
            }
            QPluginLoader loader(candidate);
            if (checkPlatform(loader.metaData(), platformName)) {
                KWaylandExtrasPluginInterface *interface = qobject_cast<KWaylandExtrasPluginInterface *>(loader.instance());
                if (interface) {
                    qCDebug(LOG_KWINDOWSYSTEM) << "Loaded plugin" << candidate << "for platform" << platformName;
                    m_backend = interface;
                    return;
                }
            }
        }
    }

    QStringList pluginCandidates()
    {
        QStringList ret;
        const auto paths = QCoreApplication::libraryPaths();
        for (const QString &path : paths) {
            QDir pluginDir(path + QLatin1String("/kf6/waylandextras"));

            if (!pluginDir.exists()) {
                continue;
            }
            const auto entries = pluginDir.entryList(QDir::Files | QDir::NoDotAndDotDot);
            for (const QString &entry : entries) {
                ret << pluginDir.absoluteFilePath(entry);
            }
        }
        return ret;
    }

    static bool checkPlatform(const QJsonObject &metadata, const QString &platformName)
    {
        const QJsonArray platforms = metadata.value(QStringLiteral("MetaData")).toObject().value(QStringLiteral("platforms")).toArray();
        return std::any_of(platforms.begin(), platforms.end(), [&platformName](const QJsonValue &value) {
            return QString::compare(platformName, value.toString(), Qt::CaseInsensitive) == 0;
        });
    }
};

KWaylandExtras::KWaylandExtras()
    : QObject()
    , d(new KWaylandExtrasPrivate)
{
    d->loadBackend();
    Q_ASSERT(d->m_backend);

    connect(d->m_backend, &KWaylandExtrasPluginInterface::xdgActivationTokenArrived, this, &KWaylandExtras::xdgActivationTokenArrived);
}

KWaylandExtras::~KWaylandExtras() = default;

KWaylandExtras *KWaylandExtras::self()
{
    static KWaylandExtras instance;
    return &instance;
}

void KWaylandExtras::requestXdgActivationToken(QWindow *window, uint32_t serial, const QString &app_id)
{
    self()->d->m_backend->requestXdgActivationToken(window, serial, app_id);
}

quint32 KWaylandExtras::lastInputSerial(QWindow *window)
{
    return self()->d->m_backend->lastInputSerial(window);
}
