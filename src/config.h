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
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>

/**
 * Table of vehicle information that is described in the configuration file.
 * 04-18 Add String
 */
struct VehicleInfoDefine {
    struct Status {
        std::string ambPropertyName;
        enum DataType {
            INT, DOUBLE, CHAR, INT16, UINT16, UINT32, INT64, UINT64, BOOL, NONE
        } type;
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
 * This class handles the data in the configuration file.
 */
class Config {
public:
    /**
     * Constructor.
     */
    Config();
    /**
     * Destructor.
     */
    ~Config();
    /**
     * Read configuration file.
     *
     * @param confpath Path to the configuration file.
     */
    bool
    readConfig(std::string confpath);
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
protected:
    bool
    parseJson(std::string config);

    std::string filename;
    bool readflag;
private:
    VehicleInfoDefine::Status::DataType
    getType(char *type, int *size);

    std::vector<VehicleInfoDefine> vehicleinfoList;
    PortInfo portinfo;
};
#endif // #ifndef CONFIG_H
