/*
    SPDX-FileCopyrightText: 2016 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/
#include <KWindowSystem>
#include <QSignalSpy>
#include <QTest>
#include <private/qtx11extras_p.h>

#include "kselectionowner.h"
#include "kx11extras.h"

class CompositingEnabledTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testRecreatingNetEventFilter();
};

void CompositingEnabledTest::testRecreatingNetEventFilter()
{
    // Never take the compositor selection away from a running desktop session.
    if (KX11Extras::compositingActive()) {
        QSKIP("This test requires an X11 screen without a running compositor");
    }

    // Simulate enabling compositing while the NetEventFilter gets recreated.
    QSignalSpy compositingChangedSpy(KX11Extras::self(), &KX11Extras::compositingChanged);
    QVERIFY(compositingChangedSpy.isValid());
    const QByteArray selectionName =
        QByteArrayLiteral("_NET_WM_CM_S") + QByteArray::number(QX11Info::appScreen());
    KSelectionOwner compositorSelection(selectionName.constData());
    QSignalSpy claimedSpy(&compositorSelection, &KSelectionOwner::claimedOwnership);
    QVERIFY(claimedSpy.isValid());
    bool filterRecreated = false;
    connect(&compositorSelection, &KSelectionOwner::claimedOwnership, this, [&] {
        // let's connect to a signal which will cause a re-creation of NetEventFilter
        QSignalSpy workAreaChangedSpy(KX11Extras::self(), &KX11Extras::workAreaChanged);
        QVERIFY(workAreaChangedSpy.isValid());
        filterRecreated = true;
    });
    compositorSelection.claim(false);

    QTRY_COMPARE(claimedSpy.count(), 1);
    QVERIFY(filterRecreated);
    QTRY_VERIFY(KX11Extras::compositingActive());
    QCOMPARE(compositingChangedSpy.count(), 1);
    QCOMPARE(compositingChangedSpy.first().at(0).toBool(), true);

    compositorSelection.release();
    xcb_flush(QX11Info::connection());
    QTRY_VERIFY(!KX11Extras::compositingActive());
    QTRY_COMPARE(compositingChangedSpy.count(), 2);
    QCOMPARE(compositingChangedSpy.last().at(0).toBool(), false);
}

QTEST_MAIN(CompositingEnabledTest)
#include "compositingenabled_test.moc"
