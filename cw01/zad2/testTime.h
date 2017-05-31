#ifndef TESTTIME
#define TESTTIME

#include <stdio.h>
#include <sys/times.h>
#include <time.h>

void testTimeReset();
void testTime(char str[],void (*fun)(int),int n,int save);
void testTimePrint(char text[]);

#endif // TEST
