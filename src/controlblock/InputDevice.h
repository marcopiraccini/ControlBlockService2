#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include <stdint.h>

class InputDevice
{
public:
    enum Channel_e
    {
        CHANNEL_1 = 0,
        CHANNEL_2,
        CHANNEL_3,
        CHANNEL_4,
        CHANNEL_UNDEFINED
    };

    InputDevice()
    {
    }

    virtual ~InputDevice()
    {
    }

    virtual void initialize(Channel_e channel) = 0;
    virtual void update() = 0;
};

#endif
