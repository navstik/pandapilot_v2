/****************************************************************************
 *
 *   Copyright (C) 2012 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file mixer.c
 *
 * Mixer utility.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <nuttx/compiler.h>

#include <systemlib/err.h>
#include <drivers/drv_mixer.h>
#include <uORB/topics/actuator_controls.h>

__EXPORT int mixer_main(int argc, char *argv[]);

static void	usage(const char *reason) noreturn_function;
static void	load(const char *devname, const char *fname) noreturn_function;

int
mixer_main(int argc, char *argv[])
{
	if (argc < 2)
		usage("missing command");

	if (!strcmp(argv[1], "load")) {
		if (argc < 4)
			usage("missing device or filename");

		load(argv[2], argv[3]);
	}

	usage("unrecognised command");
}

static void
usage(const char *reason)
{
	if (reason)
		fprintf(stderr, "%s\n", reason);

	fprintf(stderr, "usage:\n");
	fprintf(stderr, "  mixer load <device> <filename>\n");
	/* XXX other useful commands? */
	exit(1);
}

static void
load(const char *devname, const char *fname)
{
	int		dev;
	FILE		*fp;
	char		line[120];
	char		buf[2048];

	/* open the device */
	if ((dev = open(devname, 0)) < 0)
		err(1, "can't open %s\n", devname);

	/* reset mixers on the device */
	if (ioctl(dev, MIXERIOCRESET, 0))
		err(1, "can't reset mixers on %s", devname);

	/* open the mixer definition file */
	fp = fopen(fname, "r");
	if (fp == NULL)
		err(1, "can't open %s", fname);

	/* read valid lines from the file into a buffer */
	buf[0] = '\0';
	for (;;) {

		/* get a line, bail on error/EOF */
		line[0] = '\0';
		if (fgets(line, sizeof(line), fp) == NULL)
			break;

		/* if the line doesn't look like a mixer definition line, skip it */
		if ((strlen(line) < 2) || !isupper(line[0]) || (line[1] != ':'))
			continue;

		/* compact whitespace in the buffer */
		char *t, *f;
		for (f = buf; *f != '\0'; f++) {
			/* scan for space characters */
			if (*f == ' ') {
				/* look for additional spaces */
				t = f + 1;
				while (*t == ' ')
					t++;
				if (*t == '\0') {
					/* strip trailing whitespace */
					*f = '\0';
				} else if (t > (f + 1)) {
					memmove(f + 1, t, strlen(t) + 1);
				}
			}
		}

		/* if the line is too long to fit in the buffer, bail */
		if ((strlen(line) + strlen(buf) + 1) >= sizeof(buf))
			break;

		/* add the line to the buffer */
		strcat(buf, line);
	}

	/* XXX pass the buffer to the device */
	int ret = ioctl(dev, MIXERIOCLOADBUF, (unsigned long)buf);

	if (ret < 0)
		err(1, "error loading mixers from %s", fname);
	exit(0);
}
