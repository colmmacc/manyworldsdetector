#pragma once

/*
** Initialize the many worlds detector
**
** @return 0 on success, -1 on failure to initialize
*/
extern int mwd_init(void);

typedef enum {
    MWD_NOT_INITIALIZED = -1,
    MWD_NEW_PROCESS     =  1,
    MWD_NEW_THREAD      =  2,
    MWD_NO_ACTION       =  3
} mwd_reason_t;

typedef int (*mwd_callback) (mwd_reason_t reason, void *ctx);

/*
** Has a universe split occured? If mwd_defend is running in a new
** process or thread, it will invoke the callback function that was
** supplied to mwd_init, passing the supplied context to that
** function.
**
** @param calback   A function that will be invoked if a new process or
**                  thread is detected.
** @param ctx       Context to be supplied to the callback
** @param ret       The return value of the callback funtion, if invoked
**
** @return -MWD_NOT_INITIALIZED if mwd has not been initialized
**          MWD_NEW_PROCESS if a new process was detected
**          MWD_NEW_THREAD if a new thread was detected
**          MWD_NO_ACTION  if no new process or thread was detected
*/
extern mwd_reason_t mwd_defend(mwd_callback callback, void *ctx, int *ret);
