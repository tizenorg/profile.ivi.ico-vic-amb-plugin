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
#include <sys/time.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <json-glib/json-glib.h>

#include "debugout.h"

#include "configamb.h"
#include "scenarioengine.h"

std::string AMBCONFPATH = "/etc/ambd/config";
const int sleeptime = 100 * 1000;

WebsocketAppScenarioEngine::WebsocketAppScenarioEngine()
{
}

WebsocketAppScenarioEngine::~WebsocketAppScenarioEngine()
{
    namelist.clear();
}

void
WebsocketAppScenarioEngine::start()
{
    int value = 0;
    std::stringstream sstr;
    sstr.str("");
    timeval time;
    double timestamp = 0.0;

    int setnotifycall = namelist.size() - 1;
    DebugOut(10) << "=========" << "WebsocketApp Subscribe" << "========="
                 << std::endl;
    usleep(12 * 1000);
    for (auto itr = namelist.begin(); itr != namelist.end(); itr++) {
        for (auto itr2 = (*itr).second.begin(); itr2 != (*itr).second.end();
             itr2++) {
            usleep(sleeptime);
            gettimeofday(&time, NULL);
            timestamp = time.tv_sec + (time.tv_usec / (1000 * 1000));
            ControlWebsocketClientApp::vehiclename_scenario = (*itr2);
            client.send("method", "subscribe", (*itr2), sstr.str(), timestamp);
            DebugOut(10) << "[S]: " << (*itr).first << "->" << (*itr2)
                         << " , subscribe , " << timestamp << " , " << sstr.str()
                         << std::endl;
        }
    }
    for (int i = 0; i < 2; i++) {
        pthread_mutex_lock(&ControlWebsocketClientApp::mutex_scenario);
        pthread_cond_wait(&ControlWebsocketClientApp::cond_scenario,
                          &ControlWebsocketClientApp::mutex_scenario);
        pthread_mutex_unlock(&ControlWebsocketClientApp::mutex_scenario);
        DebugOut(10) << "WebsocketApp Next." << std::endl;
    }
    DebugOut(10) << "vehiclename_scenario = "
                 << ControlWebsocketClientApp::vehiclename_scenario << std::endl;
    DebugOut(10) << "=========" << "WebsocketApp Set" << "========="
                 << std::endl;

    for (auto itr = namelist.begin(); itr != namelist.end(); itr++) {
        for (auto itr2 = (*itr).second.begin(); itr2 != (*itr).second.end();
             itr2++) {
            usleep(sleeptime);
            gettimeofday(&time, NULL);
            timestamp = time.tv_sec + (time.tv_usec / (1000 * 1000));
            sstr.str("");
            sstr << value;
            DebugOut(10) << "Send Data : " << sstr.str() << "," << timestamp
                         << std::endl;
            client.send("method", "set", (*itr2), sstr.str(), timestamp);
            DebugOut(10) << "[S]: " << (*itr2) << " , set , " << timestamp
                         << " , " << sstr.str() << std::endl;
        }
    }
    pthread_mutex_lock(&ControlWebsocketClientApp::mutex_scenario);
    pthread_cond_wait(&ControlWebsocketClientApp::cond_scenario,
                      &ControlWebsocketClientApp::mutex_scenario);
    pthread_mutex_unlock(&ControlWebsocketClientApp::mutex_scenario);
    DebugOut(10) << "WebsocketApp Next." << std::endl;
    pthread_mutex_lock(&ControlWebsocketClientApp::mutex_scenario);
    pthread_cond_wait(&ControlWebsocketClientApp::cond_scenario,
                      &ControlWebsocketClientApp::mutex_scenario);
    pthread_mutex_unlock(&ControlWebsocketClientApp::mutex_scenario);
    DebugOut(10) << "WebsocketApp Next." << std::endl;
    DebugOut(10) << "=========" << "WebsocketApp Set & Get" << "========="
            << std::endl;
    for (auto itr = namelist.begin(); itr != namelist.end(); itr++) {
        for (auto itr2 = (*itr).second.begin(); itr2 != (*itr).second.end();
             itr2++) {
            usleep(sleeptime);
            gettimeofday(&time, NULL);
            timestamp = time.tv_sec + (time.tv_usec / (1000 * 1000));
            sstr.str("");
            sstr << value;
            DebugOut(10) << "Send Data : " << sstr.str() << "," << timestamp
                         << std::endl;
            client.send("method", "set", (*itr2), sstr.str(), timestamp);
            DebugOut(10) << "[S]: " << (*itr2) << " , set , " << timestamp
                         << " , " << sstr.str() << std::endl;
            usleep(sleeptime);
            gettimeofday(&time, NULL);
            timestamp = time.tv_sec + (time.tv_usec / (1000 * 1000));
            client.send("method", "get", (*itr2), sstr.str(), timestamp);
            DebugOut(10) << "[S]: " << (*itr2) << " , get , " << timestamp
                         << " , " << sstr.str() << std::endl;
        }
    }
    // Wait Get
    pthread_mutex_lock(&ControlWebsocketClientApp::mutex_scenario);
    pthread_cond_wait(&ControlWebsocketClientApp::cond_scenario,
                      &ControlWebsocketClientApp::mutex_scenario);
    pthread_mutex_unlock(&ControlWebsocketClientApp::mutex_scenario);
    std::cerr << "=========" << "WebsocketApp End" << "=========" << std::endl;
}

bool
WebsocketAppScenarioEngine::initialize()
{
    ConfigAMB confamb;
    if (!confamb.readConfig(AMBCONFPATH)) {
        std::cerr << "Error configamb.initialize." << std::endl;
        return false;
    }
    if (!conf.readConfig(confamb.getAMBformatPath())) {
        std::cerr << "Error config.initialize." << std::endl;
        return false;
    }
    if (!client.initialize(confamb.getPort())) {
        std::cerr << "Error controlwebsocketclientapp.initialize("
                  << confamb.getPort() << "." << std::endl;
        return false;
    }

    std::vector<VehicleInfoDefine> vehicleinfoList;
    vehicleinfoList = conf.getVehicleInfoConfig();
    for (auto itr = vehicleinfoList.begin(); itr != vehicleinfoList.end();
         itr++) {
        for (auto itr2 = (*itr).status.begin(); itr2 != (*itr).status.end();
             itr2++) {
            namelist[std::string((*itr).KeyEventType)].push_back(
                    (*itr2).ambPropertyName);
        }
    }
    return true;
}

int
main()
{
    g_type_init();
    DebugOut::setDebugThreshhold(5);
    WebsocketAppScenarioEngine wsengine;
    if (!wsengine.initialize()) {
        std::cerr << "Error websocketengine.initialize." << std::endl;
        return 0;
    }
    wsengine.start();

    return 0;
}
