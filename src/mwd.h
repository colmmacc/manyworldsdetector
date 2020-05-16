#pragma once

#include <stdbool.h>

/*
** Initialize the many worlds detector
**
** @return 0 on success, -1 on failure to initialize
**/
extern int mwd_init(void);

/*
** Has a universe split occured?
**
** @return true if we're in a new universe. Otherwise return false.
*/
extern bool mwd_cloned(void);

/*
** Reset the many world's detector
*/
extern void mwd_reset(void);
