#include <string.h>

#include <iostream>

#include <json/json.h>

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
    //std::cout << "JsonMessage Encode Messgae is : " << sstr.str() << "[" << sstr.str().length() << "]" << std::endl;
    memset(encodebuf, 0, sizeof(encodebuf));
    memcpy(encodebuf, const_cast<char*>(sstr.str().c_str()), sizeof(encodebuf));
    return &encodebuf[0];
}

void
JsonMessage::decode(std::string msg, size_t len)
{
    data.clear();
    //std::cout << "Decode message is " << msg << std::endl;

    json_object *rootobject;
    json_tokener *tokener = json_tokener_new();
    enum json_tokener_error err;
    do {
        rootobject = json_tokener_parse_ex(tokener, msg.c_str(), msg.length());
    } while ((err = json_tokener_get_error(tokener)) == json_tokener_continue);

    if (err != json_tokener_success) {
        std::cerr << "Error: " << json_tokener_error_desc(err) << "\n";
        return;
    }
    json_object *object = json_object_object_get(rootobject, "type");
    if (!object) {
        std::cerr << "Error Get type\n";
        return;
    }
    type = std::string(json_object_get_string(object));

    object = json_object_object_get(rootobject, "name");
    if (!object) {
        std::cerr << "Error Get name\n";
        return;
    }
    name = std::string(json_object_get_string(object));

    object = json_object_object_get(rootobject, "transactionid");
    if (!object) {
        std::cerr << "Error Get transactionid\n";
        return;
    }
    transactionid = std::string(json_object_get_string(object));

    object = json_object_object_get(rootobject, "data");
    if (json_object_get_type(object) == json_type_array) {
        array_list *datalist = json_object_get_array(object);
        for (int i = 0; i < array_list_length(datalist); i++) {
            JsonData jdata;
            json_object *dataarrayobject =
                    reinterpret_cast<json_object*>(array_list_get_idx(datalist,
                                                                      i));
            if (json_object_get_type(dataarrayobject) == json_type_object) {
                json_object *dataobject = json_object_object_get(
                        dataarrayobject, "property");
                if (dataobject) {
                    jdata.propertyName = std::string(
                            json_object_get_string(dataobject));
                }

                dataobject = json_object_object_get(dataarrayobject, "value");
                if (!dataobject) {
                    std::cerr << "Error Get value[" << i << "]\n";
                    return;
                }
                jdata.value = std::string(json_object_get_string(dataobject));

                dataobject = json_object_object_get(dataarrayobject,
                                                    "timestamp");
                if (!dataobject) {
                    std::cerr << "Error Get timestamp[" << i << "]\n";
                    return;
                }
                jdata.timestamp = json_object_get_double(dataobject);

                dataobject = json_object_object_get(dataarrayobject,
                                                    "sequence");
                if (!dataobject) {
                    std::cerr << "Error Get sequence[" << i << "]\n";
                    return;
                }
                jdata.sequence = json_object_get_int(dataobject);
                data.push_back(jdata);
            }
            else if (json_object_get_type(dataarrayobject)
                    == json_type_string) {
                jdata.propertyName = std::string(
                        json_object_get_string(dataarrayobject));
                data.push_back(jdata);
            }
        }
    }
    else {
        JsonData jdata;
        if (json_object_get_type(object) == json_type_object) {
            json_object *dataobject;
            if (type == "valuechanged") {
                jdata.propertyName = name;
            }
            else {
                dataobject = json_object_object_get(object, "property");
                if (dataobject) {
                    jdata.propertyName = std::string(
                            json_object_get_string(dataobject));
                }
            }

            dataobject = json_object_object_get(object, "value");
            if (!dataobject) {
                std::cerr << "Error Get value\n";
                return;
            }
            jdata.value = std::string(json_object_get_string(dataobject));

            dataobject = json_object_object_get(object, "timestamp");
            if (!dataobject) {
                std::cerr << "Error Get timestamp\n";
                return;
            }
            jdata.timestamp = json_object_get_double(dataobject);

            dataobject = json_object_object_get(object, "sequence");
            if (!dataobject) {
                std::cerr << "Error Get sequence\n";
                return;
            }
            jdata.sequence = json_object_get_int(dataobject);
        }
        else if (json_object_get_type(object) == json_type_string) {
            jdata.propertyName = std::string(json_object_get_string(object));
        }
        data.push_back(jdata);
    }
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
