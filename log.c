/*****************************************************************************
 * Licensed under BSD 3-clause license. See LICENSE file for more information.
 * Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
 ****************************************************************************/

/*****************************************************************************
 * Include Files
 ****************************************************************************/

#include "log.h"

#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#include "version.h"

/*****************************************************************************
 * Constants
 ****************************************************************************/

/* Settings for the log_buffer function */

enum log_buffer_constants
{
  /* These can be modified to tune output for specific needs */

  LOG_LINE_SIZE = 16, /* How many bytes to print in one line */

  /* These constants are calculated based on previus one, and should not be
   * changed lightly */

  LOG_ADDRESS_DATA_SIZE    = 8,
  LOG_SINGLE_HEX_DATA_SIZE = 3,
  LOG_HEX_DATA_SIZE        = LOG_SINGLE_HEX_DATA_SIZE * LOG_LINE_SIZE,
  LOG_CHAR_DATA_SIZE       = LOG_LINE_SIZE,
};

/* Array index for colors to reset colors (in colorful output) */

#define COLOR_RESET (4)

/*****************************************************************************
 * Private Variables
 ****************************************************************************/

static enum log_level log_level = LOG_LEVEL_INFO;

/* Array of strings defining colors for colorful output */

static char colors[5][6];

/*****************************************************************************
 * Private Functions
 ****************************************************************************/

/*****************************************************************************
 * Name: print_line
 *
 * Description:
 *   Prints line_size bytes of buf prefixed by line_number in a nice format
 *
 * Input Parameters:
 *   buf        : Pointer to memory to print
 *   line_size  : How many bytes from buf to print
 *   line_number: Line number to prefix line with
 ****************************************************************************/

static void print_line(const uint8_t *buf, size_t line_size,
                       size_t line_number)
{
  /* Buffers to hold whole line of bytes representation */

  char hex_data[LOG_HEX_DATA_SIZE + 1] = {0};
  char char_data[LOG_CHAR_DATA_SIZE + 1] = {0};

  /* Calculate buf address value */

  const unsigned int ADDRESS = LOG_LINE_SIZE * line_number;

  /* Fill data buffers with bytes represenations */

  for (size_t char_num = 0; char_num < line_size; ++char_num)
    {
      unsigned char current_byte = (char)*buf++;
      char *current_hex_pos = hex_data + char_num * LOG_SINGLE_HEX_DATA_SIZE;
      char *current_char_pos = char_data + char_num;

      sprintf(current_hex_pos, "%02x ", current_byte);

      /* Check if current_byte is printable or not */

      if (current_byte < 0x20 || current_byte >= 0x7f)
        {
          current_byte = '.';
        }

      *current_char_pos = current_byte;
    }

  /* Print single line to stderr */

  fprintf(stderr, "0x%04x: %-*s %s\n", ADDRESS, LOG_HEX_DATA_SIZE, hex_data,
          char_data);
}

/*****************************************************************************
 * Name: log_level_handler
 *
 * Description:
 *   This is signal handler for changing log level on the fly.
 *
 * Input Parameters
 *   level: Log level:
 *     SIGRTMIN + 0: error
 *     SIGRTMIN + 1: warning
 *     SIGRTMIN + 2: info
 *     SIGRTMIN + 3: debug
 *
 *     Messages are logged from current level and ALL previous levels, ie.
 *     when level is WARNING, the warning and error messages will be logged.
 ****************************************************************************/

void log_level_handler(int level)
{
  log_level = level - SIGRTMIN;
}

/*****************************************************************************
 * Name: log_enabled
 *
 * Description:
 *   Checks if log string pointed by fmt should be logged or not. It also
 *   stores enum of log_level in parameter fmt_log_level
 *
 * Input Parameters:
 *   fmt: Log string
 *
 * Output Parameters:
 *   fmt_log_level: enum representation of log level
 *
 * Output Values:
 *   0: Log is NOT enabled for given fmt
 *   1: Log is enabled for given fmt
 ****************************************************************************/

int log_enabled(const char *fmt, enum log_level *fmt_log_level)
{
  if (fmt == NULL)
    {
      return 0;
    }

  /* set log level to info only to make unconditional message print in INFO
   * color */

  *fmt_log_level = LOG_LEVEL_INFO;

  if (fmt[1] != '/')
    {
      /* Log string doesn't contain log level information, print it anyway */

      return 1;
    }

  switch (fmt[0])
    {
    case 'D': *fmt_log_level = LOG_LEVEL_DEBUG; break;
    case 'I': *fmt_log_level = LOG_LEVEL_INFO; break;
    case 'W': *fmt_log_level = LOG_LEVEL_WARNING; break;
    case 'E': *fmt_log_level = LOG_LEVEL_ERROR; break;

    /* If log level is not recognized we unconditionally allow log print. */

    default: return 1;
    }

  if (*fmt_log_level <= log_level)
    {
      return 1;
    }

  /* No contidion to allow printing has been met */

  return 0;
}

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
 *   colorful: Makes prints look nice on colorful terminals
 ****************************************************************************/

