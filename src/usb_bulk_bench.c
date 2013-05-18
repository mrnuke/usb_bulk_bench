/*
 * usb_bulk_bench - Bulk endpoint usb transfer benchmark
 *
 * Copyright (C) 2013 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>

#define MAX_QUEUE_DEPTH		32

struct bench_cfg {
	uint16_t pid;
	uint16_t vid;
	uint8_t ep;
	bool dir_out;
	bool async;
	uint8_t queue;
};

static void print_copyright(void)
{
	printf("\n");
	printf("  usb_bulk_bench - Bulk endpoint usb transfer benchmark\n\n");
	printf("  Copyright (C) 2013 Alexandru Gagniuc\n\n"
	"This program is free software: you can redistribute it and/or modify\n"
	"it under the terms of the GNU General Public License as published by\n"
	"the Free Software Foundation, either version 3 of the License, or\n"
	"(at your option) any later version.\n\n"
	"This program is distributed in the hope that it will be useful,\n"
	"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
	"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
	"GNU General Public License for more details.\n");	
}

int main(int argc, char *argv[])
{
	int option_index = 0;
	int opt, ret;
	unsigned int svid = 0, spid = 0, queue_depth = 0, endpoint = 0;
	bool is_IN = false, is_OUT = false, is_sync = false, is_async = false,
		has_device = false, has_queue_d = false, has_ep = false;
	struct bench_cfg *config;

	struct option long_options[] = {
		{"copyright",	no_argument,	   0, 'c'},
		{"device",	required_argument, 0, 'd'},
		{"endpoint",	required_argument, 0, 'e'},
		{"IN",		no_argument,       0, 'I'},
		{"OUT",		no_argument,       0, 'O'},
		{"sync",	no_argument,       0, 's'},
		{"async",	no_argument,       0, 'a'},
		{"queue-depth",	required_argument, 0, 'q'},
		{0,		0,		   0,  0 }
	};

	config = (void*)malloc(sizeof(*config));
	memset(config, 0, sizeof(*config));

	/*
	 * Parse arguments
	 */
	while(1) {
		opt = getopt_long(argc, argv, "cd:e:IOsaq:",
				  long_options, &option_index);

		if (opt == EOF)
			break;

		switch (opt) {
		case 'c':
			print_copyright();
			exit(0);
			break;
		case 'd':
			/* VIP:PID: see if it makes sense */
			ret = sscanf(optarg, "%x:%x", &svid, &spid);
			if (ret != 2) {
				printf("Invalid argument: %s\n", optarg);
				exit(EXIT_FAILURE);
			}
			has_device = true;
			break;
		case 'e':
			ret = sscanf(optarg, "%u", &endpoint);
			if (ret != 1) {
				printf("Invalid argument: %s\n", optarg);
				exit(EXIT_FAILURE);
			}
			has_ep = true;
			break;
		case 'I':
			is_IN = true;
			break;
		case 'O':
			is_OUT = true;
			break;
		case 's':
			is_sync = true;
			break;
		case 'a':
			is_async = true;
			break;
		case 'q':
			ret = sscanf(optarg, "%u", &queue_depth);
			if (ret != 1) {
				printf("Invalid argument: %s\n", optarg);
				exit(EXIT_FAILURE);
			}
			has_queue_d = true;
			break;
		default:
			break;
		}
	}

	/*
	 * Sanity-check arguments
	 */
	
	if (!has_device) {
		printf("No device specified.\n");
		exit(EXIT_FAILURE);
	}
	if ((svid > 0xffff) || (spid > 0xffff)) {
		printf("Invalid device %.4x:%.4x\n", svid, spid);
		exit(EXIT_FAILURE);
	}
	config->vid = svid;
	config->pid = spid;

	if (is_IN && is_OUT) {
		printf("Cannot do both IN and OUT benchmarks.\n");
		exit(EXIT_FAILURE);
	}

	if (is_sync && is_async) {
		printf("Cannot do both sync and async benchmarks.\n");
		exit(EXIT_FAILURE);
	}

	if (!has_ep) {
		printf("No endpoint specified.\n");
		exit(EXIT_FAILURE);
	}

	if (endpoint == 0) {
		printf("Cannot benchmark on the control endpoint.\n");
		exit(EXIT_FAILURE);
	}
	if (endpoint > 0xf) {
		printf("Invalid endpoint.\n");
		exit(EXIT_FAILURE);
	}
	config->ep = endpoint;

	/* Default to a queue depth of 2 */
	if (is_async && !has_queue_d)
		queue_depth = 2;
	if (is_async && (queue_depth > MAX_QUEUE_DEPTH) ){
		printf("Queue depth is too damn high!!!");
		exit(EXIT_FAILURE);
	}
	if (is_async && (queue_depth == 0) ){
		printf("Invalid queue depth");
		exit(EXIT_FAILURE);
	}
	config->queue = queue_depth;

	/* Default to synchronous mode */
	config->async = is_async;
	/* Default to IN transfers */
	config->dir_out = is_OUT;

	/*
	 * At this point, the arguments are sane.
	 */

	printf("  usb_bulk_bench  Copyright (C) 2013 Alexandru Gagniuc\n\n"
	"This program comes with ABSOLUTELY NO WARRANTY;\n"
	"This is free software, and you are welcome to redistribute it\n"
	"under certain conditions; invoke with `-c' for details.\n" );

	printf("Benchmarking device: %.4x:%.4x\n", config->vid, config->pid);
	printf("\tEndpoint: %i %s\n", config->ep, config->dir_out?"OUT":"IN");
	if (config->async)
		printf("\tAsynchronous requests with queue depth of %i\n",
		       config->queue);
	else
		printf("\tSynchronous requests\n");

	return EXIT_SUCCESS;
}
