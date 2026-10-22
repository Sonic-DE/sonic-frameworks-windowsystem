// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include <KWindowSystem>
#include <KX11Extras>

#include <QGuiApplication>
#include <QSignalSpy>
#include <QTest>
#include <QWindow>

#include <memory>

class KX11ExtrasNonX11Test : public QObject {
    Q_OBJECT

  private Q_SLOTS:
    void platform()
    {
        // Preserve SonicDE's intentional X11-only compatibility predicates.
        QVERIFY(KWindowSystem::isPlatformX11());
        QVERIFY(!KWindowSystem::isPlatformWayland());
        if (qEnvironmentVariable("KX11EXTRAS_TEST_MODE") == QLatin1String("cached-x11")) {
            // Deliberately exercise a stale true platform cache, not just the
            // ordinary Unknown/offscreen case. There is still no X connection.
            QCOMPARE(KWindowSystem::platform(), KWindowSystem::Platform::X11);
        }
    }

    void queries()
    {
        QVERIFY(!KX11Extras::compositingActive());
        QVERIFY(KX11Extras::windows().isEmpty());
        QVERIFY(!KX11Extras::hasWId(1));
        QVERIFY(KX11Extras::stackingOrder().isEmpty());
        QCOMPARE(KX11Extras::activeWindow(), WId(0));
        QCOMPARE(KX11Extras::currentDesktop(), 1);
        QCOMPARE(KX11Extras::numberOfDesktops(), 1);
        QVERIFY(KX11Extras::workArea().isEmpty());
        QVERIFY(KX11Extras::workArea(QList<WId>{}).isEmpty());
        QVERIFY(KX11Extras::desktopName(1).isEmpty());
        QVERIFY(!KX11Extras::mapViewport());
        // QPixmap needs a QGuiApplication, so don't exercise these in core mode.
        if (qobject_cast<QGuiApplication *>(QCoreApplication::instance())) {
            QVERIFY(KX11Extras::icon(1).isNull());
            QVERIFY(KX11Extras::icon(1, 16, 16, true, KX11Extras::NETWM).isNull());
            QVERIFY(KX11Extras::icon(1, 16, 16, true, KX11Extras::NETWM, nullptr).isNull());
        }
    }

    void setters()
    {
        KX11Extras::activateWindow(1);
        KX11Extras::forceActiveWindow(WId(1));
        if (qobject_cast<QGuiApplication *>(QCoreApplication::instance())) {
            QWindow window;
            KX11Extras::forceActiveWindow(&window);
            // The guard must precede winId(), which would create a native window.
            QVERIFY(!window.handle());
        }
        KX11Extras::setCurrentDesktop(1);
        KX11Extras::setOnAllDesktops(1, true);
        KX11Extras::setOnDesktop(1, 1);
        KX11Extras::setOnActivities(1, {});
        KX11Extras::minimizeWindow(1);
        KX11Extras::unminimizeWindow(1);
        KX11Extras::setDesktopName(1, QStringLiteral("Desktop"));
        KX11Extras::setExtendedStrut(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        KX11Extras::setStrut(1, 0, 0, 0, 0);
        KX11Extras::setType(1, NET::Normal);
        KX11Extras::setState(1, NET::Sticky);
        KX11Extras::clearState(1, NET::Sticky);
    }

    void connectSignals()
    {
        // Connecting a signal also used to instantiate NETRootInfo with c=null.
        QSignalSpy compositing(KX11Extras::self(), &KX11Extras::compositingChanged);
        QSignalSpy struts(KX11Extras::self(), &KX11Extras::strutChanged);
        QVERIFY(compositing.isValid());
        QVERIFY(struts.isValid());
        QVERIFY(!KX11Extras::compositingActive());
        QCOMPARE(compositing.count(), 0);
        QCOMPARE(struts.count(), 0);
    }
};

int main(int argc, char **argv)
{
    const auto mode = qgetenv("KX11EXTRAS_TEST_MODE");
    if (mode == "cached-x11") {
        qputenv("QT_QPA_PLATFORM", "xcb");
        if (KWindowSystem::platform() != KWindowSystem::Platform::X11) {
            return 1;
        }
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }
    std::unique_ptr<QCoreApplication> app;
    if (mode == "core") {
        app = std::make_unique<QCoreApplication>(argc, argv);
    } else {
        app = std::make_unique<QGuiApplication>(argc, argv);
    }
    KX11ExtrasNonX11Test test;
    return QTest::qExec(&test, argc, argv);
}

#include "kx11extras_nonx11test.moc"
