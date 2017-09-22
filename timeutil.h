#ifndef _COMM_TIMEUTIL_H_
#define _COMM_TIMEUTIL_H_


static inline uint32_t day()
{
    time_t tm;
    struct tm *curtime;
    time(&tm);
    curtime = localtime(&tm);

    return (1900+curtime->tm_year)* 10000 + (1 + curtime->tm_mon) + 100 + curtime->tm_mday;
}

static inline const char *date()
{
    time_t tm;
    struct tm *curtime;
    static char s_date[100];

    time(&tm);

    curtime = localtime(&tm);
    sprintf(s_date, "%d-%d-%d", (1900+curtime->tm_year), (1 + curtime->tm_mon), curtime->tm_mday);

    return s_date;
}

static inline const char *now()
{
    time_t tm;
    struct tm *curtime;
    static char s_date[100];

    time(&tm);

    curtime = localtime(&tm);
    sprintf(s_date, "%02d:%02d:%02d", curtime->tm_hour, curtime->tm_min, curtime->tm_sec);

    return s_date;
}

#endif