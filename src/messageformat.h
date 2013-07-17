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
#ifndef MESSAGEFORMAT_H_
#define MESSAGEFORMAT_H_
#include "standardmessage.h"

/**
 * Message Type.
 */
enum MessageType {
    SET, GET, CALLBACK
};

/**
 * Common_Status(Used in the data message.)
 */
enum CommonStatus {
    UNKNOWN, NOTSUPPORT, SUPPORT
};

/**
 * The size of status(Used in the data message.)
 */
static const int STATUSSIZE = StandardMessage::BUFSIZE - 
                              StandardMessage::KEYEVENTTYPESIZE - 
                              sizeof(timeval) - sizeof(int);

/**
 * The data portion of the data message.
 */
struct DataOpt {
    enum CommonStatus common_status;
    char status[STATUSSIZE];
};

/**
 * The data portion of the event message.
 */
struct EventOpt {
    int common;
    int sense;
    int event_mask;
};
#endif // MESSAGEFORMAT_H_
