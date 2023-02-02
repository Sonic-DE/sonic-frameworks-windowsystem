remove_definitions(-DQT_NO_KEYWORDS)

set(kwindowsystem_plugin_SRCS
    waylandplugin.cpp
    waylandxdgactivationv1.cpp
)

add_library(wayland MODULE ${kwindowsystem_plugin_SRCS})

qt6_generate_wayland_protocol_client_sources(wayland
    FILES ${WaylandProtocols_DATADIR}/staging/xdg-activation/xdg-activation-v1.xml
)

target_link_libraries(wayland PRIVATE KF6::WindowSystem Wayland::Client Qt::WaylandClientPrivate Qt::GuiPrivate)

install(TARGETS wayland DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf6/waylandextras)
