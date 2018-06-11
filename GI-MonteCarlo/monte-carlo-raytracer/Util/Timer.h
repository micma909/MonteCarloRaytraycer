#ifndef kdtree_Timer_h
#define kdtree_Timer_h

#include <time.h>
#include <sys/time.h>
#include <stdio.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#ifdef __WIN32
#include <windows.h>

LARGE_INTEGER
getFILETIMEoffset()
{
    SYSTEMTIME s;
    FILETIME f;
    LARGE_INTEGER t;
    
    s.wYear = 1970;
    s.wMonth = 1;
    s.wDay = 1;
    s.wHour = 0;
    s.wMinute = 0;
    s.wSecond = 0;
    s.wMilliseconds = 0;
    SystemTimeToFileTime(&s, &f);
    t.QuadPart = f.dwHighDateTime;
    t.QuadPart <<= 32;
    t.QuadPart |= f.dwLowDateTime;
    return (t);
}

int
clock_gettime(int X, struct timeval *tv)
{
    LARGE_INTEGER           t;
    FILETIME            f;
    double                  microseconds;
    static LARGE_INTEGER    offset;
    static double           frequencyToMicroseconds;
    static int              initialized = 0;
    static BOOL             usePerformanceCounter = 0;
    
    if (!initialized) {
        LARGE_INTEGER performanceFrequency;
        initialized = 1;
        usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);
        if (usePerformanceCounter) {
            QueryPerformanceCounter(&offset);
            frequencyToMicroseconds = (double)performanceFrequency.QuadPart / 1000000.;
        } else {
            offset = getFILETIMEoffset();
            frequencyToMicroseconds = 10.;
        }
    }
    if (usePerformanceCounter) QueryPerformanceCounter(&t);
    else {
        GetSystemTimeAsFileTime(&f);
        t.QuadPart = f.dwHighDateTime;
        t.QuadPart <<= 32;
        t.QuadPart |= f.dwLowDateTime;
    }
    
    t.QuadPart -= offset.QuadPart;
    microseconds = (double)t.QuadPart / frequencyToMicroseconds;
    t.QuadPart = microseconds;
    tv->tv_sec = t.QuadPart / 1000000;
    tv->tv_usec = t.QuadPart % 1000000;
    return (0);
}

#endif

#ifdef __APPLE__  // OS X does not have clock_gettime, use clock_get_time
void current_utc_time(struct timespec *ts) {
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    ts->tv_sec = mts.tv_sec;
    ts->tv_nsec = mts.tv_nsec;
}
#endif
    
#ifdef __WIN32
void current_utc_time(struct timeval *ts) {
    clock_gettime(0, ts);
}
#endif

#ifdef __gnu_linux__
#include <time.h>
#include <sys/time.h>
void current_utc_time(struct timespec *ts) {
    clock_gettime(0, ts);
}
#endif



class Timer {
private:
#ifdef __WIN32
    typedef struct timeval t_struct;
#endif
#ifdef __MACH__
    typedef struct timespec t_struct;
#endif
#ifdef __gnu_linux__
    typedef struct timespec t_struct;

#endif
         t_struct t0_,t1_;

    bool isStopped_{false};
public:
    
    Timer(){
        
    }
    
    static unsigned long getCurrentTime(){
        t_struct t;
        current_utc_time(&t);
        return t.tv_sec;
    }

    Timer(bool start){

    }
    
    void start(){
        current_utc_time(&t0_);
    }
    
    void stop(){
        current_utc_time(&t1_);
        isStopped_ = true;
    }
    
    void resume(){
        assert(false); //Not implemented
    }
    
    float read() const{
        if (isStopped_) {
            return float(t1_.tv_sec-t0_.tv_sec);
        }else{
            struct timespec t;
            current_utc_time(&t);
            return float(t.tv_sec-t0_.tv_sec);
        }
    }
    
    static Timer timedEvent(void (*f)(void)){
        Timer t;
        t.start();
        f();
        t.stop();
        return t;
    }
    
    friend std::ostream& operator<<(std::ostream &o,const Timer &t){
        o << t.read() << "s";
        return o;
    }
};

#endif
