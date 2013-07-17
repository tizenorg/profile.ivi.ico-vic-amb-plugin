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
#ifndef ABSTRACTCONFIG_H_
#define ABSTRACTCONFIG_H_
#include <string>

class AbstractConfig {
public:
    /**
     * Constructor.
     */
    AbstractConfig();

    /**
     * Destructor.
     */
    virtual
    ~AbstractConfig();

    /**
     * Read configuration file.
     *
     * @param confpath Path to the configuration file.
     */
    bool
    readConfig(std::string confpath);
protected:
    virtual bool
    parseJson(std::string config) = 0;

    bool readflag;
};
#endif //  ABSTRACTCONFIG_H_

