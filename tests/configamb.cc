#include <string.h>

#include <fstream>
#include <iostream>
#include <stdexcept>

#include <json/json.h>

#include "debugout.h"

#include "configamb.h"

using std::string;
using std::vector;

ConfigAMB::ConfigAMB() :
        port(23000)
{
}

ConfigAMB::~ConfigAMB()
{
}

std::string
ConfigAMB::getAMBformatPath()
{
    return ambformatpath;
}

int
ConfigAMB::getPort()
{
    return port;
}

bool
ConfigAMB::readConfig(std::string confpath)
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

bool
ConfigAMB::parseJson(string config)
{
    bool ret = false;
    json_object *rootobject;
    json_tokener *tokener = json_tokener_new();
    enum json_tokener_error err;
    do {
        rootobject = json_tokener_parse_ex(tokener, config.c_str(),
                                           config.length());
    } while ((err = json_tokener_get_error(tokener)) == json_tokener_continue);

    if (err != json_tokener_success) {
        std::cerr << "Error: " << json_tokener_error_desc(err) << "\n";
        return ret;
    }
    json_object *configobject = json_object_object_get(rootobject, "sources");
    if (!configobject) {
        std::cerr << "Error getting ConfigAMB\n";
        return ret;
    }
    array_list *configlist = json_object_get_array(configobject);
    for (int i = 0; i < array_list_length(configlist); i++) {
        json_object *obj = reinterpret_cast<json_object*>(array_list_get_idx(
                configlist, i));
        json_object *nameobj = json_object_object_get(obj, "name");
        if ("VehicleSource" != string(json_object_get_string(nameobj))) {
            std::cout << "Loop:" << json_object_get_string(nameobj) << "\n";
            continue;
        }
        json_object *conffileobj = json_object_object_get(obj, "configfile");
        if (!conffileobj) {
            std::cerr << "Error: conffileobj = NULL" << std::endl;
            break;
        }
        else {
            ambformatpath = std::string(json_object_get_string(conffileobj));
        }
        ret = true;
        break;
    }
    if (!ret) {
        std::cerr << "Can't find AMBformat path.\n";
        return ret;
    }

    configobject = json_object_object_get(rootobject, "sinks");
    if (!configobject) {
        std::cerr << "Error getting ConfigAMB\n";
        return ret;
    }
    configlist = json_object_get_array(configobject);
    for (int i = 0; i < array_list_length(configlist); i++) {
        json_object *obj = reinterpret_cast<json_object*>(array_list_get_idx(
                configlist, i));
        json_object *nameobj = json_object_object_get(obj, "name");
        if ("WebsocketSink" != string(json_object_get_string(nameobj))) {
            continue;
        }
        json_object *portobj = json_object_object_get(obj, "port");
        if (!portobj) {
            port = 23000;
            std::cout << "Default Port = " << port << std::endl;
        }
        else {
            port = json_object_get_int(portobj);
            std::cout << "Read Port = " << port << std::endl;
        }
        ret = true;
        break;
    }
    return ret;
}
