#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>

#ifndef WARN_TIME_SYSCALL
#define WARN_TIME_SYSCALL
#warning Syscall _times not implemented.
#endif

/* This is only provided as a stub. */
clock_t
_times (struct tms *tp)
{
  return -1;
}
