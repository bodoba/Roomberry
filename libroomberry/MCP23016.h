/*
 * ---------------------------------------------------------------------------------------
 * Copyright 2014 by Bodo Bauer <bb@bb-zone.com>
 *
 * The I2C Arduino Slave Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The I2C Arduino Slave Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * --------------------------------------------------------------------------------------
 */
#include <stdint.h>


#ifndef MCP23016_H
#define MCP23016_H

#ifdef __linux__
#ifdef __arm__
// assume we are compiling on Raspberry PI
#define TARGET_RPI
#endif
#endif

#ifndef TRUE
#define TRUE	(1==1)
#define FALSE	(0==1)
#endif

#ifndef WORD
#define WORD(a,b) ((b&0x00ff)|((a<< 8)&0xff00))
#endif

#define MCP23016_GPIO0_0 (1<<0)
#define MCP23016_GPIO0_1 (1<<1)
#define MCP23016_GPIO0_2 (1<<2)
#define MCP23016_GPIO0_3 (1<<3)
#define MCP23016_GPIO0_4 (1<<4)
#define MCP23016_GPIO0_5 (1<<5)
#define MCP23016_GPIO0_6 (1<<6)
#define MCP23016_GPIO0_7 (1<<7)

#define MCP23016_GPIO1_0 (1<<8)
#define MCP23016_GPIO1_1 (1<<9)
#define MCP23016_GPIO1_2 (1<<10)
#define MCP23016_GPIO1_3 (1<<11)
#define MCP23016_GPIO1_4 (1<<12)
#define MCP23016_GPIO1_5 (1<<13)
#define MCP23016_GPIO1_6 (1<<14)
#define MCP23016_GPIO1_7 (1<<15)

#define MCP23016_GP0     0x00
#define MCP23016_OLAT0   0x02
#define MCP23016_IPOL0   0x04
#define MCP23016_IODIR0  0x06
#define MCP23016_INTCAP0 0x08
#define MCP23016_IOCON0  0x0A

#define MCP23016_BUFFER_SIZE 16

class MCP23016 {
public:
    
    /** Create cbject to access MCP23016 GPIO via i2c interface
     * 
     * \param adr i2c Address of MCP23016
     * \param dev device to access i2c bus
     */
    MCP23016(uint8_t adr, const char *dev);

    /** Reset MCP23016 chip
     *
     * All device ports are configured as inputs
     */
    void reset(void);
    
    /** Set I/O direction
     *
     * When a bit in these registers is set, the corresponding
     * pin becomes an input. Otherwise, it becomes an output.
     * At Power-on Reset, the device ports are configured as inputs.
     *
     * \param mode 16 bit representing the GPIO ports
     */
    void setMode(uint16_t mode);

    /** Set Output Latch
     *
     * Update  the output latch that controls the output.
     *
     * \param mode 16 bit representing the GPIO ports
     */
    void setLatch(uint16_t mode);
    
    /** Set Input Polarity
     *
     * Configure the polarity of the input port data. 
     * If a bit is set, the corresponding input port
     * data bit polarity will be inverted.
     *
     * \param polarity 16 bit representing the GPIO ports
     */
    void setPolarity(uint16_t polarity);

    /** Set pin High/Low on output ports
     *
     * \param polarity 16 bit representing the GPIO ports
     */
    void setOutput(uint16_t value);

    /** Read pin High/Low status on input ports
     *
     * \param *in 16 bit representing the GPI) ports
     */
    void readInput(uint16_t *in);
    
private:
    uint8_t    address;
    const char *i2cDevice;
    char i2cErrorString[128];
    
    uint16_t errorCount;
    bool     lastStatus;
    
    bool i2cWrite(uint8_t, uint16_t);
    bool i2cRead(uint8_t, uint8_t*);
};
#endif