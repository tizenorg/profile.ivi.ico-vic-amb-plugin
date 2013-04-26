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
#include <sys/time.h>

#include <iostream>
#include <list>

#include <json-glib/json-glib.h>

#include "debugout.h"
#include "vehicleproperty.h"

#include "messageformat.h"
#include "scenarioengine.h"

extern std::list<VehicleProperty::Property> VehicleProperty::mCapabilities;
extern VehicleProperty vehiclePropertyConstruct;
const int sleeptime = 25 * 1000;

MWScenarioEngine::MWScenarioEngine()
{
}

MWScenarioEngine::~MWScenarioEngine()
{
    vehicleinfomap.clear();
}

void
MWScenarioEngine::start()
{
    char value = 1;
    char status[STATUSSIZE];
    int idx = 0;
    int server = 0;
    timeval time;
    DataOpt dopt;
    EventOpt eopt;
    int setnotifycall = 0;
    if ((vehicleinfomap.size() % 2) == 0) {
        setnotifycall = 1;
    }
    int waitcount = 0;
    auto vehicleinfomapbeginitr = vehicleinfomap.begin();
    DebugOut(10) << "=========" << "MW Callback" << "=========" << std::endl;
    usleep(12 * 1000);
    // Subscribe
    for (; setnotifycall < vehicleinfomap.size(); setnotifycall += 2) {
        usleep(sleeptime);
        vehicleinfomapbeginitr = vehicleinfomap.begin();
        advance(vehicleinfomapbeginitr, setnotifycall);
        server = ControlWebsocket::CONTROL_STANDARD;
        for (int i = 0; i < vehicleinfomapbeginitr->second.size(); i++) {
            if (vehicleinfomapbeginitr->second[i].isCustom) {
                server = ControlWebsocket::CONTROL_CUSTOM;
            }
        }
        gettimeofday(&time, NULL);
        eopt.common = 50;
        eopt.sense = 0xff;
        eopt.event_mask = 0;
        client[server].send(
                const_cast<char*>(vehicleinfomapbeginitr->first.c_str()), time,
                &eopt, 64 + sizeof(timeval) + sizeof(EventOpt));
        DebugOut(10) << "[S]: " << vehicleinfomapbeginitr->first << " , "
                     << time.tv_sec << "." << time.tv_usec << " , " 
                     << eopt.common << " , " << eopt.sense << " , " 
                     << eopt.event_mask << std::endl;
        ControlWebsocketClient::vehiclename_scenario =
                vehicleinfomapbeginitr->first;
        waitcount = vehicleinfomapbeginitr->second.size();
    }
    DebugOut(10) << "vehiclename_scenario = "
                 << ControlWebsocketClient::vehiclename_scenario << std::endl;
    // Wait Subscribe
    pthread_mutex_lock(&ControlWebsocketClient::mutex_scenario);
    pthread_cond_wait(&ControlWebsocketClient::cond_scenario,
                      &ControlWebsocketClient::mutex_scenario);
    pthread_mutex_unlock(&ControlWebsocketClient::mutex_scenario);
    DebugOut(10) << "MW Next." << std::endl;
    // Get
    DebugOut(10) << "=========" << "MW Get" << "=========" << std::endl;
    for (auto itr = vehicleinfomap.begin(); itr != vehicleinfomap.end();
            itr++) {
        usleep(sleeptime);
        memset(status, 0, sizeof(status));
        idx = 0;
        server = ControlWebsocket::CONTROL_STANDARD;
        for (auto itr2 = (*itr).second.begin(); itr2 != (*itr).second.end();
             itr2++) {
            memcpy(status + idx, &value, sizeof(value));
            if ((*itr2).isCustom) {
                server = ControlWebsocket::CONTROL_CUSTOM;
            }
            idx += (*itr2).typesize;
        }
        gettimeofday(&time, NULL);
        eopt.common = -1;
        eopt.sense = -1;
        eopt.event_mask = 0;
        client[server].send(const_cast<char*>(itr->first.c_str()), time, &eopt,
                            64 + sizeof(timeval) + sizeof(EventOpt));
        DebugOut(10) << "[S]: " << itr->first << " , " << time.tv_sec << "."
                     << time.tv_usec << " , " << eopt.common << " , " 
                     << eopt.sense << " , " << eopt.event_mask << std::endl;
    }
    // Wait Get
    pthread_mutex_lock(&ControlWebsocketClient::mutex_scenario);
    pthread_cond_wait(&ControlWebsocketClient::cond_scenario,
                      &ControlWebsocketClient::mutex_scenario);
    pthread_mutex_unlock(&ControlWebsocketClient::mutex_scenario);
    DebugOut(10) << "MW Next." << std::endl;
    // Error
    DebugOut(10) << "=========" << "MW Error" << "=========" << std::endl;
    memset(status, 0, sizeof(status));
    memcpy(status, &value, sizeof(value));
    for (server = 0; server < 4; server += 2) {
        usleep(sleeptime);
        client[server].send(const_cast<char*>("Detarame"), time, &dopt,
                            64 + sizeof(timeval) + sizeof(int) + sizeof(int));
        DebugOut(10) << "[S]: " << "Detarame" << " , " << time.tv_sec << "."
                     << time.tv_usec << std::endl;

        usleep(sleeptime);
        eopt.common = -1;
        eopt.sense = -1;
        client[server + 1].send(const_cast<char*>("Detarame"), time, &eopt,
                                64 + sizeof(timeval) + sizeof(EventOpt));
        DebugOut(10) << "[S]: " << "Detarame" << " , " << time.tv_sec << "."
                     << time.tv_usec << " , " << eopt.common << " , " 
                     << eopt.sense << " , " << eopt.event_mask << std::endl;

        usleep(sleeptime);
        eopt.common = 50;
        eopt.sense = 0xff;
        client[server + 1].send(const_cast<char*>("Detarame"), time, &eopt,
                                64 + sizeof(timeval) + sizeof(EventOpt));
        DebugOut(10) << "[S]: " << "Detarame" << " , " << time.tv_sec << "."
                     << time.tv_usec << " , " << eopt.common << " , " 
                     << eopt.sense << " , " << eopt.event_mask << std::endl;
    }
    // Set(1st)
    DebugOut(10) << "=========" << "MW Set" << "=========" << std::endl;
    for (auto itr = vehicleinfomap.begin(); itr != vehicleinfomap.end();
         itr++) {
        usleep(sleeptime);
        memset(status, 0, sizeof(status));
        idx = 0;
        server = ControlWebsocket::DATA_STANDARD;
        for (auto itr2 = (*itr).second.begin(); itr2 != (*itr).second.end();
             itr2++) {
            memcpy(status + idx, &value, sizeof(value));
            if ((*itr2).isCustom) {
                server = ControlWebsocket::DATA_CUSTOM;
            }
            idx += (*itr2).typesize;
        }
        gettimeofday(&time, NULL);
        dopt.common_status = SUPPORT;
        memcpy(dopt.status, status, STATUSSIZE);
        client[server].send(const_cast<char*>(itr->first.c_str()), time, &dopt,
                            64 + sizeof(timeval) + sizeof(int) + idx);
        DebugOut(10) << "[S]: " << itr->first << " , " << time.tv_sec << "."
                     << time.tv_usec << " , ";
    }
    value++;
    // Wait valuechanged
    for (int i = 0; i < waitcount; i++) {
        pthread_mutex_lock(&ControlWebsocketClient::mutex_scenario);
        pthread_cond_wait(&ControlWebsocketClient::cond_scenario,
                          &ControlWebsocketClient::mutex_scenario);
        pthread_mutex_unlock(&ControlWebsocketClient::mutex_scenario);
        DebugOut(10) << "MW Next." << std::endl;
    }
    // Set(2nd)
    DebugOut(10) << "=========" << "MW Set2" << "=========" << std::endl;
    for (auto itr = vehicleinfomap.begin(); itr != vehicleinfomap.end();
         itr++) {
        usleep(sleeptime);
        memset(status, 0, sizeof(status));
        idx = 0;
        server = ControlWebsocket::DATA_STANDARD;
        for (auto itr2 = (*itr).second.begin(); itr2 != (*itr).second.end();
             itr2++) {
            memcpy(status + idx, &value, sizeof(value));
            if ((*itr2).isCustom) {
                server = ControlWebsocket::DATA_CUSTOM;
            }
            idx += (*itr2).typesize;
        }
        gettimeofday(&time, NULL);
        dopt.common_status = SUPPORT;
        memcpy(dopt.status, status, STATUSSIZE);
        client[server].send(const_cast<char*>(itr->first.c_str()), time, &dopt,
                            64 + sizeof(timeval) + sizeof(int) + idx);
        DebugOut(10) << "[S]: " << itr->first << " , " << time.tv_sec << "."
                     << time.tv_usec << " , ";
    }
    // Wait valuechanged.
    for (int i = 0; i < waitcount; i++) {
        pthread_mutex_lock(&ControlWebsocketClient::mutex_scenario);
        pthread_cond_wait(&ControlWebsocketClient::cond_scenario,
                          &ControlWebsocketClient::mutex_scenario);
        pthread_mutex_unlock(&ControlWebsocketClient::mutex_scenario);
        DebugOut(10) << "MW Next." << std::endl;
    }
    std::cerr << "=========" << "MW End" << "=========" << std::endl;
}

