/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 1999 Matthias Ettrich <ettrich@kde.org>
    SPDX-FileCopyrightText: 2007 Lubos Lunak <l.lunak@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef KX11EXTRAS_H
#define KX11EXTRAS_H

#include <QObject>
#include <QWindow>

#include <kwindowsystem_export.h>

#include "netwm_def.h"

class NETWinInfo;
class NETEventFilter;

/*!
 * \qmltype KX11Extras
 * \inqmlmodule org.kde.kwindowsystem
 * \brief A collection of functions to obtain information from and manipulate
 * X11 windows. These are generally not applicable to other window systems.
 *
 * \since 5.101
 */
class KWINDOWSYSTEM_EXPORT KX11Extras : public QObject
{
    Q_OBJECT

    /*!
     * \property KX11Extras::compositingActive
     * \brief Whether desktop compositing is active
     */
    Q_PROPERTY(bool compositingActive READ compositingActive NOTIFY compositingChanged)

public:
    static KX11Extras *self();

    /*!
     * Returns the list of all toplevel windows currently managed by the
     * window manager in the order of creation. Please do not rely on
     * indexes of this list: Whenever you enter Qt's event loop in your
     * application, it may happen that entries are removed or added.
     * Your module should perhaps work on a copy of this list and verify a
     * window with hasWId() before any operations.
     *
     * Iteration over this list can be done easily with
     * \code
     *  QList<WId> windows = KWindowSystem::windows();
     *  for (auto it = windows.cbegin(), end = windows.cend(); it != end; ++it) {
     *     ... do something here,  (*it) is the current WId.
     *  }
     * \endcode
     * Returns the list of all toplevel windows
     */
    static QList<WId> windows();

    /*!
     * Test to see if \a id still managed at present.
     * \a id the window id to test
     * Returns true if the window id is still managed
     **/
    static bool hasWId(WId id);

    /*!
     * Returns the list of all toplevel windows currently managed by the
     * window manager in the current stacking order (from lower to
     * higher). May be useful for pagers.
     * Returns the list of all toplevel windows in stacking order
     */
    static QList<WId> stackingOrder();

    /*!
     * Returns the currently active window, or 0 if no window is active.
     * Returns the window id of the active window, or 0 if no window is
     *  active
     **/
    static WId activeWindow();

    /*!
     * Requests that window \a win is activated.
     *
     * There are two ways how to activate a window, by calling
     * activateWindow() and forceActiveWindow(). Generally,
     * applications shouldn't make attempts to explicitly activate
     * their windows, and instead let the user to activate them.
     * In the special cases where this may be needed, applications
     * should use activateWindow(). Window manager may consider whether
     * this request wouldn't result in focus stealing, which
     * would be obtrusive, and may refuse the request.
     *
     * The usage of forceActiveWindow() is meant only for pagers
     * and similar tools, which represent direct user actions
     * related to window manipulation.
     * Except for rare cases, this request will be always honored,
     * and normal applications are forbidden to use it.
     *
     * In case of problems, consult the KWin README in the kdebase
     * package (kdebase/kwin/README), or ask on the kwin@kde.org
     * mailing list.
     *
     * \a win the id of the window to make active
     * \a time X server timestamp of the user activity that
     *    caused this request
     */
    static void activateWindow(WId win, long time = 0);

    /*!
     * Sets window \a win to be the active window. Note that this
     * should be called only in special cases, applications
     * shouldn't force themselves or other windows to be the active
     * window. Generally, this call should used only by pagers
     * and similar tools. See the explanation in description
     * of activateWindow().
     *
     * \a win the id of the window to make active
     * \a time X server timestamp of the user activity that
     *    caused this request
     */
    static void forceActiveWindow(WId win, long time = 0);

    /*!
     * Sets window \a win to be the active window. Note that this
     * should be called only in special cases, applications
     * shouldn't force themselves or other windows to be the active
     * window. Generally, this call should used only by pagers
     * and similar tools. See the explanation in the description
     * of activateWindow().
     *
     * \a win the window to make active
     * \a time X server timestamp of the user activity that
     *    caused this request
     * \since 6.0
     */
    Q_INVOKABLE static void forceActiveWindow(QWindow *window, long time = 0);

