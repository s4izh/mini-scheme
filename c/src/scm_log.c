#include "scm_log.h"
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

const char* level_str[] = {"NONE", "ERROR", "WARNING",
                           "INFO", "DEBUG", "TRACE"};
// static_assert(sizeof(level_str) / sizeof(level_str[0]) == LOG_NUM_LEVELS);

int current_log_level = 0;
FILE* log_output;
int timestamp_mode = 0;

void scm_log_init()
{
    log_output = stdout;
}

void scm_log_set_level(scm_log_level_t level)
{
    current_log_level = level;
}

void scm_log_timestamp_mode(int enable)
{
    timestamp_mode = enable;
}

void scm_log(
    scm_log_level_t level, const char* context, const char* format, ...)
{
    if (level > current_log_level)
        return;

    va_list args;
    va_start(args, format);

    char timestamp[30];
    struct timeval tv;
    struct tm *tm_info;

    if (timestamp_mode)
    {
        gettimeofday(&tv, NULL);
        tm_info = localtime(&tv.tv_sec);

        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
        snprintf(timestamp + 19, sizeof(timestamp) - 19, ".%06ld", tv.tv_usec);

        fprintf(
                log_output, "%s %s %-*s", timestamp, level_str[level], 18, context);
    }
    else
    {
        // fprintf(log_output, "%s:0 %-*s", context, 8, level_str[level]);
        fprintf(log_output, "%s:0 %s ", context, level_str[level]);
    }

    vfprintf(log_output, format, args);
    fprintf(log_output, "\n");

    fflush(log_output);

    va_end(args);
}
