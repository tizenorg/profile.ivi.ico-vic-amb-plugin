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
#include <sstream>
#include <string>
#include <vector>

struct JsonData {
    std::string propertyName;
    std::string value;
    double timestamp;
    int sequence;
};

class JsonMessage {
public:
    JsonMessage();
    ~JsonMessage();
    char *
    encode(std::string type_, std::string name_, std::string transactionid_,
           std::vector<JsonData>& data_);
    void
    decode(std::string msg, size_t len);

    std::string
    getType();
    std::string
    getName();
    std::string
    getTransactionid();
    std::vector<JsonData>
    getData();

    static const int BUFSIZE = 256;
protected:
    std::stringstream sstr;

    std::string type;
    std::string name;
    std::string transactionid;
    std::vector<JsonData> data;
    char encodebuf[BUFSIZE];
};
