Note
====

This project is deprecated and won't be updated. Please check out
https://github.com/mlyszczek/embedlog which is much better version of
**liblog**

Name
====

liblog - A simple C logging library

Synopsis
========

~~~{.c}
#include <log.h>

enum log_level
{
  LOG_LEVEL_ERROR = 0,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_INFO,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_ERR
};

void LOGN(fmt);
void LOGV(fmt, ...);
void PERROR(fmt);

void log_init(enum log_level level, int colorful);
void log_print(const char *file, int line, const char *fmt, ...);
void log_buffer(const void *buf, size_t buflen, const char *fmt, ...);
const char *log_version(char *major, char *minor, char *patch);
~~~

Description
===========

Library allows printing formatted messages to stderr. It does NOT let you
print into file. To print to file one should use use some daemon that
redirects output to file (like svlogd).

Before library functions can be used properly user should call *log_init*
function. Call *log_init* will initialize what messages can be logger
(*log_level*) and it output should be colored or not. Init function will also
install signal handlers for dynamic log level change

After init is called, user can use rest of the functions properly. *log_print*
function is not easy to use and so is rather used with macros than alone.
There are two basic macros *LOGV* and *LOGN*. Both do the same thing, prints
messages to stderr (if *log_level* is allowed) in a formatted matter. The only
difference is that *LOGN* is used without any special parameters (like %d or
%s), and *LOGV* can use all format parameters as printf does it. This is
necessary to be c89 complaint.

Another macro is *PERROR* it is same as *perror* from *stdio.h* but also adds
file and line number where call was made. Function adds new line automaticaly.

Library also provides handy function *log_buffer*. It allows to print any
memory in wireshark-like format.

Library also installs 4 signal handlers to switch logging level on the run. It
might be useful, when one need to start debugging app without restarting it.
Installed signals are *SIGRTMIN + LOG_LEVEL*

Library logs messages from currently set log or about. So if *log_level* is
info, then error, warning and info will be logged and debug will be skipped.
When user sets *log_level* to error, only errors will be printed, and for
debug, all messages will get logged
