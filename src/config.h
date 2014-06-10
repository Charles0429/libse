#ifndef _CONFIG_H
#define _CONFIG_H

#ifdef	__linux__
#define EVENT_HAVE_EPOLL 1
#endif

#if (defined(__APPLE__) && defined(MAC_OS_X_VERSION_10_6)) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined (__NetBSD__)
#define EVENT_HAVE_KQUEUE 1
#endif

#if ( !(defined(EVENT_HAVE_POLL) || defined(EVENT_HAVE_KQUEUE)) )
#define EVENT_HAVE_SELECT
#endif

#endif/*end _CONFIG_H*/
