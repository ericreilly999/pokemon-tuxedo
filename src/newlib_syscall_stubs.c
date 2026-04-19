/*
 * newlib_syscall_stubs.c
 *
 * Stub implementations for newlib reentrant syscalls and threading primitives
 * required when building with devkitARM 15.2.0's thread-safe newlib.
 *
 * These stubs are sufficient for a GBA ROM that does not use an OS, threading,
 * or file I/O. All syscalls return error/failure; threading stubs are no-ops.
 */

#include <sys/reent.h>
#include <sys/stat.h>
#include <sys/lock.h>
#include <errno.h>

/* Global reentrant struct used by newlib */
static struct _reent s_reent = _REENT_INIT(s_reent);

/* __getreent: return the per-thread reentrant struct.
 * On bare-metal GBA there is only one thread, so return the global struct. */
struct _reent* __getreent(void)
{
    return &s_reent;
}

/* Threading lock stubs — no-ops since GBA is single-threaded */
void __libc_lock_acquire_recursive(_LOCK_RECURSIVE_T *lock)
{
    (void)lock;
}

void __libc_lock_release_recursive(_LOCK_RECURSIVE_T *lock)
{
    (void)lock;
}

/* Reentrant syscall stubs — all set errno and return failure */
int _close_r(struct _reent *r, int fd)
{
    (void)fd;
    r->_errno = ENOTSUP;
    return -1;
}

int _fstat_r(struct _reent *r, int fd, struct stat *st)
{
    (void)fd;
    (void)st;
    r->_errno = ENOTSUP;
    return -1;
}

int _getpid_r(struct _reent *r)
{
    (void)r;
    return 1;
}

int _isatty_r(struct _reent *r, int fd)
{
    (void)fd;
    r->_errno = ENOTSUP;
    return 0;
}

int _kill_r(struct _reent *r, int pid, int sig)
{
    (void)pid;
    (void)sig;
    r->_errno = ENOTSUP;
    return -1;
}

long _lseek_r(struct _reent *r, int fd, long offset, int whence)
{
    (void)fd;
    (void)offset;
    (void)whence;
    r->_errno = ENOTSUP;
    return -1;
}

long _read_r(struct _reent *r, int fd, void *buf, unsigned long cnt)
{
    (void)fd;
    (void)buf;
    (void)cnt;
    r->_errno = ENOTSUP;
    return -1;
}

long _write_r(struct _reent *r, int fd, const void *buf, unsigned long cnt)
{
    (void)fd;
    (void)buf;
    (void)cnt;
    r->_errno = ENOTSUP;
    return -1;
}
