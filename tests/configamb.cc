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

#ifdef JSONC
#include <json/json.h>
#else
#include <json-glib/json-glib.h>

#include "nullptr.h"
#endif

#include "debugout.h"

#include "configamb.h"

using std::string;
using std::vector;

ConfigAMB::ConfigAMB() : port(23000)
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
    JsonParser* parser = json_parser_new();
    GError* error = nullptr;
    if (!json_parser_load_from_data(parser, config.c_str(), config.length(),
                                    &error)) {
        DebugOut() << "Failed to load config: " << error->message;
        return ret;
    }

    JsonNode* node = json_parser_get_root(parser);

    if (node == nullptr) {
        DebugOut() << "Unable to get JSON root object";
        return ret;
    }

    JsonReader* reader = json_reader_new(node);

    if (reader == nullptr) {
        DebugOut() << "Unable to create JSON reader";
        return ret;
    }

    DebugOut(10) << "Config members: " << json_reader_count_members(reader)
                 << endl;

    json_reader_read_member(reader, "sources");

    const GError * srcReadError = json_reader_get_error(reader);

    if (srcReadError != nullptr) {
        DebugOut() << "Error getting sources member: " << srcReadError->message
                   << endl;
        return ret;
    }

    g_assert(json_reader_is_array(reader));

    std::string name = "";
    for (int i = 0; i < json_reader_count_elements(reader); i++) {
        json_reader_read_element(reader, i);

        json_reader_read_member(reader, "name");
        name = std::string(json_reader_get_string_value(reader));
        json_reader_end_member(reader);

        if (name != "VehicleSource") {
            continue;
        }
        json_reader_read_member(reader, "configfile");
        ambformatpath = std::string(json_reader_get_string_value(reader));
        json_reader_end_member(reader);
        json_reader_end_element(reader);
        ret = true;
    }

    json_reader_end_member(reader);
    if (!ret) {
        std::cerr << "Can't find AMBformat path.\n";
        return ret;
    }

    ///read the sinks:

    json_reader_read_member(reader, "sinks");

    for (int i = 0; i < json_reader_count_elements(reader); i++) {
        json_reader_read_element(reader, i);

        json_reader_read_member(reader, "name");
        name = std::string(json_reader_get_string_value(reader));
        json_reader_end_member(reader);

        if (name != "WebsocketSink") {
            continue;
        }

        if (json_reader_read_member(reader, "port")) {
            port = json_reader_get_int_value(reader);
            json_reader_end_member(reader);
        }
        else {
            port = 23000;
        }

        json_reader_end_element(reader);
    }

    json_reader_end_member(reader);

    ///TODO: this will probably explode:

    if (error) {
        g_error_free(error);
    }

    g_object_unref(reader);
    g_object_unref(parser);
    return ret;
}