bool
MWScenarioEngine::initialize()
{
    conf.readConfig(CONFPATH);

    std::vector<VehicleInfoDefine> vehicleinfoList;
    vehicleinfoList = conf.getVehicleInfoConfig();
    AbstractPropertyType *type;
    for (auto itr = vehicleinfoList.begin(); itr != vehicleinfoList.end();
         itr++) {
        for (auto itr2 = (*itr).status.begin(); itr2 != (*itr).status.end();
             itr2++) {
            MWInfo info;
            info.type = (*itr2).type;
            info.typesize = (*itr2).typesize;
            info.isCustom = true;
            type = VehicleProperty::getPropertyTypeForPropertyNameValue(
                    (*itr2).ambPropertyName, "0");
            DebugOut(10) << "Read config file. " << (*itr2).ambPropertyName
                         << " is ";
            if (type != nullptr) {
                info.isCustom = false;
                DebugOut(10) << "Standard." << std::endl;
                delete type;
            }
            else {
                DebugOut(10) << "Custom." << std::endl;
            }
            vehicleinfomap[std::string((*itr).KeyEventType)].push_back(info);
        }
    }
    PortInfo portinfo = conf.getPort();
    if (!client[0].initialize(portinfo.standard.dataPort,
                              ControlWebsocket::DATA_STANDARD)) {
        return false;
    }
    if (!client[1].initialize(portinfo.standard.controlPort,
                              ControlWebsocket::CONTROL_STANDARD)) {
        return false;
    }
    if (!client[2].initialize(portinfo.custom.dataPort,
                              ControlWebsocket::DATA_CUSTOM)) {
        return false;
    }
    if (!client[3].initialize(portinfo.custom.controlPort,
                              ControlWebsocket::CONTROL_CUSTOM)) {
        return false;
    }
    return true;
}

int
main()
{
    g_type_init();
    DebugOut::setDebugThreshhold(5);
    MWScenarioEngine mwengine;
    if (!mwengine.initialize()) {
        std::cerr << "Error dbusengine.initialize." << std::endl;
        return 0;
    }
    mwengine.start();
    return 0;
}
