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

#include <fstream>
#include <iostream>
#include <stdexcept>

#include <json-glib/json-glib.h>

#include "debugout.h"
#include "nullptr.h"

#include "config.h"

using std::string;
using std::vector;

Config::Config()
{
}

Config::~Config()
{
    vehicleinfoList.clear();
}

bool
Config::readConfig(std::string confpath)
{
    std::ifstream in(confpath, std::ios::in);
    std::string output;
    std::string line;
    while (in.good()) {
        getline(in, line);
        output.append(line);
    }
    return parseJson(output);
}

vector<VehicleInfoDefine>
Config::getVehicleInfoConfig()
{
    return vehicleinfoList;
}

PortInfo
Config::getPort()
{
    return portinfo;
}

bool
Config::parseJson(string config)
{
    /* ToDo */
    /* json_reader_read_member's error check. */
    bool ret = false;
    JsonParser *parser = json_parser_new();
    GError *error = nullptr;
    if (!json_parser_load_from_data(parser, config.c_str(), config.length(),
                                    &error)) {
        cerr << "Failed to load config[" << error->message << "]." << endl;
        DebugOut() << "Failed to load config[" << error->message << "]." << endl;
        return ret;
    }

    JsonNode *node = json_parser_get_root(parser);
    if (node == nullptr) {
        cerr << "Unable to get JSON root object." << endl;
        DebugOut() << "Unable to get JSON root object." << endl;
        return ret;
    }

    JsonReader *reader = json_reader_new(node);
    if (reader == nullptr) {
        cerr << "Unable to create JSON reader." << endl;
        DebugOut() << "Unable to create JSON reader." << endl;
        return ret;
    }

    json_reader_read_member(reader, "Config");
    const GError *configReadError = json_reader_get_error(reader);
    if (configReadError != nullptr) {
        cerr << "Error getting sources member[" 
             << configReadError->message << "]." << endl;
        DebugOut() << "Error getting sources member[" 
                   << configReadError->message << "]." << endl;
        return ret;
    }
    g_assert(json_reader_is_array(reader));

    int sectionNum = json_reader_count_elements(reader);
    string section = "";
    for (int i = 0; i < sectionNum; i++) {
        json_reader_read_element(reader, i);
        json_reader_read_member(reader, "Section");
        section = std::string(json_reader_get_string_value(reader));
        json_reader_end_member(reader);

        if (section == "Common") {
            json_reader_read_member(reader, "VehicleInfoDefine");
            int elementNum = json_reader_count_elements(reader);

            bool fcontinue = true;
            int statusNum = 0;
            for (int i = 0; i < elementNum; i++) {
                fcontinue = true;
                VehicleInfoDefine vid;
                json_reader_read_element(reader, i);
                json_reader_read_member(reader, "KeyEventType");
                strcpy(vid.KeyEventType, json_reader_get_string_value(reader));
                json_reader_end_member(reader);

                for (auto itr = vehicleinfoList.begin();
                        itr != vehicleinfoList.end(); itr++) {
                    if (strcmp(vid.KeyEventType, (*itr).KeyEventType) == 0) {
                        json_reader_end_element(reader);
                        fcontinue = false;
                        break;
                    }
                }
                if (!fcontinue) {
                    continue;
                }

                json_reader_read_member(reader, "Priority");
                vid.priority = json_reader_get_int_value(reader);
                json_reader_end_member(reader);

                json_reader_read_member(reader, "DBusInterface");
                vid.dbusInterface = json_reader_get_string_value(reader);
                json_reader_end_member(reader);

                json_reader_read_member(reader, "DBusObject");
                vid.dbusObject = json_reader_get_string_value(reader);
                json_reader_end_member(reader);

                json_reader_read_member(reader, "Status");
                statusNum = json_reader_count_elements(reader);

                for (int j = 0; j < statusNum; j++) {
                    VehicleInfoDefine::Status status;
                    json_reader_read_element(reader, j);
                    json_reader_read_member(reader, "AMBPropertyName");
                    status.ambPropertyName = json_reader_get_string_value(
                            reader);
                    json_reader_end_member(reader);

                    json_reader_read_member(reader, "Type");
                    status.type = getType(
                            const_cast<char*>(json_reader_get_string_value(
                                    reader)),
                            &status.typesize);
                    json_reader_end_member(reader);

                    json_reader_read_member(reader, "Default");
                    status.defaultvalue = std::string(
                            json_reader_get_string_value(reader));
                    json_reader_end_member(reader);

                    json_reader_read_member(reader, "AccessControl");
                    status.accessControl = std::string(
                            json_reader_get_string_value(reader));
                    json_reader_end_member(reader);

                    json_reader_read_member(reader, "DBusProperty");
                    status.dbusPropertyName = std::string(
                            json_reader_get_string_value(reader));
                    json_reader_end_member(reader);

                    json_reader_end_member(reader);
                    json_reader_end_element(reader);

                    json_reader_read_member(reader, "Status");
                    vid.status.push_back(status);
                }
                vehicleinfoList.push_back(vid);

                json_reader_end_member(reader);
                json_reader_end_element(reader);

            }
            json_reader_end_member(reader);
            ret = true;
            json_reader_read_member(reader, "DefaultInfoPort");
            json_reader_read_member(reader, "DataPort");
            portinfo.standard.dataPort = json_reader_get_int_value(reader);
            json_reader_end_member(reader);
            json_reader_read_member(reader, "CtrlPort");
            portinfo.standard.controlPort = json_reader_get_int_value(reader);
            json_reader_end_member(reader);
            json_reader_end_member(reader);
            json_reader_read_member(reader, "CustomizeInfoPort");
            json_reader_read_member(reader, "DataPort");
            portinfo.custom.dataPort = json_reader_get_int_value(reader);
            json_reader_end_member(reader);
            json_reader_read_member(reader, "CtrlPort");
            portinfo.custom.controlPort = json_reader_get_int_value(reader);
            json_reader_end_member(reader);
            json_reader_end_member(reader);
        }

        json_reader_end_element(reader);
    }

    json_reader_end_member(reader);
    g_object_unref(reader);
    g_object_unref(parser);
    return ret;
}

