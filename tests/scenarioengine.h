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
#include <map>
#include <string>
#include <vector>

#include "abstractpropertytype.h"
#include "ambconfig.h"
#include "controlwebsocketclient.h"
#include "controlwebsocketclientapp.h"

const std::string CONFPATH = "/etc/ambd/AMBformat.conf";

struct MWInfo {
    DataType type;
    int typesize;
    bool isCustom;
};

class ScenarioEngine {
public:
    ScenarioEngine() :
            loadscenario(0)
    {
    }
    ~ScenarioEngine();
    virtual void
    start() = 0;
    virtual bool
    initialize() = 0;
protected:

    AMBConfig conf;
    int loadscenario;
};

class MWScenarioEngine : public ScenarioEngine {
public:
    MWScenarioEngine();
    ~MWScenarioEngine();
    void
    start();
    bool
    initialize();
private:
    bool stopFlg;
    std::map<std::string, std::vector<MWInfo> > vehicleinfomap;
    ControlWebsocketClient client[4];
};

class WebsocketAppScenarioEngine : public ScenarioEngine {
public:
    WebsocketAppScenarioEngine();
    ~WebsocketAppScenarioEngine();
    void
    start();
    bool
    initialize();
private:
    std::map<std::string, std::vector<std::string> > namelist;
    ControlWebsocketClientApp client;
};
