/*****************************************************************************
 * Licensed under BSD 3-clause license. See LICENSE file for more information.
 * Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
 ****************************************************************************/

#ifndef BOFC_LIBLOG_H
#define BOFC_LIBLOG_H

/*****************************************************************************
 * Include files
 ****************************************************************************/

#include <stdio.h>

/*****************************************************************************
 * Preprocesor directives
 ****************************************************************************/

/*****************************************************************************
 * Name: LOGN
 *
 * Description:
 *   Macro logs to stderr messages without additional parameters. It should be
 *   used for calls like "LOGN("log string\n")"
 ****************************************************************************/

#define LOGN(fmt) \
  do \
    { \
      log_print(__FILE__, __LINE__, fmt); \
    } \
  while (0)

/*****************************************************************************
 * Name: LOGV
 *
 * Description:
 *   Variadic version of LOGN which should be used when printing with
 *   additional parameters like "LOGV("log: %s", string)"
 ****************************************************************************/

#define LOGV(fmt, ...) \
  do \
    { \
      log_print(__FILE__, __LINE__, fmt, __VA_ARGS__); \
    } \
  while (0)

/*****************************************************************************
 * Public Types
 ****************************************************************************/

enum log_level
{
  LOG_LEVEL_ERROR = 0,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_INFO,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_ERR // not a log level, just value indicating wrong log level
};

/*****************************************************************************
 * Public Functions
 ****************************************************************************/

/*****************************************************************************
 * Name: log_init
 *
 * Description:
 *   Initializes logger and signal handlers for logger level
 *
 * Input Parameters:
 *   level:    Initial log level (can be changed later using signals)
 *   colorful: Make prints look nice on colorful terminal
 ****************************************************************************/

void log_init(enum log_level level, int colorful);

/*****************************************************************************
 * Name: log_print
 *
 * Description:
 *   Prints fmt to stderr including file and line.
 *
 * Input Parameters:
 *   file: Location from where log is called (usually __FILE__)
 *   line: Location from where log is called (usually __LINE__)
 *   fmt : Message log. If first character is one of (D, I, W or E) and second
 *     one is '/', then in pair it is treated as log level
 *
 * Info:
 *   This function alone is not a big of a help and it should be rather called
 *   with macros LOGN and LOGV
 ****************************************************************************/

void log_print(const char *file, int line, const char *fmt, ...);

/*****************************************************************************
 * Name: log_buffer
 *
 * Description:
 *   Function prints given buf (memory) in a nice format, try it, you'll like
 *   it.
 *
 * Input Parameters:
 *   buf    : Adress of the memory to print
 *   buflen : How many bytes to print
 *   fmt    : Custom message for the memory print (optional)
 *   ...    : Parameters for the fmt (optional)
 ****************************************************************************/

void log_buffer(const void *buf, size_t buflen, const char *fmt, ...);

/*****************************************************************************
 * Name: log_version
 *
 * Description:
 *   Returns version of the library
 *
 * Output Parameters:
 *   major: Major version (API changes)
 *   minor: Minor version (Added functions)
 *   patch: Patch version (bug fixes)
 *
 *   Function also returns pointer to char where full version string is stored
 ****************************************************************************/

const char *log_version(char *major, char *minor, char *patch);

#endif /* BOFC_LIBLOG_H */
