#ifndef __SCM_LOG_H__
#define __SCM_LOG_H__

typedef enum {
    LOG_NONE = 0,
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO,
    LOG_DEBUG,
    LOG_TRACE,
    LOG_NUM_LEVELS,
} scm_log_level_t;

void scm_log_init(void);
void scm_log_set_level(scm_log_level_t level);
scm_log_level_t scm_log_get_level(void);
void scm_log(
    scm_log_level_t level, const char* context, const char* format, ...);

// #define GET_CONTEXT()                                                   \
//     ({                                                                  \
//         const char* file = __FILE__;                                    \
//         const char* last_slash = strrchr(file, '/');                    \
//         const char* basename = last_slash ? last_slash + 1 : file;      \
//         const char* dot = strrchr(basename, '.');                       \
//         const char* line = __LINE__;                                    \
//         const char* col = __COLUMN__;                                   \
//         static char context[256];                                       \
//         size_t len = dot ? (size_t)(dot - basename) : strlen(basename); \
//         strncpy(context, basename, len);                                \
//         strcat(context, ":");                                           \
//         strcat(context, line);                                          \
//         strcat(context, col);                                           \
//         context[len + strlen(line) + strlen(len) + 3] = '\0';           \
//         context;                                                        \
//     })

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

#define GET_CONTEXT()                                                   \
    ({                                                                  \
        const char* file = __FILE__;                                    \
        const char* last_slash = strrchr(file, '/');                    \
        static char context[256];                                       \
        snprintf(context, sizeof(context), "%s:%d", file, __LINE__);    \
        context;                                                        \
    })

#pragma GCC diagnostic pop

// const char* basename = last_slash ? last_slash + 1 : file;
// const char* dot = strrchr(basename, '.');                    
// size_t len = dot ? (size_t)(dot - basename) : strlen(basename);

void scm_log_timestamp_mode(int enable);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#define SCM_LOG(level, msg, ...) \
    scm_log(level, GET_CONTEXT(), msg, ##__VA_ARGS__)
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#define SCM_DEBUG(msg, ...) \
    scm_log(LOG_DEBUG, GET_CONTEXT(), msg, ##__VA_ARGS__)
#pragma GCC diagnostic pop

#define SCM_ERROR(msg, ...) \
    scm_log(LOG_ERROR, GET_CONTEXT(), msg, ##__VA_ARGS__)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#define SCM_LOG_IF(level, expr)               \
    do {                                      \
        if ((level) <= scm_log_get_level()) { \
            (expr);                           \
            printf("\n");                     \
        }                                     \
    } while (0)
#pragma GCC diagnostic pop

#endif // !__SCM_LOG_H__
