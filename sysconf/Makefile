# $FrauBSD: sysconf/Makefile 2016-01-09 13:08:21 -0800 freebsdfrau $
# $FreeBSD$

PROG=	sysconf
SRCS=	sysconf.c conf.c

MAN=	sysconf.8

# Use cc for now, because we plan to MFC to stable/9 (but test with clang)
#CC=		clang
WARNS?=		3
CFLAGS+=	-I. -I${.CURDIR} -g
LDFLAGS+=	-lfigpar

.include <bsd.prog.mk>
