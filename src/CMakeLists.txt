include(CheckIncludeFiles)

include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${gio_INCLUDE_DIRS} ${gio-unix_INCLUDE_DIRS} /usr/include/amb /usr/include/dbus-1.0)
set(CMAKE_CXX_FLAGS "-g -Wall")
set(vehicleplugin_headers config.h convert.h standardmessage.h datamessage.h eventmessage.h messageformat.h controlwebsocket.h mwinterface.h viccommunicator.h ambinterface.h)
set(vehicleplugin_sources config.cc convert.cc standardmessage.cc eventmessage.cc datamessage.cc mwinterface.cc controlwebsocket.cc viccommunicator.cc ambinterface.cc)
add_library(vehicleplugin MODULE ${vehicleplugin_sources})
set_target_properties(vehicleplugin PROPERTIES PREFIX "")
target_link_libraries(vehicleplugin amb websockets -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries})

install(TARGETS vehicleplugin LIBRARY DESTINATION lib/automotive-message-broker)

set(ico-vic-amb-plugin-conf_docs AMBformat.conf)
install (FILES ${ico-vic-amb-plugin-conf_docs} DESTINATION ${CONF_INSTALL_DIR})
