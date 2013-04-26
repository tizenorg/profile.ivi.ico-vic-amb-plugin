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

#include "standardmessage.h"

StandardMessage::StandardMessage()
{
}

StandardMessage::~StandardMessage()
{
}

void
StandardMessage::decode(char *msg, int size)
{
    int restsize = size;
    if (size <= KEYEVENTTYPESIZE) {
        memset(KeyEventType, 0, size);
        memcpy(KeyEventType, msg, size);
        return;
    }
    memcpy(KeyEventType, msg, KEYEVENTTYPESIZE);
    restsize -= KEYEVENTTYPESIZE;
    if (restsize >= static_cast<int>(sizeof(timeval))) {
        memcpy(&recordtime, msg + KEYEVENTTYPESIZE, sizeof(timeval));
    }
}

char *
StandardMessage::encode(char *name, timeval time)
{
    memset(messagebuf, 0, sizeof(messagebuf));
    memcpy(messagebuf, name, KEYEVENTTYPESIZE);
    memcpy(messagebuf + KEYEVENTTYPESIZE, &time, sizeof(time));
    return &messagebuf[0];
}
char *
StandardMessage::getKeyEventType()
{
    return &KeyEventType[0];
}

timeval
StandardMessage::getRecordtime()
{
    return recordtime;
}
