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
#include "controlwebsocketclient.h"
#include "datamessage.h"
#include "eventmessage.h"
#include "messageformat.h"

pthread_mutex_t ControlWebsocketClient::mutex_scenario =
        PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ControlWebsocketClient::cond_scenario = PTHREAD_COND_INITIALIZER;
std::string ControlWebsocketClient::vehiclename_scenario = "";

ControlWebsocketClient::ControlWebsocketClient()
{
    mutex = PTHREAD_MUTEX_INITIALIZER;
    protocollist[1] = {NULL, NULL, 0};
}

ControlWebsocketClient::~ControlWebsocketClient()
{
}

bool
ControlWebsocketClient::initialize(int port,
                                   enum ControlWebsocket::ServerProtocol stype)
{
    DebugOut(10) << "ControlWebsocketClient initialize.(" << port << ")\n";
    type = stype;
    switch (type) {
    case ControlWebsocket::DATA_STANDARD:
    {
        protocollist[0] = {"standarddatamessage-only", ControlWebsocketClient::callback_receive, 0};
        break;
    }
    case ControlWebsocket::CONTROL_STANDARD : {
        protocollist[0] = {"standardcontrolmessage-only", ControlWebsocketClient::callback_receive, 0};
        break;
    }
    case ControlWebsocket::DATA_CUSTOM : {
        protocollist[0] = {"customdatamessage-only", ControlWebsocketClient::callback_receive, 0};
        break;
    }
    case ControlWebsocket::CONTROL_CUSTOM : {
        protocollist[0] = {"customcontrolmessage-only", ControlWebsocketClient::callback_receive, 0};
        break;
    }
    default : {
        return false;
    }
    }
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
    if (pthread_create(&threadid, NULL, ControlWebsocketClient::run,
                       (void*)this) == -1) {
        libwebsocket_context_destroy(context);
        return false;
    }
    return true;
}

bool
ControlWebsocketClient::send(char *keyeventtype, timeval time, void *data,
                             size_t len)
{
    pthread_mutex_lock(&mutex);
    memset(buf, 0, sizeof(buf));
    switch (type) {
    case ControlWebsocket::DATA_STANDARD:
    case ControlWebsocket::DATA_CUSTOM:
    {
        memcpy(buf + LWS_SEND_BUFFER_PRE_PADDING,
               datamsg.encode(keyeventtype, time,
                              *(reinterpret_cast<DataOpt*>(data))),
               len);
        DebugOut(10) << keyeventtype << " encode\n";
        break;
    }
    case ControlWebsocket::CONTROL_STANDARD:
    case ControlWebsocket::CONTROL_CUSTOM:
    {
        memcpy(buf + LWS_SEND_BUFFER_PRE_PADDING,
               eventmsg.encode(keyeventtype, time,
                               *(reinterpret_cast<EventOpt*>(data))),
               len);
        break;
    }
    default:
    {
        return false;
        break;
    }
    }
    int ret = libwebsocket_write(
            socket,
            reinterpret_cast<unsigned char*>(buf + LWS_SEND_BUFFER_PRE_PADDING),
            len, LWS_WRITE_BINARY);
    DebugOut(10) << "libwebsocket_write return " << ret << "\n";
    pthread_mutex_unlock(&mutex);
    return true;
}

bool
ControlWebsocketClient::receive(char *keyeventtype, timeval recordtime,
                                void *data, size_t len)
{
    datamsg.decodeOpt(keyeventtype, recordtime, data);
    return true;
}

void
ControlWebsocketClient::observation()
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
ControlWebsocketClient::callback_receive(libwebsocket_context *context,
                                         libwebsocket *wsi,
                                         libwebsocket_callback_reasons reason,
                                         void *user, void *in, size_t len)
{
    switch (reason) {
    case LWS_CALLBACK_CLIENT_RECEIVE:
    {
        DataMessage msg;
        msg.decode(reinterpret_cast<char*>(in), len);
        DebugOut(10) << "[R]: " << msg.getKeyEventType() << " , "
                     << msg.getRecordtime().tv_sec << "."
                     << msg.getRecordtime().tv_usec << " , ";
        if (string(msg.getKeyEventType())
                == ControlWebsocketClient::vehiclename_scenario) {
            pthread_cond_signal(&ControlWebsocketClient::cond_scenario);
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
ControlWebsocketClient::run(void *arg)
{
    ControlWebsocketClient *control =
            reinterpret_cast<ControlWebsocketClient*>(arg);
    control->observation();
    return NULL;
}
