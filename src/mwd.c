#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "mwd.h"

/* This value is always zero and we leave it alone */
static int zero = 0;

/* We set up a page that will be zeroed on fork() */
static void *zeroed_when_copied_page = NULL;

/* Our thread-local sentinel. It's initialized to &zero so that new threads can be detected.*/
static __thread void *zeroed_when_copied = &zero;

static void mwd_on_fork()
{
    *(int *) zeroed_when_copied = 0;
}

static void mwd_reset()
{
    /* Be ready to detect */
    *(int *) zeroed_when_copied_page = 1;

    /* Pivot from &zero to our magic page */
    zeroed_when_copied = zeroed_when_copied_page;
}

int mwd_init(void)
{
    long pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize < 0) {
        return -1;
    }

    if (posix_memalign(&zeroed_when_copied_page, pagesize, pagesize) != 0) {
        return -1;
    }

    int success = -1;

#if defined(MADV_WIPEONFORK)
    /* Linux: Set the memory to be zeroed by the kernel or hypervisor */
    if (madvise(zeroed_when_copied_page, pagesize, MADV_WIPEONFORK) == 0) {
        success = 0;
    }
#endif

#if defined(MAP_INHERIT_ZERO)
    /* BSD: Set the memory not to be inheritable */
    if (minherit(zeroed_when_copied_page, pagesize, MAP_INHERIT_ZERO) == 0) {
        success = 0;
    }
#endif

    /* Use pthreads to zero the memory too */
    if (pthread_atfork(NULL, NULL, mwd_on_fork) == 0) {
        success = 0;
    }

    mwd_reset();

    return success;
}

mwd_reason_t mwd_defend(mwd_callback callback, void *ctx, int *ret)
{
    if (zeroed_when_copied_page == NULL) {
        return MWD_NOT_INITIALIZED;
    }

    if (*(int *) zeroed_when_copied == 1) {
        return MWD_NO_ACTION;
    }

    mwd_reason_t reason = MWD_NEW_PROCESS;
    if (zeroed_when_copied == &zero) {
        reason = MWD_NEW_THREAD;
    }

    /* We're in either a new thread or a new process, so call the callback */
    *ret = callback(reason, ctx);

    mwd_reset();

    return reason;
}
