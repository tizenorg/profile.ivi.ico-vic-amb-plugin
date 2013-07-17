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
#include <unistd.h>

#include <iostream>
#include <sstream>

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
void *ControlWebsocketClient::wsi[4] = {NULL, NULL, NULL, NULL};

ControlWebsocketClient::ControlWebsocketClient()
{
    mutex = PTHREAD_MUTEX_INITIALIZER;
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
    stringstream address, protocol;
    address.str("");
    address << "ws://127.0.0.1:" << port;
    protocol.str("");
    switch (type) {
    case ControlWebsocket::DATA_STANDARD:
    {
        protocol << "standarddatamessage-only";
        break;
    }
    case ControlWebsocket::CONTROL_STANDARD:
    {
        protocol << "standardcontrolmessage-only";
        break;
    }
    case ControlWebsocket::DATA_CUSTOM:
    {
        protocol << "customdatamessage-only";
        break;
    }
    case ControlWebsocket::CONTROL_CUSTOM:
    {
        protocol << "customcontrolmessage-only";
        break;
    }
    default:
    {
        return false;
    }
    }
    context = ico_uws_create_context(address.str().c_str(),
                                     protocol.str().c_str());
    if (context == NULL) {
        return false;
    }
    if (ico_uws_set_event_cb(context, ControlWebsocketClient::callback_receive,
                             ((void *)type)) != 0) {
        DebugOut() << "ControlWebsocket[" << type << "]"
                   << " couldn't set callback function." << std::endl;
        return false;
    }
    /*
     socket = libwebsocket_client_connect(context, "127.0.0.1", port, 0, "/",
     "localhost", "websocket",
     protocollist[0].name, -1);
     if (socket == NULL) {
     return false;
     }
     */
    if (pthread_create(&threadid, NULL, ControlWebsocketClient::run,
                       (void*)this) == -1) {
        ico_uws_close(context);
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
        memcpy(buf,
               datamsg.encode(keyeventtype, time,
                              *(reinterpret_cast<DataOpt*>(data))),
                              len);
        DebugOut(10) << keyeventtype << " encode\n";
        break;
    }
    case ControlWebsocket::CONTROL_STANDARD:
    case ControlWebsocket::CONTROL_CUSTOM:
    {
        memcpy(buf,
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
    ico_uws_send(context, wsi[type], reinterpret_cast<unsigned char*>(buf),
                 len);
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
    while (true) {
        ico_uws_service(context);
        usleep(50);
    }
}

void
ControlWebsocketClient::callback_receive(const struct ico_uws_context *context,
                                         const ico_uws_evt_e event,
                                         const void *id,
                                         const ico_uws_detail *detail,
                                         void *user_data)
{
    switch (event) {
    case ICO_UWS_EVT_OPEN:
    {
        int idx = reinterpret_cast<int>(user_data);
        DebugOut() << "Open. wsi index = " << idx << "\n";
        ControlWebsocketClient::wsi[idx] = const_cast<void*>(id);
        break;
    }
    case ICO_UWS_EVT_ERROR:
        break;
    case ICO_UWS_EVT_CLOSE:
    {
        int idx = reinterpret_cast<int>(user_data);
        ControlWebsocketClient::wsi[idx] = NULL;
        break;
    }
    case ICO_UWS_EVT_RECEIVE:
    {
        DataMessage msg;
        char *recvbuf =
                reinterpret_cast<char*>(detail->_ico_uws_message.recv_data);
        msg.decode(recvbuf, detail->_ico_uws_message.recv_len);
        DebugOut(10) << "[R]: " << msg.getKeyEventType() << " , "
                     << msg.getRecordtime().tv_sec << "."
                     << msg.getRecordtime().tv_usec << " , ";
        if (string(msg.getKeyEventType())
                == ControlWebsocketClient::vehiclename_scenario) {
            pthread_cond_signal(&ControlWebsocketClient::cond_scenario);
        }
        break;
    }
    case ICO_UWS_EVT_ADD_FD:
        break;
    case ICO_UWS_EVT_DEL_FD:
        break;
    default:
        break;
    }
}

void *
ControlWebsocketClient::run(void *arg)
{
    ControlWebsocketClient *control =
            reinterpret_cast<ControlWebsocketClient*>(arg);
    control->observation();
    return NULL;
}
