//
//  EventLog.h
//  
//
//  Created by Bodo Bauer on 15.02.15.
//
//

#ifndef EVENTLOG_H
#define EVENTLOG_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <stdint.h>

typedef struct {
    uint16_t count;
    uint64_t timestamp;
    char     text[64];
} logEntry_t;


class EventLog {
  
public:
    EventLog( uint16_t size );
    
    uint16_t   addLogEntry( const char *text );
    logEntry_t getLogEntry( uint16_t );
    uint16_t   getIndexLast( void );
    uint16_t   getIndexFirst( void );
    logEntry_t getFirstLogEntry( void );
    logEntry_t getNextLogEntry( void );
    
private:
    logEntry_t *eventLog;

    uint16_t lastEntry;
    uint16_t logSize;
    uint16_t numEntry;
    uint16_t logCursor;
    
    uint64_t getUsecTime(void);
};

#endif /* defined(____EventLog__) */
