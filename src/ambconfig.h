/*
 * Copyright (c) 2013 TOYOTA MOTOR CORPORATION.
 *
 * Contact: shibata@mac.tec.toyota.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef AMBCONFIG_H_
#define AMBCONFIG_H_
#include <stdint.h>

#include <vector>

#include "abstractconfig.h"
#include "common.h"

/**
 * Table of vehicle information that is described in the configuration file.
 * 04-18 Add String
 */
struct VehicleInfoDefine {
    struct Status {
        std::string ambPropertyName;
        DataType type;
        int typesize;
        std::string accessControl;
        std::string defaultvalue;
        std::string dbusPropertyName;
    };
    /* ToDo */
    //undefine data structure;
    struct Sense {
        int temp;
    };
    /* ToDo */
    //undefine data structure;
    struct EventMask {
        int temp;
    };

    char KeyEventType[64];
    std::vector<Status> status;
    std::vector<Sense> sense;
    std::vector<EventMask> event_mask;
    int priority;
    std::string dbusInterface;
    std::string dbusObject;
};

/**
 * Information Websocket port.
 */
struct PortInfo {
    struct {
        int dataPort;
        int controlPort;
    } standard;

    struct {
        int dataPort;
        int controlPort;
    } custom;
};

/**
 * Reading VehiclePlugin's config file.
 */
class AMBConfig : public AbstractConfig {
public:
    /**
     * Constructor.
     */
    AMBConfig();

    /**
     * Destructor.
     */
    ~AMBConfig();

    /**
     * This function will return a table of vehicle information read from the configuration file.
     *
     * @return Vehicle information defined by the configuration file.(Failure NULL)
     */
    std::vector<VehicleInfoDefine>
    getVehicleInfoConfig();

    /**
     * This function returns the socket port used to Websocket.
     */
    PortInfo
    getPort();
private:
    bool
    parseJson(std::string config);

    DataType
    getType(char *type, int *size);

    std::vector<VehicleInfoDefine> vehicleinfoList;
    PortInfo portinfo;
};
#endif //  AMBCONFIG_H_