    /*!
     * Returns true if a compositing manager is running (i.e. ARGB windows
     * are supported, effects will be provided, etc.).
     */
    static bool compositingActive();

    /*!
     * Returns the current virtual desktop.
     * Returns the current virtual desktop
     **/
    static int currentDesktop();

    /*!
     * Returns the number of virtual desktops.
     * Returns the number of virtual desktops
     **/
    static int numberOfDesktops();

    /*!
     * Convenience function to set the current desktop to \a desktop.
     * See NETRootInfo.
     * \a desktop the number of the new desktop
     */
    static void setCurrentDesktop(int desktop);

    /*!
     * Sets window \a win to be present on all virtual desktops if @p
     * is true. Otherwise the window lives only on one single desktop.
     *
     * \a win the id of the window
     * \a b true to show the window on all desktops, false
     *          otherwise
     */
    static void setOnAllDesktops(WId win, bool b);

    /*!
     * Moves window \a win to desktop \a desktop.
     *
     * \a win the id of the window
     * \a desktop the number of the new desktop
     */
    static void setOnDesktop(WId win, int desktop);

    /*!
     * Moves window \a win to activities \a activities.
     *
     * \a win the id of the window
     * \a activities the list of activity UUIDs
     *
     * \sa KWindowInfo::activities
     */
    static void setOnActivities(WId win, const QStringList &activities);

    /*!
     * Returns an icon for window \a win.
     *
     * If  \a width and \a height are specified, the best icon for the requested
     * size is returned.
     *
     * If \a scale is true, the icon is smooth-scaled to have exactly
     * the requested size.
     *
     * \a win the id of the window
     * \a width the desired width, or -1
     * \a height the desired height, or -1
     * \a scale if true the icon will be scaled to the desired size. Otherwise the
     *        icon will not be modified.
     * Returns the icon of the window
     */
    static QPixmap icon(WId win, int width = -1, int height = -1, bool scale = false);

    /*!
     * Masks specifying from which sources to read an icon. They are tried from the best
     * until an icon is found.
     * @li NETWM from property from the window manager specification
     * @li WMHints from WMHints property
     * @li ClassHint load icon after getting name from the classhint
     * @li XApp load the standard X icon (last fallback)
     */
    enum IconSource {
        NETWM = 1, //!< read from property from the window manager specification
        WMHints = 2, //!< read from WMHints property
        ClassHint = 4, //!< load icon after getting name from the classhint
        XApp = 8, //!< load the standard X icon (last fallback)
    };
    /*!
     * @overload
     *
     * Overloaded variant that allows specifying from which sources the icon should be read.
     * You should usually prefer the simpler variant which tries all possibilities to get
     * an icon.
     *
     * \a win the id of the window
     * \a width the desired width, or -1
     * \a height the desired height, or -1
     * \a scale if true the icon will be scaled to the desired size. Otherwise the
     *        icon will not be modified.
     * \a flags OR-ed flags from the IconSource enum
     */
    static QPixmap icon(WId win, int width, int height, bool scale, int flags);

    /*!
     * @overload
     *
     * Overloaded variant that allows passing in the NETWinInfo to use for reading the
     * information. This variant is only useful on the X11 platform, other platforms do not
     * use NETWinInfo and delegate to the variant without NETWinInfo. Though if compiled with
     * X11 support the X11 variant is used on other platforms if info is not \c nullptr.
     * This can be used by applications using e.g. platform wayland but also connecting to an
     * XServer.
     *
     * The NETWinInfo must be constructed with property NET::WMIcon in order to use the
     * IconSource flag NETWM. NET::WM2IconPixmap for IconSource flag WMHints and
     * NET::WM2WindowClass for IconSource flag ClassHint.
     *
     * \a win the id of the window
     * \a width the desired width, or -1
     * \a height the desired height, or -1
     * \a scale if true the icon will be scaled to the desired size. Otherwise the
     *        icon will not be modified.
     * \a flags OR-ed flags from the IconSource enum
     * \a into the NETWinInfo to use for reading properties.
     **/
    static QPixmap icon(WId win, int width, int height, bool scale, int flags, NETWinInfo *info);