void log_init(enum log_level level, int colorful)
{
  struct sigaction sa;

  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = log_level_handler;

  sigaction(SIGRTMIN + LOG_LEVEL_ERROR,   &sa, NULL);
  sigaction(SIGRTMIN + LOG_LEVEL_WARNING, &sa, NULL);
  sigaction(SIGRTMIN + LOG_LEVEL_INFO,    &sa, NULL);
  sigaction(SIGRTMIN + LOG_LEVEL_DEBUG,   &sa, NULL);

  if (colorful)
    {
      strcpy(colors[LOG_LEVEL_ERROR],   "\033[31m");
      strcpy(colors[LOG_LEVEL_WARNING], "\033[35m");
      strcpy(colors[LOG_LEVEL_INFO],    "\033[32m");
      strcpy(colors[LOG_LEVEL_DEBUG],   "\033[34m");
      strcpy(colors[COLOR_RESET],       "\033[0m");
    }

  if (level >= LOG_LEVEL_ERR)
    {
      fprintf(stderr, "E/Given log level is wrong, passed: %d\n", level);
      return;
    }

  log_level = level;
}

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
****************************************************************************/

void log_print(const char *file, int line, const char *fmt, ...)
{
  va_list va;
  enum log_level level;

  if (log_enabled(fmt, &level) == 0 || file == NULL)
    {
      return;
    }

  va_start(va, fmt);

  fprintf(stderr, "%s[%s:%d]", colors[level], file, line);
  vfprintf(stderr, fmt, va);
  fprintf(stderr, "%s", colors[COLOR_RESET]);

  va_end(va);
}

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
 *   fmt    : Custom message for the memory print (optional) (this also
 *     defines log level of tne buffer)
 *   ...    : Parameters for the fmt (optional)
 ****************************************************************************/

void log_buffer(const void *buf, size_t buflen, const char *fmt, ...)
{
  /* Single line of data printed on the stderr will be formated like this
   * (please note, that this is docuemnted for lines that shows 16 bytes in a
   * line, this value can be changed in compile time only):
   *
   * 0xNNNN: HH HH HH HH HH HH HH HH HH HH HH HH HH HH HH HH  CCCCCCCCCCCCCCCC
   *
   * Where:
   *   NNNN: Adress of the first byte in line
   *   HH  : Byte value in hex representation
   *   C   : Character representation of the byte (or '.' if not printable)
   *
   * First 8 bytes are fixed, and consists of address of the first byte and
   * ':  ' (2 spaces). Next bytes are counted from formula 3*LOG_LINE_SIZE (3
   * bytes are for 2 hex characters and a space). Next there is 2 bytes for
   * space, and LOG_LINE_SIZE bytes for character representation */

  /* Sanity check */

  if (buf == NULL)
    {
      return;
    }

  /* String needed to print separator. This string must be longer than any
   * possible separator to print */

  static const char *separator =
      "---------------------------------------------------------------------";

  /* Calculate some constants for the function call */

  const size_t MSG_SIZE = buflen;
  const size_t LINES_COUNT = MSG_SIZE / LOG_LINE_SIZE;
  const size_t LAST_LOG_LINE_SIZE = MSG_SIZE % LOG_LINE_SIZE;
  enum log_level level = LOG_LEVEL_DEBUG;

  /* Print user log */

  if (fmt)
    {
      /* Caller provided fmt, lets check if anything should be printed
       * (including buf) */

      if (log_enabled(fmt, &level) == 0)
        {
          return;
        }

      va_list va;
      va_start(va, fmt);

      fprintf(stderr, "%s", colors[level]);
      vfprintf(stderr, fmt, va);

      va_end(va);
    }
  else
    {
      fprintf(stderr, "%s", colors[LOG_LEVEL_DEBUG]);
    }

  /* Print log table preamble */

  fprintf(stderr, "%.*s %.*s  %.*s\n", LOG_ADDRESS_DATA_SIZE - 1, separator,
          LOG_HEX_DATA_SIZE - 1, separator, LOG_CHAR_DATA_SIZE, separator);

  fprintf(stderr, "%-*s%-*s%s\n", LOG_ADDRESS_DATA_SIZE, "addres:",
          LOG_HEX_DATA_SIZE + 1, "hex:", "ascii:");

  fprintf(stderr, "%.*s %.*s  %.*s\n", LOG_ADDRESS_DATA_SIZE - 1, separator,
          LOG_HEX_DATA_SIZE - 1, separator, LOG_CHAR_DATA_SIZE, separator);

  /* Print all WHOLE lines */

  for (size_t line_number = 0; line_number < LINES_COUNT; ++line_number)
    {
      print_line((const uint8_t *)buf, LOG_LINE_SIZE, line_number);

      /* Move memory pointer to point to next chunk of data to print */

      buf = (const uint8_t *)buf + LOG_LINE_SIZE;
    }

  /* If buflen is not divisible by LOG_LINE_SIZE we still need to print last
   * line that is less then LOG_LINE_SIZE */

  if (LAST_LOG_LINE_SIZE)
    {
      print_line((const uint8_t *)buf, LAST_LOG_LINE_SIZE, LINES_COUNT);
    }

  fprintf(stderr, "%.*s %.*s  %.*s%s\n", LOG_ADDRESS_DATA_SIZE - 1, separator,
          LOG_HEX_DATA_SIZE - 1, separator, LOG_CHAR_DATA_SIZE, separator,
          colors[COLOR_RESET]);
}

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

const char *log_version(char *major, char *minor, char *patch)
{
  if (major && minor && patch)
    {
      /* strtok modifies input string, so we need a copy of version */

      char version[11 + 1];
      strncpy(version, VERSION, 11);

      strncpy(major, strtok(version, "."), 3);
      strncpy(minor, strtok(NULL, "."), 3);
      strncpy(patch, strtok(NULL, "."), 3);
    }

  return VERSION;
}
