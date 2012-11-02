project(choqok-fanfou)
cmake_minimum_required(VERSION 2.6)

find_package(KDE4 REQUIRED)
find_package(Choqok REQUIRED)
find_package(QJson REQUIRED)
find_package(QCA2 REQUIRED)
find_package(QtOAuth REQUIRED)

include(KDE4Defaults)
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include_directories(${KDE4_INCLUDES})
include_directories(${CHOQOK_INCLUDE_DIR} ${CHOQOK_INCLUDE_DIR}/choqok ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${QCA2_INCLUDE_DIR} ${QTOAUTH_INCLUDE_DIR})


kde4_add_ui_files(choqok_fanfou_UIS
    fanfoueditaccount_base.ui
)

set(choqok_fanfou_SRCS
    ${choqok_fanfou_UIS}
    fanfouaccount.cpp
    fanfoueditaccount.cpp
    fanfoumicroblog.cpp
    fanfoupostwidget.cpp
    fanfoutimelinewidget.cpp
)

kde4_add_plugin(choqok_fanfou ${choqok_fanfou_SRCS})

target_link_libraries(choqok_fanfou
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
    ${QJSON_LIBRARY}
    ${QTOAUTH_LIBRARY}
    ${CHOQOK_LIBRARIES}
)

# Install

install(TARGETS choqok_fanfou DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES choqok_fanfou.desktop DESTINATION ${SERVICES_INSTALL_DIR})

add_subdirectory(icons)
