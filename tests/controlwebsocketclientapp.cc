/**
 * Copyright (C) 2012  TOYOTA MOTOR CORPORATION.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */
#include <string.h>

#include <iostream>

#include "debugout.h"

#include "config.h"
#include "controlwebsocketclientapp.h"
#include "messageformat.h"

pthread_mutex_t ControlWebsocketClientApp::mutex_scenario =
        PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ControlWebsocketClientApp::cond_scenario =
        PTHREAD_COND_INITIALIZER;
std::string ControlWebsocketClientApp::vehiclename_scenario = "";

ControlWebsocketClientApp::ControlWebsocketClientApp()
{
    mutex = PTHREAD_MUTEX_INITIALIZER;
    protocollist[1] = {NULL, NULL, 0};
}

ControlWebsocketClientApp::~ControlWebsocketClientApp()
{
}

bool
ControlWebsocketClientApp::initialize(int port)
{
    DebugOut(10) << "ControlWebsocketClientApp initialize.(" << port << ")\n";
    protocollist[0] = {"http-only", ControlWebsocketClientApp::callback_receive, 0};

    context = libwebsocket_create_context(CONTEXT_PORT_NO_LISTEN, "lo",
                                          protocollist,
                                          libwebsocket_internal_extensions,
                                          NULL, NULL, -1, -1, 0);
    if (context == NULL) {
        return false;
    }
    socket = libwebsocket_client_connect(context, "127.0.0.1", port, 0, "/",
                                         "localhost", "websocket",
                                         protocollist[0].name, -1);
    if (socket == NULL) {
        return false;
    }
    if (pthread_create(&threadid, NULL, ControlWebsocketClientApp::run,
                       (void*)this) == -1) {
        libwebsocket_context_destroy(context);
        return false;
    }
    return true;
}

bool
ControlWebsocketClientApp::send(std::string type, std::string name,
                                std::string propertyname, std::string value,
                                double timestamp)
{
    pthread_mutex_lock(&mutex);
    memset(buf, 0, sizeof(buf));
    JsonData jdata;
    jdata.propertyName = propertyname;
    jdata.value = value;
    jdata.timestamp = timestamp;
    vector<JsonData> dataarray;
    dataarray.push_back(jdata);
    memcpy(buf + LWS_SEND_BUFFER_PRE_PADDING,
           jsonmsg.encode(type, name, "transactionid", dataarray),
           JsonMessage::BUFSIZE);
    int i = 0;
    while (buf[(i++) + LWS_SEND_BUFFER_PRE_PADDING] != '\0') ;
    int ret = libwebsocket_write(
            socket,
            reinterpret_cast<unsigned char*>(buf + LWS_SEND_BUFFER_PRE_PADDING),
            i, LWS_WRITE_TEXT);
    DebugOut(10) << "libwebsocket_write return " << ret << "\n";
    pthread_mutex_unlock(&mutex);
    return true;
}

bool
ControlWebsocketClientApp::receive(char *keyeventtype, timeval recordtime,
                                   void *data, size_t len)
{
    return true;
}

void
ControlWebsocketClientApp::observation()
{
    int ret = 0;
    while (ret >= 0) {
        ret = libwebsocket_service(context, 100);
        if (ret != 0) {
            break;
        }
    }
}

int
ControlWebsocketClientApp::callback_receive(
        libwebsocket_context *context, libwebsocket *wsi,
        libwebsocket_callback_reasons reason, void *user, void *in, size_t len)
{
    switch (reason) {
    case LWS_CALLBACK_CLIENT_RECEIVE:
    {
        JsonMessage jmsg;
        jmsg.decode(std::string(reinterpret_cast<char*>(in)), len);
        std::vector<JsonData> jdataarray;
        jdataarray = jmsg.getData();
        for (auto itr = jdataarray.begin(); itr != jdataarray.end(); itr++) {
            DebugOut(10) << "[R]: " << (*itr).propertyName << " , "
                         << (*itr).timestamp << " , " << (*itr).value << std::endl;
            if (((*itr).propertyName
                == ControlWebsocketClientApp::vehiclename_scenario)
                || (jmsg.getName()
                == ControlWebsocketClientApp::vehiclename_scenario)) {
                pthread_cond_signal(&ControlWebsocketClientApp::cond_scenario);
            }
        }
        break;
    }
    default:
    {
        break;
    }
    }
    return 0;
}

void *
ControlWebsocketClientApp::run(void *arg)
{
    ControlWebsocketClientApp *control =
            reinterpret_cast<ControlWebsocketClientApp*>(arg);
    control->observation();
    return NULL;
}
