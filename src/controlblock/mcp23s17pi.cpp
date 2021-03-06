#include "mcp23s17pi.h"

#include <iostream>
#include <stdio.h>

MCP23S17PI::MCP23S17PI(ChipSelectPin chipSelectPin, uint8_t deviceID) :
                _deviceID(deviceID),
                _GPIOA(0),
                _GPIOB(0),
                _IODIRA(0),
                _IODIRB(0),
                _GPPUA(0),
                _GPPUB(0)
{
    if (chipSelectPin == CHIPSELECT_0)
    {
        _chipSelectPin = BCM2835_SPI_CS0;
    }
    else
    {
        _chipSelectPin = BCM2835_SPI_CS1;
    }
}

bool MCP23S17PI::isBCM2835Initialized = false;

void MCP23S17PI::begin()
{
    if (!isBCM2835Initialized)
    {
        if (!bcm2835_init())
        {
            printf("Error initializing GPIO.");
            throw 1;
        }

        bcm2835_spi_begin();

        bcm2835_spi_setClockDivider (BCM2835_SPI_CLOCK_DIVIDER_32);    // 3.9 MHz
        bcm2835_spi_chipSelect(_chipSelectPin);                      // The default

        writeRegister(MCP23S17PI_IOCON, IOCON_INIT);

        isBCM2835Initialized = true;
    }
}

void MCP23S17PI::setPinMode(uint8_t pin, Direction dir)
{
    if (pin < 16)
    {
        uint8_t reg;
        uint8_t* data;

        if (pin < 8)
        {
            reg = MCP23S17PI_IODIRA;
            data = &_IODIRA;
        }
        else
        {
            reg = MCP23S17PI_IODIRB;
            pin &= 0x07;
            data = &_IODIRB;
        }

        if (DIR_INPUT == dir)
        {
            *data |= (1 << pin);
        }
        else
        {
            *data &= (~(1 << pin));
        }
        writeRegister(reg, *data);
    }
}

void MCP23S17PI::setPullupMode(uint8_t pin, Pullup mode)
{
    if (pin < 16)
    {
        uint8_t reg;
        uint8_t* data;

        if (pin < 8)
        {
            reg = MCP23S17PI_GPPUA;
            data = &_GPPUA;
        }
        else
        {
            reg = MCP23S17PI_GPPUB;
            pin &= 0x07;
            data = &_GPPUB;
        }

        if (PULLUP_ENABLED == mode)
        {
            *data |= (1 << pin);
        }
        else
        {
            *data &= (~(1 << pin));
        }
        writeRegister(reg, *data);
    }
}

MCP23S17PI::Level MCP23S17PI::digitalRead(uint8_t pin)
{
    if (pin < 8)
    {
        _GPIOA = readRegister(MCP23S17PI_GPIOA);
        if ((_GPIOA & (1 << pin)) != 0)
        {
            return LEVEL_HIGH;
        }
        else
        {
            return LEVEL_LOW;
        }
    }
    else if (pin < 16)
    {
        _GPIOB = readRegister(MCP23S17PI_GPIOB);
        pin &= 0x07;
        if ((_GPIOB & (1 << pin)) != 0)
        {
            return LEVEL_HIGH;
        }
        else
        {
            return LEVEL_LOW;
        }
    }
    else
    {
        std::cout << "Error while MCP23S17PI::digitalRead call. pin=" << static_cast<int>(pin) << std::endl;
        throw 11;
    }
}

void MCP23S17PI::digitalWrite(uint8_t pin, Level level)
{
    if (pin < 16)
    {
        uint8_t reg;
        uint8_t* data;

        if (pin < 8)
        {
            reg = MCP23S17PI_GPIOA;
            data = &_GPIOA;
        }
        else
        {
            reg = MCP23S17PI_GPIOB;
            pin &= 0x07;
            data = &_GPIOB;
        }

        if (LEVEL_HIGH == level)
        {
            *data |= 1 << pin;
        }
        else
        {
            *data &= ~(1 << pin);
        }
        writeRegister(reg, *data);
    }
}

void MCP23S17PI::writeGPIO(uint16_t data)
{
    writeRegisterWord(MCP23S17PI_GPIOA, data);
}

uint16_t MCP23S17PI::readGPIO()
{
    return readRegisterWord(MCP23S17PI_GPIOA);
}

void MCP23S17PI::writeRegister(uint8_t regAddress, uint8_t data)
{
    char spiData[3];
    spiData[0] = MCP23S08_CMD_WRITE | ((_deviceID) << 1u);
    spiData[1] = regAddress;
    spiData[2] = data;
    bcm2835_spi_transfern(spiData, 3);
}

void MCP23S17PI::writeRegisterWord(const uint8_t& regAddress, uint16_t& data)
{
    writeRegister(regAddress, static_cast<uint8_t>(data));
    writeRegister(regAddress + 1u, static_cast<uint8_t>(data >> 8u));
}

uint8_t MCP23S17PI::readRegister(uint8_t regAddress)
{
    char spiData[3];
    spiData[0] = MCP23S08_CMD_READ | ((_deviceID) << 1u);
    spiData[1] = regAddress;
    bcm2835_spi_transfern(spiData, 3);
    return spiData[2];
}

uint16_t MCP23S17PI::readRegisterWord(uint8_t regAddress)
{
    char buffer[2];

    buffer[0] = readRegister(regAddress);
    buffer[1] = readRegister(regAddress + 1);

    return (uint16_t) (((uint16_t) (buffer[1]) << 8) | (uint16_t) buffer[0]);
}

void MCP23S17PI::end()
{
    bcm2835_spi_end();
}
