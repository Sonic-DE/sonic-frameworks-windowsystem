remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)
remove_definitions(-DQT_NO_CAST_TO_ASCII)
add_definitions(-DAUTOTEST_BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}")

include(ECMMarkAsTest)
include(ECMAddTests)

set(ECM_TEST_NAME_PREFIX "kwindowsystem-")

find_package(Qt6 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Test Widgets)

add_executable(kx11extras_nonx11test kx11extras_nonx11test.cpp)
target_link_libraries(kx11extras_nonx11test PRIVATE KF6::WindowSystem Qt6::Test Qt6::Gui)
foreach(mode offscreen minimal cached-x11 core)
    add_test(NAME kwindowsystem-kx11extras-${mode} COMMAND kx11extras_nonx11test)
    set_tests_properties(kwindowsystem-kx11extras-${mode} PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;KX11EXTRAS_TEST_MODE=${mode}")
endforeach()
set_tests_properties(kwindowsystem-kx11extras-minimal PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=minimal;KX11EXTRAS_TEST_MODE=minimal")

macro(KWINDOWSYSTEM_UNIT_TESTS)
   foreach(_testname ${ARGN})
      set(libs KF6::WindowSystem Qt6::Test Qt6::Widgets Qt6::GuiPrivate)
      list(APPEND libs XCB::XCB XCB::KEYSYMS XCB::ICCCM)
      ecm_add_test(${_testname}.cpp LINK_LIBRARIES ${libs} GUI)
   endforeach(_testname)
endmacro(KWINDOWSYSTEM_UNIT_TESTS)
macro(KWINDOWSYSTEM_EXECUTABLE_TESTS)
   foreach(_testname ${ARGN})
      add_executable(${_testname} ${_testname}.cpp)
      target_link_libraries(${_testname} KF6::WindowSystem Qt6::Test XCB::XCB Qt6::GuiPrivate)
      ecm_mark_as_test(${_testname})
   endforeach()
endmacro()

include_directories(${CMAKE_SOURCE_DIR}/src/platforms/xcb)
kwindowsystem_unit_tests(
        kmanagerselectiontest
        kstartupinfo_unittest
        kxmessages_unittest
        kkeyserver_x11_unittest
)

kwindowsystem_unit_tests(
        kwindoweffectstest
        kwindowinfox11test
        kwindowsystemx11test
        kwindowsystem_threadtest
        netrootinfotestwm
        netwininfotestclient
        netwininfotestwm
        compositingenabled_test
)

kwindowsystem_executable_tests(
        fixx11h_test
        fixx11h_test2
        dontcrashmapviewport
)
