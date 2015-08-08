/*-
 * Copyright (c) 2012 SHIMIZU Ryo <ryo@nerv.org>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <paths.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <util.h>
#include <errno.h>
#include <string.h>
#include <err.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

static void
usage(void)
{
	fprintf(stderr, "usage: ttyconsole [-sd]\n");
	fprintf(stderr, "	-s	silent discard\n");
	fprintf(stderr, "	-d	daemon mode\n");
	exit(1);
}

int
main(int argc, char *argv[])
{
	struct stat sb;
	ssize_t bufsize, size;
	void *buf;
	const int on = 1;
	int rc;
	int pty_fd, tty_fd;
	int ch, dflag, sflag;

	dflag = sflag = 0;
	while ((ch = getopt(argc, argv, "ds")) != -1) {
		switch (ch) {
		case 'd':
			dflag = 1;
			break;
		case 's':
			sflag = 1;
			break;
		default:
			usage();
		}
	}

	if (dflag)
		daemon(0, 1);

	rc = openpty(&pty_fd, &tty_fd, NULL, NULL, NULL);
	if (rc < 0)
		err(EXIT_FAILURE, "openpty");

	if (ioctl(tty_fd, TIOCCONS, &on) == -1)
		err(EXIT_FAILURE, "ioctl: TIOCCONS");

	if (fstat(tty_fd, &sb) == -1)
		err(EXIT_FAILURE, "fstat");

	bufsize = sb.st_blksize ? sb.st_blksize : 1024 * 64;
	buf = malloc(bufsize);
	if (buf == NULL)
		err(EXIT_FAILURE, NULL);

	for (;;) {
		size = read(pty_fd, buf, bufsize);
		if (size < 0) {
			perror("read");
			break;
		}

		if (!sflag) {
			size = write(STDOUT_FILENO, buf, size);
			if (size < 0) {
				perror("write");
				break;
			}
		}
	}

	return EXIT_FAILURE;
}
