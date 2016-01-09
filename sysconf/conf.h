/*-
 * Copyright (c) 2013-2016 Devin Teske <dteske@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FrauBSD: sysconf/conf.h 2016-01-09 13:08:21 -0800 freebsdfrau $
 * $FreeBSD$
 */

#ifndef _CONF_H_
#define _CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <figpar.h>
#include <sys/queue.h>

/*
 * Anatomy of a config file option.
 */
struct option {
	SLIST_ENTRY(option) entries;

	char *name;	/* option name */
	char *value;	/* option value */
};

/* FreeBSD loader.conf(5) characteristics */
#define LOADER_DEFAULTS		"/boot/defaults/loader.conf"
#define ENV_LOADER_DEFAULTS	"LOADER_DEFAULTS"
#define VAR_LOADER_CONF_FILES	"loader_conf_files"

/* FreeBSD sysctl.conf(5) characteristics */
#define SYSCTL_CONF		"/etc/sysctl.conf"
#define SYSCTL_CONF_LOCAL	"/etc/sysctl.conf.local"

/* Environment for this utility */
#define ENV_SYS_CONFS	"SYS_CONFS"

/* Function prototypes */
struct option *	sysconf_get_option(const char *directive);
int		parse_sysconf_all(void);

#ifdef __cplusplus
}
#endif

#endif /* !_CONF_H_ */