    /*!
     * Minimizes the window with id \a win.
     * On X11 this follows the protocol described in ICCCM section 4.1.4.
     *
     * \a win The window to minimize
     * \sa unminimizeWindow()
     */
    static void minimizeWindow(WId win);
    /*!
     * Unminimizes the window with id \a win.
     * On X11 this follows the protocol described in ICCCM section 4.1.4.
     *
     * \a win The window to unminimize
     * \sa minimizeWindow()
     **/
    static void unminimizeWindow(WId win);

    /*!
     * Returns the workarea for the specified desktop, or the current
     * work area if no desktop has been specified.
     * \a desktop the number of the desktop to check, -1 for the
     *        current desktop
     * Returns the size and position of the desktop
     **/
    static QRect workArea(int desktop = -1);

    /*!
     * Returns the workarea for the specified desktop, or the current
     * work area if no desktop has been specified. Excludes struts of
     * clients in the exclude List.
     *
     * \a excludes the list of clients whose struts will be excluded
     * \a desktop the number of the desktop to check, -1 for the
     *        current desktop
     * Returns the size and position of the desktop
     **/
    static QRect workArea(const QList<WId> &excludes, int desktop = -1);

    /*!
     * Returns the name of the specified desktop.
     * \a desktop the number of the desktop
     * Returns the name of the desktop
     **/
    static QString desktopName(int desktop);

    /*!
     * Sets the name of the specified desktop.
     * \a desktop the number of the desktop
     * \a name the new name for the desktop
     **/
    static void setDesktopName(int desktop, const QString &name);

    /*!
     * Function that reads and returns the contents of the given text
     * property (WM_NAME, WM_ICON_NAME,...).
     */
    static QString readNameProperty(WId window, unsigned long atom);

    /*!
     * Returns true if viewports are mapped to virtual desktops.
     */
    static bool mapViewport();

    /*!
     * Sets the strut of window \a win to \a left_width
     * ranging from \a left_start to \a left_end on the left edge,
     * and simiarly for the other edges. For not reserving a strut, pass 0 as the width.
     * E.g. to reserve 10x10 square in the topleft corner, use e.g.
     * setExtendedStrut( w, 10, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 ).
     *
     * \a win the id of the window
     * \a left_width width of the strut at the left edge
     * \a left_start starting y coordinate of the strut at the left edge
     * \a left_end ending y coordinate of the strut at the left edge
     * \a right_width width of the strut at the right edge
     * \a right_start starting y coordinate of the strut at the right edge
     * \a right_end ending y coordinate of the strut at the right edge
     * \a top_width width of the strut at the top edge
     * \a top_start starting x coordinate of the strut at the top edge
     * \a top_end ending x coordinate of the strut at the top edge
     * \a bottom_width width of the strut at the bottom edge
     * \a bottom_start starting x coordinate of the strut at the bottom edge
     * \a bottom_end ending x coordinate of the strut at the bottom edge
     */
    static void setExtendedStrut(WId win,
                                 qreal left_width,
                                 qreal left_start,
                                 qreal left_end,
                                 qreal right_width,
                                 qreal right_start,
                                 qreal right_end,
                                 qreal top_width,
                                 qreal top_start,
                                 qreal top_end,
                                 qreal bottom_width,
                                 qreal bottom_start,
                                 qreal bottom_end);
    /*!
     * Convenience function for setExtendedStrut() that automatically makes struts
     * as wide/high as the screen width/height.
     * Sets the strut of window \a win to \a left, \a right, \a top, \a bottom.
     *
     * \a win the id of the window
     * \a left the left strut
     * \a right the right strut
     * \a top the top strut
     * \a bottom the bottom strut
     */
    static void setStrut(WId win, qreal left, qreal right, qreal top, qreal bottom);

    /*!
     * Sets the type of window \a win to \a windowType.
     *
     * \a win the id of the window
     * \a windowType the type of the window (see NET::WindowType)
     *
     * \since 6.0
     */
    static void setType(WId win, NET::WindowType windowType);

