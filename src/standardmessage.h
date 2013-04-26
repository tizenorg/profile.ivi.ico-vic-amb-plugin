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
#ifndef STANDARDMESSGAE_H_
#define STANDARDMESSGAE_H_

#include <sys/time.h>

/**
 * This class handles the common parts of the message.
 */
class StandardMessage {
public:
    /**
     * Constructor.
     */
    StandardMessage();
    /**
     * Destoructor.
     */
    ~StandardMessage();
    /**
     * This function will attempt to decode the message.
     *
     * @param msg Message to be deceded.
     * @param size the size of the message.
     */
    virtual void
    decode(char *msg, int size);
    /**
     * This function decodes the data portion of the data message.
     *
     * @param keyeventtype The name of vehicle information of MW.
     * @param time The recordtime of vehicle information of MW.
     * @param opt The data portion of the data message.
     */
    char*
    encode(char *name, timeval time);

    /**
     * Get the name of vehicle information of MW.
     *
     * @return The name of vehicle information of MW.
     */
    char *
    getKeyEventType();
    /**
     * Get the recordtime of vehicle information of MW.
     *
     * @return The recordtime of vehicle information of MW.
     */
    timeval
    getRecordtime();

    /**
     * The size of message buffer.
     */
    static const int BUFSIZE = 256;
    /**
     * The size of name of vehicle information of MW.
     */
    static const int KEYEVENTTYPESIZE = 64;

protected:
    char messagebuf[BUFSIZE];
    char KeyEventType[KEYEVENTTYPESIZE];
    timeval recordtime;
};
#endif // #ifndef STANDARDMESSGAE_H_
