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
#include "controlwebsocketclientapp.h"
#include "messageformat.h"

pthread_mutex_t ControlWebsocketClientApp::mutex_scenario =
        PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ControlWebsocketClientApp::cond_scenario =
        PTHREAD_COND_INITIALIZER;
std::string ControlWebsocketClientApp::vehiclename_scenario = "";
void *ControlWebsocketClientApp::wsiapp = NULL;

ControlWebsocketClientApp::ControlWebsocketClientApp()
{
    mutex = PTHREAD_MUTEX_INITIALIZER;
}

ControlWebsocketClientApp::~ControlWebsocketClientApp()
{
}

bool
ControlWebsocketClientApp::initialize(int port)
{
    DebugOut(10) << "ControlWebsocketClientApp initialize.(" << port << ")\n";
    stringstream address;
    address.str("");
    address << "ws://127.0.0.1:" << port;
    context = ico_uws_create_context(address.str().c_str(), "http-only");
    if (context == NULL) {
        return false;
    }
    if (ico_uws_set_event_cb(context,
                             ControlWebsocketClientApp::callback_receive, NULL)
        != 0) {
        DebugOut() << "ControlWebsocket[" << type << "]"
                   << " couldn't set callback function." << std::endl;
        return false;
    }
    if (pthread_create(&threadid, NULL, ControlWebsocketClientApp::run,
                       (void*)this) == -1) {
        ico_uws_close(context);
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
    memcpy(buf, jsonmsg.encode(type, name, "transactionid", dataarray),
           JsonMessage::BUFSIZE);
    int i = 0;
    while (buf[(i++)] != '\0')
        ;
    ico_uws_send(context, wsiapp, reinterpret_cast<unsigned char*>(buf), i);
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
    while (true) {
        ico_uws_service(context);
        usleep(50);
    }
}

void
ControlWebsocketClientApp::callback_receive(
        const struct ico_uws_context *context, const ico_uws_evt_e event,
        const void *id, const ico_uws_detail *detail, void *user_data)
{
    switch (event) {
    case ICO_UWS_EVT_OPEN:
    {
        ControlWebsocketClientApp::wsiapp = const_cast<void*>(id);
        break;
    }
    case ICO_UWS_EVT_ERROR:
        break;
    case ICO_UWS_EVT_CLOSE:
    {
        ControlWebsocketClientApp::wsiapp = NULL;
        break;
    }
    case ICO_UWS_EVT_RECEIVE:
    {
        JsonMessage jmsg;
        char *recvbuf =
                reinterpret_cast<char*>(detail->_ico_uws_message.recv_data);
        jmsg.decode(recvbuf, detail->_ico_uws_message.recv_len);
        std::vector<JsonData> jdataarray;
        jdataarray = jmsg.getData();
        for (auto itr = jdataarray.begin(); itr != jdataarray.end(); itr++) {
            DebugOut(10) << "[R]: " << (*itr).propertyName << " , "
                         << (*itr).timestamp << " , " << (*itr).value << std::endl;
            if (((*itr).propertyName
                  == ControlWebsocketClientApp::vehiclename_scenario) || 
                (jmsg.getName()
                 == ControlWebsocketClientApp::vehiclename_scenario)) {
                pthread_cond_signal(&ControlWebsocketClientApp::cond_scenario);
            }
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
ControlWebsocketClientApp::run(void *arg)
{
    ControlWebsocketClientApp *control =
            reinterpret_cast<ControlWebsocketClientApp*>(arg);
    control->observation();
    return NULL;
}