    /*!
     * Clears the state of window \a win from \a state.
     *
     * Possible values are or'ed combinations of NET::Modal,
     * NET::Sticky, NET::MaxVert, NET::MaxHoriz, NET::Shaded,
     * NET::SkipTaskbar, NET::SkipPager, NET::Hidden,
     * NET::FullScreen, NET::KeepAbove, NET::KeepBelow,
     * NET::SkipSwitcher
     *
     * \a win the id of the window
     * \a state the flags that will be cleared
     *
     * \since 6.0
     */
    static void clearState(WId win, NET::States state);

    /*!
     * Sets the state of window \a win to \a state.
     *
     * Possible values are or'ed combinations of NET::Modal,
     * NET::Sticky, NET::MaxVert, NET::MaxHoriz, NET::Shaded,
     * NET::SkipTaskbar, NET::SkipPager, NET::Hidden,
     * NET::FullScreen, NET::KeepAbove, NET::KeepBelow,
     * NET::SkipSwitcher
     *
     * \a win the id of the window
     * \a state the new flags that will be set
     *
     * \since 6.0
     */
    static void setState(WId win, NET::States state);

Q_SIGNALS:

    /*!
     * Switched to another virtual desktop.
     * \a desktop the number of the new desktop
     */
    void currentDesktopChanged(int desktop);

    /*!
     * A window has been added.
     * \a id the id of the window
     */
    void windowAdded(WId id);

    /*!
     * A window has been removed.
     * \a id the id of the window that has been removed
     */
    void windowRemoved(WId id);

    /*!
     * Hint that \<Window> is active (= has focus) now.
     * \a id the id of the window that is active
     */
    void activeWindowChanged(WId id);

    /*!
     * Desktops have been renamed.
     */
    void desktopNamesChanged();

    /*!
     * The number of desktops changed.
     * \a num the new number of desktops
     */
    void numberOfDesktopsChanged(int num);

    /*!
     * The workarea has changed.
     */
    void workAreaChanged();

    /*!
     * Something changed with the struts, may or may not have changed
     * the work area. Usually just using the workAreaChanged() signal
     * is sufficient.
     */
    void strutChanged();

    /*!
     * Emitted when the stacking order of the window changed. The new order
     * can be obtained with stackingOrder().
     */
    void stackingOrderChanged();

    /*!
     * The window changed.
     *
     * Carries the NET::Properties and NET::Properties2 that were changed.
     *
     * \a id the id of the window
     * \a properties the properties that were modified
     * \a properties2 the properties2 that were modified
     */
    void windowChanged(WId id, NET::Properties properties, NET::Properties2 properties2);

    /*!
     * Compositing was enabled or disabled.
     *
     * Note that this signal may be emitted before any compositing plugins
     * have been initialized in the window manager.
     *
     * If you need to check if a specific compositing plugin such as the
     * blur effect is enabled, you should track that separately rather
     * than test for it in a slot connected to this signal.
     */
    void compositingChanged(bool enabled);

protected:
    void connectNotify(const QMetaMethod &signal) override;

private:
    friend class KWindowInfo;
    friend class KWindowSystemPrivateX11;
    friend class NETEventFilter;
    friend class MainThreadInstantiator;

    enum FilterInfo {
        INFO_BASIC = 1, // desktop info, not per-window
        INFO_WINDOWS = 2, // also per-window info
    };

    KWINDOWSYSTEM_NO_EXPORT void init(FilterInfo info);
    KWINDOWSYSTEM_NO_EXPORT QPoint desktopToViewport(int desktop, bool absolute);
    KWINDOWSYSTEM_NO_EXPORT int viewportToDesktop(const QPoint &pos);

    KWINDOWSYSTEM_NO_EXPORT QPoint constrainViewportRelativePosition(const QPoint &pos);

    // used in xcb/kwindowsystem.cpp
    static bool showingDesktop();
    static void setShowingDesktop(bool showing);

    /*!
     * \internal
     * Returns mapped virtual desktop for the given window geometry.
     */
    KWINDOWSYSTEM_NO_EXPORT static int viewportWindowToDesktop(const QRect &r);

    KWINDOWSYSTEM_NO_EXPORT NETEventFilter *s_d_func()
    {
        return d.get();
    }
    std::unique_ptr<NETEventFilter> d;
};

#endif
