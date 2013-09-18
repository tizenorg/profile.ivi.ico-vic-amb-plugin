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
#include <string.h>

#include <fstream>
#include <iostream>
#include <stdexcept>

#include <json/json.h>

#include "ambconfig.h"
#include "debugout.h"

AMBConfig::AMBConfig()
{
}

AMBConfig::~AMBConfig()
{
}

std::vector<VehicleInfoDefine>
AMBConfig::getVehicleInfoConfig()
{
    return vehicleinfoList;
}

PortInfo
AMBConfig::getPort()
{
    return portinfo;
}

bool
AMBConfig::parseJson(std::string config)
{
    bool ret = false;
    json_object *rootobject;
    json_tokener *tokener = json_tokener_new();
    if (tokener == NULL) {
        return ret;
    }
    enum json_tokener_error err;
    do {
        rootobject = json_tokener_parse_ex(tokener, config.c_str(),
                                           config.length());
    } while ((err = json_tokener_get_error(tokener)) == json_tokener_continue);
    json_tokener_free(tokener);

    if (err != json_tokener_success) {
        std::cerr << "Error: " << json_tokener_error_desc(err) << "\n";
        return ret;
    }
    json_object *configobject = json_object_object_get(rootobject, "Config");
    if (!configobject) {
        DebugOut() << "Error getting Config\n";
        return ret;
    }
    array_list *configlist = json_object_get_array(configobject);
    if (configlist == NULL) {
        DebugOut() << "Config is not Array.\n";
        return ret;
    }
    for (int i = 0; i < array_list_length(configlist); i++) {
        json_object *obj = reinterpret_cast<json_object*>(array_list_get_idx(
                configlist, i));
        if (obj == NULL) {
            break;
        }
        json_object *sectionobj = json_object_object_get(obj, "Section");
        if (sectionobj == NULL) {
            break;
        }
        if ("Common" != std::string(json_object_get_string(sectionobj))) {
            continue;
        }
        json_object *defineobj = json_object_object_get(obj,
                                                        "VehicleInfoDefine");
        if (defineobj == NULL) {
            break;
        }
        array_list *definelist = json_object_get_array(defineobj);
        if (definelist == NULL) {
            break;
        }
        for (int j = 0; j < array_list_length(definelist); j++) {
            DebugOut() << "VehicleInfoDefine : " << j << "/"
                       << array_list_length(definelist) << "\n";
            json_object *obj_vi =
                    reinterpret_cast<json_object*>(array_list_get_idx(
                            definelist, j));
            if (obj_vi == NULL) {
                break;
            }
            VehicleInfoDefine vid;
            json_object *keyeventtypeobj = json_object_object_get(
                    obj_vi, "KeyEventType");
            if (keyeventtypeobj == NULL) {
                continue;
            }
            strcpy(vid.KeyEventType, json_object_get_string(keyeventtypeobj));
            json_object *statusobj = json_object_object_get(obj_vi, "Status");
            if (statusobj == NULL) {
                continue;
            }
            array_list *statuslist = json_object_get_array(statusobj);
            if (statuslist == NULL) {
                continue;
            }
            for (int k = 0; k < array_list_length(statuslist); k++) {
                json_object *obj_sts =
                        reinterpret_cast<json_object*>(array_list_get_idx(
                                statuslist, k));
                if (obj_sts == NULL) {
                    break;
                }
                VehicleInfoDefine::Status status;
                json_object *statuschildobj = json_object_object_get(
                        obj_sts, "AMBPropertyName");
                if (statuschildobj == NULL) {
                    continue;
                }
                status.ambPropertyName = string(
                        json_object_get_string(statuschildobj));
                statuschildobj = json_object_object_get(obj_sts, "Type");
                if (statuschildobj == NULL) {
                    continue;
                }
                status.type =
                        getType(const_cast<char*>(json_object_get_string(
                                statuschildobj)),
                                &status.typesize);
                if (status.type == NONE) {
                    continue;
                }
                statuschildobj = json_object_object_get(obj_sts,
                                                        "AccessControl");
                if (statuschildobj != NULL) {
                    status.accessControl = string(
                            json_object_get_string(statuschildobj));
                }
                statuschildobj = json_object_object_get(obj_sts, "Default");
                if (statuschildobj == NULL) {
                    continue;
                }
                status.defaultvalue = string(
                        json_object_to_json_string(statuschildobj));
                statuschildobj = json_object_object_get(obj_sts,
                                                        "DBusProperty");
                if (statuschildobj != NULL) {
                    status.dbusPropertyName = string(
                            json_object_get_string(statuschildobj));
                }
                vid.status.push_back(status);
            }
            /* ToDo */
            /* Sense, EventMask */
            json_object *priorityobj = json_object_object_get(obj_vi,
                                                              "Priority");
            if (priorityobj != NULL) {
                vid.priority = json_object_get_int(priorityobj);
            }
            json_object *dbusifobj = json_object_object_get(obj_vi,
                                                            "DBusInterface");
            if (dbusifobj != NULL) {
                vid.dbusInterface = string(json_object_get_string(dbusifobj));
            }
            json_object *dbusobjobj = json_object_object_get(obj_vi,
                                                             "DBusObject");
            if (dbusobjobj != NULL) {
                vid.dbusObject = string(json_object_get_string(dbusobjobj));
            }
            vehicleinfoList.push_back(vid);
        }
        json_object *defaultportobj = json_object_object_get(obj,
                                                             "DefaultInfoPort");
        if (defaultportobj == NULL) {
            break;
        }
        json_object *portobj = json_object_object_get(defaultportobj,
                                                      "DataPort");
        if (portobj == NULL) {
            break;
        }
        portinfo.standard.dataPort = json_object_get_int(portobj);
        portobj = json_object_object_get(defaultportobj, "CtrlPort");
        if (portobj == NULL) {
            break;
        }
        portinfo.standard.controlPort = json_object_get_int(portobj);
        json_object *customportobj = json_object_object_get(
                obj, "CustomizeInfoPort");
        if (customportobj == NULL) {
            break;
        }
        portobj = json_object_object_get(customportobj, "DataPort");
        if (portobj == NULL) {
            break;
        }
        portinfo.custom.dataPort = json_object_get_int(portobj);
        portobj = json_object_object_get(customportobj, "CtrlPort");
        if (portobj == NULL) {
            break;
        }
        portinfo.custom.controlPort = json_object_get_int(portobj);
        ret = true;
        break;
    }

    return ret;
}

