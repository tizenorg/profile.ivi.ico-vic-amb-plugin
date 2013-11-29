project(ico-vic-amb-plugin)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE, Debug)

include(FindPkgConfig)

set(PROJECT_NAME "ico-vic-amb-plugin")
set(PROJECT_VERSION "0.1.0")

set (DOC_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/doc/automotive-message-broker/${PROJECT_NAME}" CACHE PATH "The directory the docs will be installed to")
set (CONF_INSTALL_DIR "/etc/ambd" CACHE PATH "The directory the config will be installed to")

add_definitions(-DPROJECT_VERSION="${PROJECT_VERSION}")
add_definitions(-DPROJECT_NAME="${PROJECT_NAME}")
add_definitions(-DLATER1024)

set (LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
set (LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/${CMAKE_LIBRARY_ARCHITECTURE}" CACHE STRING "Directory where lib will install")

include (CMakeForceCompiler)
option(ENABLE_ICECC "Enable icecc checking, for distributed compilation" OFF)

if (ENABLE_ICECC)
   find_program(ICECC icecc)
    if (ICECC)
        message(STATUS "icecc will be used for distributed compiling")
        cmake_force_cxx_compiler(${ICECC} icecc)
    else(ICECC)
        message(STATUS "Not using icecc")
    endif(ICECC)
endif(ENABLE_ICECC)

find_package(Boost REQUIRED)

pkg_check_modules(glib REQUIRED glib-2.0 gobject-2.0)
pkg_check_modules(gio REQUIRED gio-2.0)
pkg_check_modules(json REQUIRED json)
pkg_check_modules(ambd REQUIRED automotive-message-broker)
pkg_check_modules(elementary REQUIRED elementary)
pkg_check_modules(ecore REQUIRED ecore)
pkg_check_modules(appcore-efl REQUIRED appcore-efl)
pkg_check_modules(dbus REQUIRED dbus-1)
pkg_check_modules(edbus REQUIRED edbus)

set(include_dirs ${libtool_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${json_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/lib ${ambd_INCLUDE_DIRS})
#set(link_libraries ${ambd_LIBRARIES} ${libtool_LIBRARY} ${json_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR}/lib)
set(link_libraries -lamb ${libtool_LIBRARY} ${json_LIBRARIES} -L${CMAKE_CURRENT_BINARY_DIR}/lib)

set(ARCHIVE_NAME ${CMAKE_PROJECT_NAME}-${PROJECT_VERSION})
add_custom_target(dist COMMAND git archive --prefix=${ARCHIVE_NAME}/ HEAD | bzip2 > ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.bz2 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

set(ico-vic-amb-plugin_docs README)
install (FILES ${ico-vic-amb-plugin_docs} DESTINATION ${DOC_INSTALL_DIR})

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(tool)
