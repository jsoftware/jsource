/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
// emulation of Unix functions on Windows
#ifdef _WIN32
// Tip o'hat to Michaelangel007 on StackOverflow
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdint.h> // portable: uint64_t   MSVC: __int64 
#include "jerr.h"

struct jtimespec { long long tv_sec, tv_nsec; };
struct jtimeval { long long tv_sec, tv_usec; };
struct jtimezone { int tz_minuteswest, tz_dsttime; };

int jgettimeofday(struct jtimeval *tp, struct jtimezone * tzp)
{
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970 
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime( &system_time );
    SystemTimeToFileTime( &system_time, &file_time );
    time =  ((uint64_t)file_time.dwLowDateTime )      ;
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
    return 0;
}

//monotonic clock
//alternative is QueryPerformanceCounter; it probably uses rdtsc, which is stable on recent processors, but it gives inconsistent results when the processor goes to sleep
struct jtimespec jmtclk(){
 long long t=GetTickCount64();
 return (struct jtimespec){.tv_sec=t/1000,.tv_nsec=1000000*(t%1000)};}
struct jtimespec jmtfclk(){return jmtclk();}

// unavailable on older windows
#if NOSYNCHLIB
#define WakeByAddressSingle(p)
#define WakeByAddressAll(p)
#define WaitOnAddress(p,v,n,ns) 1
#endif

void jfutex_wake1(unsigned *p){WakeByAddressSingle(p);}
void jfutex_wakea(unsigned *p){WakeByAddressAll(p);}
unsigned char jfutex_wait(unsigned *p,unsigned v){return WaitOnAddress(p,&v,4,INFINITE)?0:EVFACE;}
long long jfutex_waitn(unsigned *p,unsigned v,unsigned long long ns){
 if(WaitOnAddress(p,&v,4,ns/1000000))return 0;
 if(GetLastError()==ERROR_TIMEOUT)return -1;
 //is there EINTR on windows?  Does it manifest as a spurious wake with no error?
 return EVFACE;}
#else
#include"j.h"
struct jtimespec jmtclk(){struct timespec r; clock_gettime(CLOCK_MONOTONIC,&r);R r;}
#ifndef CLOCK_MONOTONIC_RAW
#define CLOCK_MONOTONIC_RAW CLOCK_MONOTONIC
#endif
struct jtimespec jmtfclk(){struct timespec r; clock_gettime(CLOCK_MONOTONIC_RAW,&r);R r;}
#endif
