include(CheckIncludeFiles)
set(CMAKE_CXX_FLAGS "-g")
include_directories(${CMAKE_SOURCE_DIR}/lib ${include_dirs} ${gio_INCLUDE_DIRS} ${gio-unix_INCLUDE_DIRS} /usr/include/amb /usr/include/dbus-1.0 ../src)

set(testVehiclePluginMW_sources ../src/abstractconfig.cc ../src/ambconfig.cc ../src/standardmessage.cc ../src/eventmessage.cc ../src/datamessage.cc controlwebsocketclient.cc scenarioengine.cc mwscenario.cc)
add_executable(testVehiclePluginMW ${testVehiclePluginMW_sources})
target_link_libraries(testVehiclePluginMW amb pthread websockets -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${gio_LIBRARIES} ${gio-unix_LIBRARIES} -lico-util-com)

set(testVehiclePluginWSApp_sources ../src/abstractconfig.cc ../src/ambconfig.cc ../src/standardmessage.cc ../src/eventmessage.cc ../src/datamessage.cc configamb.cc standardjsonmessage.cc controlwebsocketclient.cc controlwebsocketclientapp.cc scenarioengine.cc websocketscenario.cc)
add_executable(testVehiclePluginWSApp ${testVehiclePluginWSApp_sources})
target_link_libraries(testVehiclePluginWSApp amb pthread websockets -L${CMAKE_CURRENT_BINARY_DIR}/lib ${link_libraries} ${gio_LIBRARIES} ${gio-unix_LIBRARIES} -lico-util-com)
