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

#ifndef CMDLINE_H
#define CMDLINE_H

#ifndef TRUE
#define TRUE	(1==1)
#define FALSE	(0==1)
#endif

class CmdLine {
public:
    CmdLine(int argc, char *argv[] );
    
    bool useRoomba(void);
    bool useCamera(void);
    bool showPreviewWindow(void);
    
private:
    int   argc;
    char *argv[];
    
    bool UseCamera;
    bool ShowPreviewWindow;
    bool UseRoomba;
};
#endif
