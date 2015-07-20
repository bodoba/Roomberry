//
//  EventLog.cpp
//  
//
//  Created by Bodo Bauer on 15.02.15.
//
//

#include <string.h>
#include "EventLog.h"
#include <sys/time.h>

EventLog::EventLog( uint16_t size ) {
    logSize=size;
    eventLog = (logEntry_t*) malloc ( sizeof(logEntry_t) * logSize + 1 );
    lastEntry=0;
    numEntry=1;

    eventLog[0].count=0;
    eventLog[0].timestamp=getUsecTime();
    strncpy( eventLog[0].text, "Initialized Log", 64);
}

logEntry_t EventLog::getFirstLogEntry( void ) {
    return (eventLog[getIndexFirst()]);
}

logEntry_t EventLog::getNextLogEntry( void ) {
    // TDB !!
    return (eventLog[getIndexFirst()]);
}

uint16_t   EventLog::getIndexLast( void ) {
    return ( lastEntry );
}

uint16_t   EventLog::getIndexFirst( void ) {
    uint16_t firstEntry = 0;
    if ( numEntry >= logSize ) {
        firstEntry = (lastEntry+1) % logSize;
    }
    return ( firstEntry );
}

uint16_t   EventLog::addLogEntry( const char *text ) {
    uint16_t count = eventLog[lastEntry].count;
    
    if ( ++lastEntry >= logSize ) {
        lastEntry = 0;
    }

    if ( numEntry < logSize ) {
        numEntry++;
    }

    eventLog[lastEntry].count=count+1;
    eventLog[lastEntry].timestamp=getUsecTime();
    strncpy( eventLog[lastEntry].text, text, 64);
    
    return ( lastEntry );
}

logEntry_t EventLog::getLogEntry( uint16_t entry ) {
    return (eventLog[entry]);
}

uint64_t EventLog::getUsecTime(void) {
    struct timeval tp;
    uint64_t sec, usec;
    
    gettimeofday( &tp, NULL );
    sec  = (uint64_t)( tp.tv_sec );
    usec = (uint64_t)(( tp.tv_usec ));
    return ( sec * 1000000 + usec);
}