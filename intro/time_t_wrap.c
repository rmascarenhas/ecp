/* time_t_wrap.c - Shows when and after how much time time_t will wrap.
 *
 * Some time ago, in the golden days of UNIX, the word processor source had a
 * comment that read:
 *
 *    Take this out and a UNIX Demon will dog your steps from now until the
 *    time_t's wrap around.
 *
 * The comment was eventually removed, and the UNIX Demon is still hunting
 * the outlaw.
 *
 * This program determines when will time_t wrap and then print the date
 * on the standard output. It will also calculate how long the UNIX Demon
 * should be hunting based on the current time.
 *
 * Author: Renato Mascarenhas
 */

#include <time.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define MINUTE (60)
#define HOUR   (60 * MINUTE)
#define DAY    (24 * HOUR)
#define WEEK   (7 * DAY)
#define MONTH  (30 * DAY)
#define YEAR   (365 * DAY)

void printInfo(const char *name, double *seconds, int period);

int
main() {
  time_t time_t_wrap, currtime;
  struct tm *utc_time;
  double diff;

  /* time_t can be defined in multiple ways according to the platform. The
   * standard even states that it can be stored as an integer or a floating point
   * number. In my current platform, it is stored as a plain 32-bit int. You should
   * check what type it is defined to in your host platform by taking a look at
   * /usr/include/time.h */
  time_t_wrap = INT_MAX;

  /* use gmtime(2) in order to retrieve time in UTC, instead of relying on local time,
   * which may vary depending on the system this program is run */
  utc_time = gmtime(&time_t_wrap);
  if (utc_time == NULL) {
    perror("gmtime");
    exit(EXIT_FAILURE);
  }

  printf("UNIX Demon will hunt you until %s", asctime(utc_time));

  currtime = time(NULL);
  if (currtime == -1) {
    perror("time");
    exit(EXIT_FAILURE);
  }

  diff = difftime(time_t_wrap, currtime);
  printf("That is, ");
  printInfo("years",   &diff, YEAR);
  printInfo("months",  &diff, MONTH);
  printInfo("weeks",   &diff, WEEK);
  printInfo("days",    &diff, DAY);
  printInfo("hours",   &diff, HOUR);
  printInfo("minutes", &diff, MINUTE);

  if (diff > 0) {
    printf("and %d seconds.", (int) diff);
  }

  printf("\n");

  exit(EXIT_SUCCESS);
}

void
printInfo(const char *name, double *seconds, int period) {
  int n;

  n = *seconds / period;
  if (n > 0) {
    printf("%d %s, ", n, name);
  }

  *seconds -= period * n;
}
