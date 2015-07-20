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

#include <string.h>
#include "Roomberry.h"

int main ( int argc, char* argv[] ) {
    roomberryBatteryData_t batteryData;
    
    Roomberry Roomba("/dev/ttyAMA0");
    
    if ( Roomba.getBatteryData( &batteryData ) ) {
        
        printf ( "Charging State: ");
        
        switch (batteryData.chargingState) {
            case 0:
                printf ( "Not charging\n");
                break;
            case 1:
                printf ( "Reconditioning Charging\n");
                break;
            case 2:
                printf ( "Full Charging\n");
                break;
            case 3: printf ( "Trickle Charging\n");
                break;
            case 4: printf ( "Waiting\n");
                break;
            case 5:
                printf ( "Charging Fault Condition\n");
                break;
            default:
                printf ( "Unknown\n");
        }
        
        printf ( "Voltage:        %d mV\n",  batteryData.voltage );
        printf ( "Current:        %d mA\n",  batteryData.current );
        printf ( "Temperature:    %d C\n",   batteryData.temperature );
        printf ( "Charge:         %d mAh\n", batteryData.batteryCharge );
        printf ( "Capacity:       %d mAh\n", batteryData.batteryCapacity );
    } else {
        printf ( "ERROR: Unable to read charging state\n");
        return(-1);
    }
    
    return(0);
}
