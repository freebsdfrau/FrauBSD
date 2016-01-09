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
 */

#include <sys/cdefs.h>
#ifdef __FrauBSD__
__FBSDID("$FrauBSD: sysconf/conf.c 2016-01-09 13:08:21 -0800 freebsdfrau $");
#else
__FBSDID("$FreeBSD$");
#endif

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <figpar.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "conf.h"

/* Data to process */
static SLIST_HEAD(slisthead, option) head = SLIST_HEAD_INITIALIZER(head);

/* Function prototypes for private functions (see style(9)) */
int setoid(struct fp_config *option, uint32_t line, char *directive,
	char *value);
int sysconf_sanitize_value(char **value);

/*
 * Function to sanitize the system configuration value prior to strdup(3) for
 * storage into the option structure. The rules for sanitizing the value come
 * from determining how the Forth layer interprets loader.conf(5). For the sake
 * of consistency, we make sysctl.conf(5) play by the same rules.
 */
int
sysconf_sanitize_value(char **value)
{
	size_t len;
	char *cp;

	/* return if either handle, pointer, or string is NULL */
	if (value == NULL || *value == NULL || (len = strlen(*value)) == 0)
		return 0;

	cp = *value;
	if (*cp == '"') {
		/* support.4th will produce an error and not set the variable
		 * if the last character is not a quote. */
		if (len > 1 && cp[len-1] != '"') return -1;

		/* Trim the leading quote by writing a new address to the
		 * handle and trim the trailing quote by writing a NUL-byte
		 * into the string */
		value[0]++;
		cp[len-1] = '\0';
	} else {
		/* support.4th will produce an error and not set the variable
		 * unless all characters are alnum, underscore, or dot */
		while (*cp != '\0') {
			if (isdigit(*cp) || isalpha(*cp) || *cp == '_') {
				cp++;
				continue;
			}
			return -1;
		}
	}

	return 0;
}

/*
 * figpar call-back for variables
 */
int
setoid(struct fp_config *option __unused, uint32_t line, char *directive,
    char *value)
{
	struct option *opt, *opt_new;

	/* Check/modify the variable against known rules */
	if (sysconf_sanitize_value(&value) != 0)
		return -1; /* stop processing at first dirty value */

	/* Is there already an entry with this name? */
	SLIST_FOREACH(opt, &head, entries) {
		if (!strcmp(opt->name, directive)) {
			/* Reuse this entry */
			free(opt->value);
			opt->value = strdup(value);
			return 0;
		}
	}

	opt_new = (struct option *)malloc(sizeof(struct option));
	opt_new->name = strdup(directive);
	opt_new->value = strdup(value);

	/* Add to option list */
	SLIST_INSERT_HEAD(&head, opt_new, entries);
	
	return 0;
}

/*
 * Parse the following system configuation files (in order of appearance):
 *
 * 	+ $LOADER_DEFAULTS (default `/boot/defaults/loader.conf')
 * 	+ loader_conf_files (defined in $LOADER_DEFAULTS)
 * 		- Default "/boot/loader.conf /boot/loader.conf.local"
 * 	+ /etc/sysctl.conf
 * 	+ /etc/sysctl.conf.local
 *
 * Returns zero on success, -1 on failure (and errno should be consulted). No
 * error is returned if any of `loader_conf_files' or `/etc/sysctl.conf.local'
 * exists, but an error _is_ returned if $LOADER_DEFAULTS or `/etc/sysctl.conf'
 * does not exist.
 */
int
parse_sysconf_all(void)
{
	int res, processing_options = 0x0;
	char *cp, path[PATH_MAX];

	SLIST_INIT(&head);

	/* Allow $LOADER_DEFAULTS to override `/boot/defaults/loader.conf' */
	if ((cp = getenv(ENV_LOADER_DEFAULTS)) != NULL && *cp != '\0')
		snprintf(path, PATH_MAX, "%s", cp);
	else
		snprintf(path, PATH_MAX, "%s", LOADER_DEFAULTS);

	/* Set figpar processing options */
	processing_options |= FP_REQUIRE_EQUALS;
	processing_options |= FP_BREAK_ON_EQUALS;
	processing_options |= FP_CASE_SENSITIVE;

	/*
	 * Process first file (either $LOADER_DEFAULTS if set, or
	 * `/boot/defaults/loader.conf')
	 */
	if ((res = parse_config(NULL, path, setoid, processing_options)) != 0)
		if (errno != 0) warn("%s", path);

	return res;
}

/*
 * Return a pointer to the system configuration option specific to `directive'
 * or a pointer to a dummy struct if none found.
 */
struct option *
sysconf_get_option(const char *directive)
{
	struct option *opt;

	SLIST_FOREACH(opt, &head, entries)
		if (!strcmp(opt->name, directive)) return opt;

	return NULL;
}
