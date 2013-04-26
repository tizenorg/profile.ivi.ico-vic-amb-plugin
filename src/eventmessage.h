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
#ifndef EVENTMESSAGE_H_
#define EVENTMESSAGE_H_
#include "messageformat.h"

class StandardMessage;

/**
 * This class handled the event message.
 */
class EventMessage : public StandardMessage {
public:
    /**
     * Constructor.
     */
    EventMessage();
    /**
     * Destructor.
     */
    ~EventMessage();
    /**
     * This function will attempt to decode the message.
     *
     * @param msg Message to be deceded.
     * @param size The size of the message.
     */
    void
    decode(char *msg, int size);
    /**
     * This function decodes the data portion of the event message.
     *
     * @param keyeventtype The name of vehicle information of MW.
     * @param time The recordtime of vehicle information of MW.
     * @param opt The data portion of the event message.
     */
    void
    decodeOpt(char *keyeventype, timeval time, void *opt);
    /**
     * This function will attempt to encode the message.
     *
     * @param name The name of vehicle information of MW.
     * @param time The recordtime of vehicle information of MW.
     * @param opt The data portion of the event message.
     * @return Binary data.
     */
    char *
    encode(char *name, timeval time, EventOpt opt);
    /**
     * Get the data portion of the event message.
     *
     * @return The data portion of the event message.
     */
    EventOpt
    getEventOpt();

public:
    EventOpt eventopt;
};
#endif // #ifndef EVENTMESSAGE_H_
