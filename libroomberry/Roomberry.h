/*
 * ---------------------------------------------------------------------------------------
 * Copyright 2014 by Bodo Bauer <bb@bb-zone.com>
 *
 * This Roomba Open Interface class is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Roomba Open Interface class is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this file.  If not, see <http://www.gnu.org/licenses/>.
 * --------------------------------------------------------------------------------------
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <stdint.h>

#ifndef ROOMBERRY_H
#define ROOMBERRY_H

#ifndef TRUE
#define TRUE	(1==1)
#define FALSE	(0==1)
#endif

#define ROOMBERRY_BAUDRATE B115200

typedef struct {
    uint8_t   chargingState;
    uint16_t  voltage;
    int16_t   current;
    int8_t    temperature;
    uint16_t  batteryCharge;
    uint16_t  batteryCapacity;
} roomberryBatteryData_t;

typedef struct {
    uint16_t left;
    uint16_t frontLeft;
    uint16_t centerLeft;
    uint16_t centerRight;
    uint16_t frontRight;
    uint16_t right;
} roomberryLightBumperSignals_t;

typedef struct {
    bool left;
    bool frontLeft;
    bool centerLeft;
    bool centerRight;
    bool frontRight;
    bool right;
} roomberryLightBumper_t;

typedef struct {
    bool left;
    bool right;
} roomberryBumper_t;

typedef struct {
    uint8_t note;
    uint8_t duration;
} roomberryNote;

class Roomberry {

public:
    /** Create Roomberry Object
     *
     * Initialize serial connection (115200Baud 8N1)
     * send start command. Leaves Roomba in passive mode.
     *
     * \param tty Device file to connect to Roomba (i.e. "/dev/ttyAMA0")
     */
    Roomberry(const char *tty);
    
    /**  Enter Full Mode
     *
     * This command gives you complete control over Roomba
     * by putting the OI into Full mode, and turning off
     * the cliff, wheel-drop and internal charger safety
     * features. That is, in Full mode, Roomba executes
     * any command that you send it, even if the internal
     * charger is plugged in, or command triggers a cliff
     * or wheel drop condition.
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool fullMode(void);
    
    /**  Enter Safe Mode
     *
     * This command puts the OI into Safe mode, enabling user
     * control of Roomba. It turns off all LEDs. The OI can
     * be in Passive, Safe, or     Full mode to accept this
     * command. If a safety condition occurs Roomba reverts
     * automatically to Passive mode.
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool safeMode(void);
    
    /**  Enter Passive Mode
     *
     * In Passive mode, you can request and receive sensor
     * data using any of the sensor commands, but you cannot
     * change the current command parameters for the actuators
     * (motors, speaker, lights) to something else.
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool passiveMode(void);
    
    /** Print 4 byte string to LEDs
     *
     * This command controls the four 7 segment displays on the Roomba 560
     * and 570 using ASCII character codes. Because a 7 segment display is
     * not sufficient to display alphabetic characters properly, all characters
     * are an approximation, and not all ASCII codes are implemented.
     *
     * \param string A four character string 
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool printLed(const char* string );

    /** Read front bumpers
     *
     * Read right and left bumper state
     * 
     * \param *bumper Pointer to store bumper state
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool getBumper(roomberryBumper_t *bumper);
    
    /** Read light bumpers
     *
     * get state of light bumpers (left/right, front left/right, center left/right)
     *
     *   TRUE  -> Bumper 'pressed' (hit obstacle)
     *
     *   FALSE -> Bumper clear
     *
     * \param *lightBumper Pointer to store light bumper state
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool getLightBumper( roomberryLightBumper_t *lightBumper );

    /** Read light bumpers signal values
     *
     * get signal state of light bumpers (left/right, front left/right, center left/right)
     *
     *  Range 0-4095
     *
     * \param *lightBumperSignals Pointer to store light bumper signal values
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool getLightBumperSignals( roomberryLightBumperSignals_t *lightBumperSignals );
    
    
    /** Read front bumpers, light bumpers and light bumpers signal values all at once
     *
     * \param *bumper Pointer to store bumper state
     * \param *lightBumper Pointer to store light bumper state
     * \param *lightBumperSignals Pointer to store light bumper signal values
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool getAllBumper ( roomberryBumper_t *bumper,
                        roomberryLightBumper_t *lightBumper,
                        roomberryLightBumperSignals_t *lightBumperSignals );
    
    /** Read light bump left signal value
     *
     * The strength of the light bump signal is returned as an
     * unsigned 16-bit value in the range of 0-4095.
     *
     * \param *signal Pointer to store signal value
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool getLightBumpLeftSignal ( uint16_t *signal );
    
    /** Read light bump front left signal value
     *
     * The strength of the light bump signal is returned as an
     * unsigned 16-bit value in the range of 0-4095.
     *
     * \param *signal Pointer to store signal value
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool getLightBumpFrontLeftSignal ( uint16_t *signal );
    
    /** Read light bump center left signal value
     *
     * The strength of the light bump signal is returned as an
     * unsigned 16-bit value in the range of 0-4095.
     *
     * \param *signal Pointer to store signal value
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool getLightBumpCenterLeftSignal ( uint16_t *signal );

    /** Read right bump left signal value
     *
     * The strength of the light bump signal is returned as an
     * unsigned 16-bit value in the range of 0-4095.
     *
     * \param *signal Pointer to store signal value
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool getLightBumpRightSignal ( uint16_t *signal );
    
    /** Read light bump front right signal value
     *
     * The strength of the light bump  signal is returned as an
     * unsigned 16-bit value in the range of 0-4095.
     *
     * \param *signal Pointer to store signal value
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool getLightBumpFrontRightSignal ( uint16_t *signal );
    
    /** Read light bump center right signal value
     *
     * The strength of the light bump signal is returned as an
     * unsigned 16-bit value in the range of 0-4095.
     *
     * \param *signal Pointer to store signal value
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool getLightBumpCenterRightSignal ( uint16_t *signal );

    /** Read Battery Data
     * 
     * Get:
     * 
     * Roomba’s current charging state
     *
     * The voltage of Roomba’s battery in millivolts (mV)
     *
     * The current in milliamps (mA) flowing into or out
     * of Roomba’s battery.
     * Negative currents indicate that the current is flowing     
     * out of the battery, as during normal running. Positive
     * currents indicate that the current is flowing into the
     * battery, as during charging.
     * 
     * The temperature of Roomba’s battery in degrees Celsius.
     * 
     * The current charge of Roomba’s battery in milliamp-hours
     * (mAh). The charge value decreases as the battery is
     * depleted during running and increases when the battery
     * is charged.
     *
     * The estimated charge capacity of Roomba’s battery in
     * milliamp-hours (mAh).
     *
     * \param *roomberryBatteryData Pointer to store battery data
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool getBatteryData ( roomberryBatteryData_t *batteryData );

    /** Read distance Roomba has traveled
     *
     * The distance that Roomba has traveled in millimeters since the
     * distance it was last requested is sent as a signed 16-bit value,
     * high byte first. This is the same as the sum of the distance
     * traveled by both wheels divided by two. Positive values indicate
     * travel in the forward direction; negative values indicate travel
     * in the reverse direction. If the value is not polled frequently
     * enough, it is capped at its minimum or maximum.
     *
     * \param *distance Pointer to store distance (Range: -32768 – 32767)
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool getDistance( int16_t *distance );
    
    /** Read angle Roomba has turned
     *
     * The angle in degrees that Roomba has turned since the angle was
     * last requested. Counter-clockwise angles are positive and
     * clockwise angles are negative. If the value is not polled
     * frequently enough, it is capped at its minimum or maximum.
     * Range: -32768 – 32767
     *
     * \param *angle Pointer to store angle (Range: -32768 – 32767)
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool getAngle( int16_t *angle );
    
    /** Power down Roomba
     *
     * This command powers down Roomba. The OI can be in Passive, Safe,
     * or Full mode to accept this command.
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool powerDown(void);

    /** Make turn
     *
     * This command controls Roomba’s drive wheels. The longer radii make
     * Roomba drive straighter, while the shorter radii make Roomba turn more.
     * The radius is measured from the center of the turning circle to the
     * center of Roomba. A positive velocity and a positive radius makes
     * Roomba drive forward while turning toward the left. A negative radius
     * makes Roomba turn toward the right. Special cases for the radius make
     * Roomba turn in place or drive straight, as specified below. A negative
     * velocity makes Roomba drive backward.
     *
     * Special cases for radius:
     *
     * Straight = 32768 or 32767 = hex 8000 or 7FFF Turn in place clockwise = -1
     *
     * Turn in place counter-clockwise = 1
     *
     * \param velocity The average velocity of the drive wheels in millimeters per second (-500 – 500 mm/s)
     * \param radius   The radius in millimeters at which Roomba will turn  (-2000 – 2000 mm)
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool turn(int16_t velocity, int16_t radius);

    /** Let Roomba drive straight
     *
     * Roomba’s driving straight forward of reverse.
     *
     * \param velocity The average velocity of the drive wheels in millimeters per second (-500 – 500 mm/s)
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool driveStraight(int16_t velocity);

    /** Stop Roomba
     *
     * This stops any driving or turning action
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool stop(void);

    /** Let Roomba spin in place
     *
     * Roomba spins in place clockwise or counterclockwise
     *
     * \param velocity   The average velocity of the drive wheels in millimeters per second (-500 – 500 mm/s)
     * \param clockwise  TRUE -> spin clockwise, FALSE -> spin counterclockwise
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool spin(int16_t velocity, bool clockwise);

    /** Turn vacuum on or off
     *
     * Control Roomba's vacuum motor
     *
     * \param on  TRUE -> turn vacuum on, FALSE -> turn vacuum off
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool vacuum(bool on);
    
    /** Turn side brush on or off
     *
     * Control Roomba's side brush motor
     *
     * \param on  TRUE -> turn side brush on, FALSE -> turnside brush off
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool sideBrush(bool on);
    
    /** Control Power/Clean LED
     * 
     * Clean/Power uses a bicolor (red/green) LED. This LED is controlled
     * by two data bytes: one for the color and the other for the intensity.
     *
     * \param color  0 = green, 255 = red. Intermediate values are intermediate colors (orange, yellow, etc).
     * \param intensity  0 = off, 255 = full intensity. Intermediate values are intermediate intensities.
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool powerLed(uint8_t color, uint8_t intensity);
    
    /** Upload song to Roomba
     *
     * This command lets you specify up to four songs to the OI that you can play at
     * a later time. Each song is associated with a song number. The Play command uses
     * the song number to identify your song selection. Each song can contain up to
     * sixteen notes. Each note is associated with a note number that uses MIDI
     * note definitions and a duration. The duration of a musical note, in increments
     * of 1/64th of a second. Example: a half-second long musical note has a duration
     * value of 32.
     * The pitch of the musical note Roomba will play, according to the MIDI note
     * numbering scheme. The lowest musical note that Roomba will play is Note #31.
     * Roomba considers all musical notes outside the range of 31 – 127 as rest notes,
     * and will make no sound during the duration of those notes.
     *
     * \param songNumber The song number associated with the specific song. (0 – 4)
     * \param songLength The length of the song, according to the number of musical notes within the song. (1 – 16)
     * \param song Array of Pitch/Douration couples making up the song
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool uploadSong(uint8_t songNumber, uint8_t songLength, roomberryNote *song);

    /** Play specified  song
     *
     * Play song previously uploaded to Roomba.
     *
     * \param songNumber The song number to be played. (0 – 4)
     *
     * \return Successful command execution (TRUE/FALSE)
     */
    bool playSong(uint8_t songNumber);

private:
    int  ttyFd;
    uint8_t motorState;
    uint8_t ledState;
    uint8_t powerLedColor;
    uint8_t powerLedIntensity;
    
    bool getLightBumpSignal ( uint8_t pID, uint16_t *signal );
    bool sendCommand( uint8_t sendBytes, uint8_t receiveBytes, uint8_t* buffer);
};
#endif
