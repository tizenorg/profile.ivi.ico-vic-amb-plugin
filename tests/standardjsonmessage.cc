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

#include <iostream>

#include <json-glib/json-glib.h>

#include "nullptr.h"

#include "standardjsonmessage.h"

JsonMessage::JsonMessage()
{
}

JsonMessage::~JsonMessage()
{
    data.clear();
}

char *
JsonMessage::encode(std::string type_, std::string name_,
                    std::string transactionid_, std::vector<JsonData>& data_)
{
    sstr.str("");
    sstr << "{\"type\":\"" << type_ << "\", \"name\":\"" << name_
         << "\", \"data\":[";
    for (auto itr = data_.begin(); itr != data_.end(); itr++) {
        if (itr != data_.begin()) {
            sstr << ",";
        }
        if (name_ != "set") {
            sstr << "\"" << (*itr).propertyName << "\"";
        }
        else {
            sstr << "{\"property\":\"" << (*itr).propertyName
                    << "\", \"value\":\"" << (*itr).value << "\"}";
        }
    }
    sstr << "],\"transactionid\":\"" << transactionid_ << "\"}";
    memset(encodebuf, 0, sizeof(encodebuf));
    memcpy(encodebuf, const_cast<char*>(sstr.str().c_str()), sizeof(encodebuf));
    return &encodebuf[0];
}

void
JsonMessage::decode(std::string msg, size_t len)
{
    data.clear();

    JsonParser* parser = json_parser_new();
    GError* error = nullptr;
    if (!json_parser_load_from_data(parser, msg.c_str(), len, &error)) {
        std::cerr << "Failed to load config: " << error->message << '\n';
        return;
    }

    JsonNode* node = json_parser_get_root(parser);

    if (node == nullptr) {
        std::cerr << "Unable to get JSON root object\n";
        return;
    }

    JsonReader* reader = json_reader_new(node);

    if (reader == nullptr) {
        std::cerr << "Unable to create JSON reader\n";
        return;
    }

    if (json_reader_read_member(reader, "type")) {
        type = std::string(json_reader_get_string_value(reader));
        json_reader_end_member(reader);
    }
    else {
        std::cerr << "Error Get type\n";
        return;
    }

    if (json_reader_read_member(reader, "name")) {
        name = std::string(json_reader_get_string_value(reader));
        json_reader_end_member(reader);
    }
    else {
        std::cerr << "Error Get name\n";
        return;
    }

    if (json_reader_read_member(reader, "transactionid")) {
        transactionid = std::string(json_reader_get_string_value(reader));
        json_reader_end_member(reader);
    }
    else {
        std::cerr << "Error Get transactionid\n";
        return;
    }

    if (json_reader_read_member(reader, "data")) {
        if (json_reader_is_array(reader)) {
            JsonData jdata;
            for (int i = 0; i < json_reader_count_elements(reader); i++) {
                json_reader_read_element(reader, i);
                if (json_reader_is_object(reader)) {
                    if (json_reader_read_member(reader, "property")) {
                        jdata.propertyName = std::string(
                                json_reader_get_string_value(reader));
                        json_reader_end_member(reader);
                    }
                    if (json_reader_read_member(reader, "value")) {
                        jdata.value = std::string(
                                json_reader_get_string_value(reader));
                        json_reader_end_member(reader);
                    }
                    if (json_reader_read_member(reader, "timestamp")) {
                        jdata.timestamp = json_reader_get_double_value(reader);
                        json_reader_end_member(reader);
                    }
                    if (json_reader_read_member(reader, "sequence")) {
                        jdata.sequence = json_reader_get_int_value(reader);
                        json_reader_end_member(reader);
                    }
                    data.push_back(jdata);
                }
                else if (json_reader_is_value(reader)) {
                    jdata.propertyName = std::string(
                            json_reader_get_string_value(reader));
                    data.push_back(jdata);
                }
                json_reader_end_element(reader);
            }
        }
        else {
            JsonData jdata;
            if (json_reader_is_object(reader)) {
                if (type == "valuechanged") {
                    jdata.propertyName = name;
                }
                else {
                    if (json_reader_read_member(reader, "property")) {
                        jdata.propertyName = std::string(
                                json_reader_get_string_value(reader));
                        json_reader_end_member(reader);
                    }
                }
                if (json_reader_read_member(reader, "value")) {
                    jdata.value = std::string(
                            json_reader_get_string_value(reader));
                    json_reader_end_member(reader);
                }
                else {
                    std::cerr << "Error Get value\n";
                    return;
                }
                if (json_reader_read_member(reader, "timestamp")) {
                    jdata.timestamp = json_reader_get_double_value(reader);
                    json_reader_end_member(reader);
                }
                else {
                    std::cerr << "Error Get timestamp\n";
                    return;
                }
                if (json_reader_read_member(reader, "sequence")) {
                    jdata.sequence = json_reader_get_int_value(reader);
                    json_reader_end_member(reader);
                }
                else {
                    std::cerr << "Error Get sequence\n";
                    return;
                }
            }
            else if (json_reader_is_value(reader)) {
                jdata.propertyName = std::string(
                        json_reader_get_string_value(reader));
            }
            data.push_back(jdata);
        }
    }
    else {
        std::cerr << "Error Get data\n";
        return;
    }
    json_reader_end_member(reader);

    if (error)
        g_error_free(error);

    g_object_unref(reader);
    g_object_unref(parser);
}

std::string
JsonMessage::getType()
{
    return type;
}

std::string
JsonMessage::getName()
{
    return name;
}

std::string
JsonMessage::getTransactionid()
{
    return transactionid;
}

std::vector<JsonData>
JsonMessage::getData()
{
    return data;
}
