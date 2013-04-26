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
#ifndef CONFIGAPP_H
#define CONFIGAPP_H

#include <string>
#include <vector>

#include "config.h"

/**
 * This class handles the data in the configuration file.
 */
class ConfigAMB : public Config {
public:
    /**
     * Constructor.
     */
    ConfigAMB();
    /**
     * Destructor.
     */
    ~ConfigAMB();
    bool
    readConfig(std::string confpath);
    int
    getPort();
    std::string
    getAMBformatPath();
private:
    bool
    parseJson(std::string config);
    std::string ambformatpath;
    int port;
};
#endif // #ifndef CONFIGAPP_H
