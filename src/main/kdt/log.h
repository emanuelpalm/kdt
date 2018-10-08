/**
 * Logging utilities.
 *
 * This particular logger divides log messages into four categories. Panics,
 * bugs, warnings and notices.
 *
 * - Panics are used to report on events that cannot be meaningfully recovered
 *   from, such as critical system resources not being available.
 * - Bugs are used to report when entering application states that should be
 *   impossible, such as an enumerator having an unexpected value.
 * - Warnings are used to report when operations fail but can be meaningfully
 *   recovered from, such as network failures.
 * - Notices are statements about events of interest that cannot be classified
 *   as being failures, such as clients connecting.
 *
 * @file
 */
#ifndef KDT_LOG_H
#define KDT_LOG_H

#include <stdnoreturn.h>

/**
 * Log call context macro.
 *
 * This macro expands into a call context which describes the file and line at
 * which the macro is placed.
 */
#define LOG_CTX ((log_Context) { __FILE_REL__, __LINE__ })

typedef struct log_Context log_Context;

/**
 * Log call context.
 *
 * Represents the function, file and line at which a log function was invoked.
 */
struct log_Context {
    const char *file;
    const int line;
};

/**
 * Initialized logger.
 */
void log_Init();

/**
 * Logs message and exits application with non-normal status code.
 *
 * @param MESSAGE Message to report.
 */
#define log_Panic(MESSAGE) log_PanicCF(LOG_CTX, MESSAGE)

/**
 * Logs formatted string and exits application with non-normal status code.
 *
 * @param FORMAT Format string of reported message.
 * @param ... Format arguments.
 */
#define log_PanicF(FORMAT, ...) log_PanicCF(LOG_CTX, FORMAT, __VA_ARGS__)

/**
 * Logs call context and formated string and exists application with non-normal
 * status code.
 *
 * @param context Call context.
 * @param format Format string of reported message.
 * @param ... Format arguments.
 */
noreturn void log_PanicCF(log_Context context, const char *format, ...);

/**
 * Logs bug.
 *
 * @param MESSAGE Message to report.
 */
#define log_Bug(MESSAGE) log_BugCF(LOG_CTX, MESSAGE)

/**
 * Logs formatted string bug.
 *
 * @param FORMAT Format string of reported message.
 * @param ... Format arguments.
 */
#define log_BugF(FORMAT, ...) log_BugCF(LOG_CTX, FORMAT, __VA_ARGS__)

/**
 * Logs call context and formated string bug.
 *
 * @param context Call context.
 * @param format Format string of reported message.
 * @param ... Format arguments.
 */
void log_BugCF(log_Context context, const char *format, ...);

/**
 * Logs warning.
 *
 * @param MESSAGE Message to report.
 */
#define log_Warn(MESSAGE) log_WarnCF(LOG_CTX, MESSAGE)

/**
 * Logs formatted string warning.
 *
 * @param FORMAT Format string of reported message.
 * @param ... Format arguments.
 */
#define log_WarnF(FORMAT, ...) log_WarnCF(LOG_CTX, FORMAT, __VA_ARGS__)

/**
 * Logs call context and formated string warning.
 *
 * @param context Call context.
 * @param format Format string of reported message.
 * @param ... Format arguments.
 */
void log_WarnCF(log_Context context, const char *format, ...);

/**
 * Logs notice.
 *
 * @param MESSAGE Message to report.
 */
#define log_Note(MESSAGE) log_NoteCF(LOG_CTX, MESSAGE)

/**
 * Logs formatted string notice.
 *
 * @param FORMAT Format string of reported message.
 * @param ... Format arguments.
 */
#define log_NoteF(FORMAT, ...) log_NoteCF(LOG_CTX, FORMAT, __VA_ARGS__)

/**
 * Logs call context and formated string notice.
 *
 * @param context Call context.
 * @param format Format string of reported message.
 * @param ... Format arguments.
 */
void log_NoteCF(log_Context context, const char *format, ...);

#endif