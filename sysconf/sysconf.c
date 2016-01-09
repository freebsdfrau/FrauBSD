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
__FBSDID("$FrauBSD: sysconf/sysconf.c 2016-01-09 13:08:21 -0800 freebsdfrau $");
#else
__FBSDID("$FreeBSD$");
#endif

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "conf.h"
#include "sysconf.h"

/* Data to process */
unsigned int nargs = 0;

/* Extra display information */
char *pgm;			/* set to argv[0] by main() */
uint8_t show_equals = FALSE;	/* `-e' */

/* Function prototypes for private functions (see style(9)) */
void help(void);
void usage(void);

/*
 * Program description here.
 */
int
main(int argc, char *argv[])
{
	int ch;

	pgm = argv[0]; /* store a copy of invocation name */

	/*
	 * Process command-line options
	 */
	while ((ch = getopt(argc, argv, "-aAcDef:FhinNqR:vx")) != -1)
	{
		switch(ch) {
		case '-': /* long option */
			if (strcmp(argv[optind], "--help") == 0) help();
			else {
				warnx("illegal option -- %s", argv[optind]);
				usage();
			}
			break;
		case 'a': /* all */
			break;
		case 'A': /* all including defaults */
			break;
		case 'c': /* check only */
			break;
		case 'D': /* defaults only */
			break;
		case 'e': /* show equals */
			show_equals = TRUE;
			break;
		case 'f': /* file */
			break;
		case 'h': /* help/usage */
			usage();
			break;
		case 'i': /* ignore unknown */
			break;
		case 'n': /* value only */
			break;
		case 'N': /* name only */
			break;
		case 'q': /* quiet */
			break;
		case 'R': /* root dir */
			break;
		case 'v': /* verbose */
			break;
		case 'x': /* remove */
			break;
		case '?': /* unknown argument (based on optstring) */
		default: /* unhandled argument (based on switch) */
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	/* Process the contents of system configuration files */
	if (parse_sysconf_all() != 0 && errno != 0) exit(EXIT_FAILURE);

	/* Process remaining arguments as list of names to display */
	{
		int n = 0;
		char *cp;
		struct option *opt;

		for (cp = argv[0]; n < argc; cp = argv[n++])
		{
			nargs++;
			if ((opt = sysconf_get_option(cp)) == NULL) continue;
			printf(show_equals ? "%s=\"%s\"\n" : "%s: %s\n",
			       cp, opt->value);
		}
	}

	/* Display usage and exit if not given at least one name */
	if (nargs == 0) {
		warnx("no names provided");
		usage();
	}

	exit(EXIT_SUCCESS);
}

/*
 * Print short usage statement to stderr and exit with error status.
 */
void
usage(void)
{
	fprintf(stderr, "Usage: %s [OPTIONS] name[=value]\n", pgm);
	fprintf(stderr, "Try `%s --help' for more information.\n", pgm);
	exit(EXIT_FAILURE);
}

/*
 * Print long usage statement to stderr and exit with error status.
 */
void
help(void)
{
	fprintf(stderr, "Usage: %s [OPTIONS] name[=value]\n", pgm);
	fprintf(stderr, "Usage: %s [OPTIONS] -a | -A\n", pgm);
	fprintf(stderr, "OPTIONS:\n");
#define OPTFMT "\t%-9s %s\n"
	fprintf(stderr, OPTFMT, "-a",
	    "Dump a list of all non-default configuration variables.");
	fprintf(stderr, OPTFMT, "-A",
	    "Dump a list of all configuration variables (incl. defaults).");
	fprintf(stderr, OPTFMT, "-c",
	    "Check. Return success if no changes needed, else error.");
	fprintf(stderr, OPTFMT, "-D",
	    "Show default value(s) only (this is the same as setting");
	fprintf(stderr, OPTFMT, "",
	    "SYS_CONFS to NULL or passing `-f' with a NULL file-argument).");
	fprintf(stderr, OPTFMT, "-e",
	    "Separate the name and value of variable(s) with `='.");
	fprintf(stderr, OPTFMT, "-f file",
	    "Operate on the specified file(s) instead of normal collection");
	fprintf(stderr, OPTFMT, "",
	    "of files. Specify multiple times for additional files.");
	fprintf(stderr, OPTFMT, "-F",
	    "Show only the last configuration file each name appears in.");
	fprintf(stderr, OPTFMT, "-h",
	    "Print a short usage statement to stderr and exit.");
	fprintf(stderr, OPTFMT, "--help",
	    "Print this message to stderr and exit.");
	fprintf(stderr, OPTFMT, "-i",
	    "Ignore unknown names.");
	fprintf(stderr, OPTFMT, "-n",
	    "Show only variable values, not their names.");
	fprintf(stderr, OPTFMT, "-N",
	    "Show only variable names, not their values.");
	fprintf(stderr, OPTFMT, "-q",
	    "Quiet. Reverse previous occurrences of `-v' flag.");
	fprintf(stderr, OPTFMT, "-R dir",
	    "Operate within the root directory `dir' rather than `/'.");
	fprintf(stderr, OPTFMT, "-v",
	    "Verbose. Print pathname of the specific configuration file");
	fprintf(stderr, OPTFMT, "",
	    "where the name was found.");
	fprintf(stderr, OPTFMT, "--version",
	    "Print version information to stdout and exit.");
	fprintf(stderr, OPTFMT, "-x",
	    "Remove name(s) from specified file(s).");
	fprintf(stderr, "\n");
	fprintf(stderr, "ENVIRONMENT:\n");
#define ENVFMT "\t%-15s %s\n"
	fprintf(stderr, ENVFMT, "SYS_CONFS",
	    "Override default configuration files (NULL allowed).");
	fprintf(stderr, ENVFMT, "LOADER_DEFAULTS",
	    "Location of `/boot/defaults/loader.conf' file.");
	exit(EXIT_FAILURE);
}
