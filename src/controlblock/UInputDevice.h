#ifndef UINTPUTDEVICE_H
#define UINTPUTDEVICE_H

#include <iostream>
#include <stdint.h>

extern "C"
{
#include <linux/input.h>
#include <linux/uinput.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
}

class UInputDevice
{
public:
    /**
     * Constructor
     */
    UInputDevice();

    /**
     * Destructor
     */
    ~UInputDevice();

    /**
     * Sets the state of a key. To let the changes have an effect the method \ref sync() needs to be called.
     * @param keycode - The keycode
     * @param keyvalue
     * @param evtype
     */
    void setKeyState(uint16_t keycode, int16_t keyvalue, uint16_t evtype);

    /**
     * Sends all recent key state changes via \ref setKeyState() to the uinput device.
     */
    void sync();

protected:
    uint32_t m_fileDescriptor;
    int32_t getHandle();
    void releaseHandle();
};

#endif
