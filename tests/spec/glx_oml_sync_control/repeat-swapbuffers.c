/*
 * Copyright © 2014 The TOVA Company
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/**
 * \file repeat-swapbuffers.c
 * Verifies that glXSwapBuffersMscOML does not increment SBC more often than
 * MSC
 */

#include "piglit-util-gl-common.h"
#include "piglit-glx-util.h"
#include "common.h"

static int64_t target_msc_delta = 0;
static const int64_t divisor = 0;
static const int64_t msc_remainder = 0;
static const unsigned int loops = 5;


struct omlTriple {
	int64_t ust;
	int64_t msc;
	int64_t sbc;
};

static enum piglit_result
draw(Display *dpy)
{
	struct omlTriple old = {.ust = 0xd0, .msc = 0xd0, .sbc = 0xd0};
	struct omlTriple new = {.ust = 0xd0, .msc = 0xd0, .sbc = 0xd0};
	int64_t target_sbc = 0;
	int i;

	piglit_set_timeout(5, PIGLIT_FAIL);

	if (!glXGetSyncValuesOML(dpy, win, &old.ust, &old.msc, &old.sbc)) {
		fprintf(stderr, "Initial glXGetSyncValuesOML failed\n");
		return PIGLIT_FAIL;
	}

	old.msc -= 1;
	for (i = 0; i < loops; ++i) {
		int64_t target_msc = 0;
		glClearColor(0.0, 1.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		if (target_msc_delta) {
			target_msc = old.msc + target_msc_delta;
		}

		target_sbc = glXSwapBuffersMscOML(dpy, win, target_msc,
				divisor, msc_remainder);
		if (target_sbc <= 0) {
			fprintf(stderr, "glXSwapBuffersMscOML failed\n");
			return PIGLIT_FAIL;
		}
		if (!glXGetSyncValuesOML(dpy, win, &new.ust, &new.msc, &new.sbc))
		{
			fprintf(stderr, "glXGetSyncValuesOML failed\n");
			return PIGLIT_FAIL;
		}

		if (new.sbc - old.sbc > new.msc - old.msc) {
			fprintf(stderr,
				"SBC incremented more than once per msc\n");
			return PIGLIT_FAIL;
		}
		if (new.sbc > old.sbc) {
			old = new;
		}
	}
	return PIGLIT_PASS;
}

static unsigned int
parse_num_arg(int argc, char **argv, int j)
{
	char *ptr;
	unsigned int val;

	if (j >= argc) {
		fprintf(stderr, "%s requires an argument\n", argv[j - 1]);
		piglit_report_result(PIGLIT_FAIL);
	}

	val = strtoul(argv[j], &ptr, 0);
	if (!val || *ptr != '\0') {
		fprintf(stderr, "%s requires an argument\n", argv[j - 1]);
		piglit_report_result(PIGLIT_FAIL);
	}

	return val;
}

int
main(int argc, char **argv)
{
	int j;
	for (j = 1; j < argc; ++j) {
		if (!strcmp(argv[j], "-msc-delta")) {
			++j;
			target_msc_delta = parse_num_arg(argc, argv, j);
		} else {
			fprintf(stderr, "unsupported option %s\n", argv[j]);
			piglit_report_result(PIGLIT_FAIL);
		}
	}
	piglit_automatic = true;
	piglit_oml_sync_control_test_run(false, draw);

	return 0;
}
