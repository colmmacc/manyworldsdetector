# Many Worlds Detector

The Many Worlds Detector is a micro library that allows code to detect that it
has been cloned. This is useful in libraries that need to maintain per-process
or per-thread state that can't be safely copied and can't control whether or
when the application using the library creates processes or threads. For
example a user-space random number generator needs to reseed whenever its
internal state is copied.

## How to use the many worlds detector

    int mwd_init(void);

The Many Worlds Detector needs to be initialized by calling ``mwd\_init()''.
``mwd\_init()'' will return 0 on success, or -1 on a failure to initialize.

Once initialized, the Many Worlds Defender can be used by calling
``mwd\_defend()''.

    typedef enum {
        MWD_NOT_INITIALIZED,
        MWD_NEW_PROCESS,
        MWD_NEW_THREAD,
        MWD_NO_ACTION
    } mwd_reason_t;

    typedef int (*mwd_callback) (mwd_reason_t reason, void *ctx);

    mwd_reason_t mwd_defend(mwd_callback callback, void *ctx, int *ret);

If the many worlds detector detects that it is running in a different thread
or process than the either the most recent invokation of ``mwd\_defend()'' or
``mwd\_init()'', then the supplied callback function will be invoked, and the
return value of that function placed in ``ret''.

The callback function will be passed a reason corresponding to either
``MWD\_NEW\_PROCESS'' or ``MWD\_NEW\_THREAD'' depending on the reason it was
invoked, as well as a caller-supplied ``ctx'' pointer.

If a new process or thread was detected, ``mwd\_defend()'' will return
the same reason supplied to the callback. Otherwise ``MWD\_NOT\_INITIALIZED''
will be returned if the Many Worlds Detector library was not initialized, or
``MWD\_NO\_ACTION'' if no new process or thread was detected.