DataType
AMBConfig::getType(char *type, int *size)
{
    if (strcmp(type, "int") == 0) {
        *size = sizeof(int);
        return INT;
    }
    else if (strcmp(type, "double") == 0) {
        *size = sizeof(double);
        return DOUBLE;
    }
    else if (strcmp(type, "char") == 0) {
        *size = sizeof(char);
        return CHAR;
    }
    else if (strcmp(type, "int16_t") == 0 || strcmp(type, "int16") == 0) {
        *size = sizeof(int16_t);
        return INT16;
    }
    else if (strcmp(type, "uint16_t") == 0 || strcmp(type, "uint16") == 0) {
        *size = sizeof(uint16_t);
        return UINT16;
    }
    else if (strcmp(type, "uint32_t") == 0 || strcmp(type, "uint32") == 0) {
        *size = sizeof(uint32_t);
        return UINT32;
    }
    else if (strcmp(type, "int64_t") == 0 || strcmp(type, "int64") == 0) {
        *size = sizeof(int64_t);
        return INT64;
    }
    else if (strcmp(type, "uint64_t") == 0 || strcmp(type, "uint64") == 0) {
        *size = sizeof(uint64_t);
        return UINT64;
    }
    else if (strcmp(type, "bool") == 0 || strcmp(type, "boolean") == 0) {
        *size = sizeof(bool);
        return BOOL;
    }
    *size = 0;
    return NONE;
}