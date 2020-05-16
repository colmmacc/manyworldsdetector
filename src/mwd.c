#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "mwd.h"

static __thread void *zeroed_when_copied;

static void mwd_on_fork()
{
    *(int *) zeroed_when_copied = 0;
}

/*
** Reset the many worlds detector
*/
void mwd_reset(void)
{
    *(int *) zeroed_when_copied = 1;
}

/*
** Initialize the many worlds detector
**
** @return 0 on success, -1 on failure to initialize
**/
int mwd_init(void)
{
    long pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize < 0) {
        return -1;
    }

    if (posix_memalign(&zeroed_when_copied, pagesize, pagesize) != 0) {
        return -1;
    }

    mwd_reset();

    int success = -1;

#if defined(MADV_WIPEONFORK)
    /* Linux: Set the memory to be zeroed by the kernel or hypervisor */
    if (madvise(zeroed_when_copied, pagesize, MADV_WIPEONFORK) == 0) {
        success = 0;
    }
#endif

#if defined(MAP_INHERIT_ZERO)
    /* BSD: Set the memory not to be inheritable */
    if (minherit(zeroed_when_copied, pagesize, MAP_INHERIT_ZERO) == 0) {
        success = 0;
    }
#endif

    /* Use pthreads to zero the memory too */
    if (pthread_atfork(NULL, NULL, mwd_on_fork) == 0) {
        success = 0;
    }

    return success;
}

/*
** Has a universe split occured?
**
** @return true if we're in a new universe. Otherwise return false.
*/
bool mwd_cloned(void)
{
    return (*(int *) zeroed_when_copied == 0);
}