VehicleInfoDefine::Status::DataType
Config::getType(char *type, int *size)
{
    if (strcmp(type, "int") == 0) {
        *size = sizeof(int);
        return VehicleInfoDefine::Status::INT;
    }
    else if (strcmp(type, "double") == 0) {
        *size = sizeof(double);
        return VehicleInfoDefine::Status::DOUBLE;
    }
    else if (strcmp(type, "char") == 0) {
        *size = sizeof(char);
        return VehicleInfoDefine::Status::CHAR;
    }
    else if (strcmp(type, "int16_t") == 0 || strcmp(type, "int16") == 0) {
        *size = sizeof(int16_t);
        return VehicleInfoDefine::Status::INT16;
    }
    else if (strcmp(type, "uint16_t") == 0 || strcmp(type, "uint16") == 0) {
        *size = sizeof(uint16_t);
        return VehicleInfoDefine::Status::UINT16;
    }
    else if (strcmp(type, "uint32_t") == 0 || strcmp(type, "uint32") == 0) {
        *size = sizeof(uint32_t);
        return VehicleInfoDefine::Status::UINT32;
    }
    else if (strcmp(type, "int64_t") == 0 || strcmp(type, "int64") == 0) {
        *size = sizeof(int64_t);
        return VehicleInfoDefine::Status::INT64;
    }
    else if (strcmp(type, "uint64_t") == 0 || strcmp(type, "uint64") == 0) {
        *size = sizeof(uint64_t);
        return VehicleInfoDefine::Status::UINT64;
    }
    else if (strcmp(type, "bool") == 0 || strcmp(type, "boolean") == 0) {
        *size = sizeof(bool);
        return VehicleInfoDefine::Status::BOOL;
    }
    return VehicleInfoDefine::Status::NONE;
}
