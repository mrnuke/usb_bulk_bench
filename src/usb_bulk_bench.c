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
#include <libusb.h>
#include <sys/time.h>

#define MAX_QUEUE_DEPTH		32
#define TRANSFER_TIMEOUT	1000
#define ARRAY_SIZE(x)		(sizeof(x)/sizeof(x[0]))

struct bench_cfg {
	uint16_t pid;
	uint16_t vid;
	uint8_t ep;
	bool dir_out;
	bool async;
	uint8_t queue;
	uint32_t size;
};

int do_benchmark(struct bench_cfg *conf);

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
	unsigned int svid = 0, spid = 0, queue_depth = 0, endpoint = 0,
		     transfer_size = 0;
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
		{"transfer-size",required_argument,0, 't'},
		{0,		0,		   0,  0 }
	};

	config = (void*)malloc(sizeof(*config));
	memset(config, 0, sizeof(*config));

	/*
	 * Parse arguments
	 */
	while(1) {
		opt = getopt_long(argc, argv, "cd:e:IOsaq:t:",
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
		case 't':
			ret = sscanf(optarg, "%u", &transfer_size);
			if (ret != 1) {
				printf("Invalid argument: %s\n", optarg);
				exit(EXIT_FAILURE);
			}
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

	/* Default to a transfer size of 64 bytes */
	config->size = (transfer_size ? transfer_size : 0);

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

	return do_benchmark(config);
}

inline double get_time()
{
	struct timespec timer;
	clock_gettime(CLOCK_MONOTONIC, &timer);
	return (double)timer.tv_sec + (double)(timer.tv_nsec)/1E9;
}

static int do_sync_bench(libusb_device_handle *handle, unsigned char ep,
			 uint32_t transz)
{
	int ret, len;
	uint8_t *buf;
	double start_nsec, end_nsec, speed;

	if ((buf = malloc(transz)) == NULL) {
		printf("Cannot allocate buffer\n");
		return EXIT_FAILURE;
	}

	start_nsec = get_time();
	while (1) {
		ret = libusb_bulk_transfer(handle, ep, buf, transz, &len,
					   TRANSFER_TIMEOUT);
		if (ret != LIBUSB_SUCCESS) {
			printf("Bulk transfer error: %s\n",
			       libusb_error_name(ret));
			return EXIT_FAILURE;
		}
		end_nsec = get_time();

		speed = (double)len / (end_nsec-start_nsec);
		printf("\rSpeed %.1f KiB/s", speed/1024 );
		start_nsec = end_nsec;
		fflush(stdout);
	}

	/* Never reached */
	free(buf);
}

static size_t qd;
static double *starttimes;
static double *speeds;

static void update_timing(size_t idx, int datalen)
{
	size_t i;
	double endtime, time, avg_speed;

	endtime = get_time();
	time = endtime - starttimes[idx];
	starttimes[idx] = endtime;
	speeds[idx] = (double)datalen/time;

	avg_speed = 0.0;
	for (i = 0; i < qd; i++) {
		avg_speed += speeds[i];
	}

	printf("\rSpeed %.1f KiB/s", avg_speed/1024 );
	fflush(stdout);
}

void async_cb(struct libusb_transfer *transfer)
{
	int ret;

	/* A failed transfer can mess up the timings, so halt if we meet one */
	if (transfer->status != LIBUSB_TRANSFER_COMPLETED)
	{
		printf("Transfer failed: %s\n",
		       libusb_error_name(transfer->status));
		exit(EXIT_FAILURE);
	}

	/* The meat and potatoes */
	update_timing((size_t)transfer->user_data, transfer->actual_length);

	/* Keep running, Forrest */
	ret = libusb_submit_transfer(transfer);
	if (ret != LIBUSB_SUCCESS) {
		printf("Error resubmitting transfer: %s\n",
		       libusb_error_name(ret));
		exit(EXIT_FAILURE);
	}
}


static int do_async_bench(libusb_context *ctx, libusb_device_handle *handle,
			  unsigned char ep, uint32_t transz, uint8_t depth)
{
	int ret;
	size_t i;
	uint8_t *buf, *curr_buf;;
	struct libusb_transfer **transfers;

	if ((buf = malloc(transz * depth)) == NULL) {
		printf("Cannot allocate buffers\n");
		return EXIT_FAILURE;
	}

	if ((transfers = malloc(depth * sizeof(*transfers))) == NULL) {
		printf("Cannot allocate transfers\n");
		return EXIT_FAILURE;
	}

	if ((starttimes = malloc(depth * sizeof(*starttimes))) == NULL) {
		printf("Cannot allocate stuff\n");
		return EXIT_FAILURE;
	}

	if ((speeds = malloc(depth * sizeof(*speeds))) == NULL) {
		printf("Cannot allocate stuff\n");
		return EXIT_FAILURE;
	}
	memset(speeds, 0, depth * sizeof(*speeds));

	/* update_timing() needs this global. */
	qd = depth;

	/*
	 * Submit initial transfers
	 * The transfers will re-submit themselves when completed
	 */
	for (i = 0; i < depth; i++)
	{
		transfers[i] = libusb_alloc_transfer(0);
		curr_buf = buf + transz * i;
		libusb_fill_bulk_transfer(transfers[i], handle, ep, curr_buf,
					  transz, async_cb, (void*)i,
					  TRANSFER_TIMEOUT);
		starttimes[i] = get_time();
		ret = libusb_submit_transfer(transfers[i]);
		if (ret != LIBUSB_SUCCESS) {
			printf("Error submitting transfer: %s\n",
			       libusb_error_name(ret));
			return EXIT_FAILURE;
		}
	}

	/*
	 * Run forever
	 */
	while (1) {
		ret =  libusb_handle_events(ctx);
		if (ret != LIBUSB_SUCCESS) {
			printf("Error: %s\n", libusb_error_name(ret));
			return EXIT_FAILURE;
		}
	}

	/* Never reached */
	return EXIT_SUCCESS;
}


int do_benchmark(struct bench_cfg *conf)
{
	int ret;
	unsigned char ep;
	libusb_context *ctx;
	libusb_device_handle *handle;

	if ((ret= libusb_init(&ctx)) != LIBUSB_SUCCESS) {
		printf("Initialization error: %s\n", libusb_error_name(ret));
		return EXIT_FAILURE;
	}

	handle = libusb_open_device_with_vid_pid(ctx, conf->vid, conf->pid);
	if (handle == NULL) {
		printf("Cannot open device.\n");
		return EXIT_FAILURE;
	}

	if ((ret = libusb_claim_interface(handle, 0)) != LIBUSB_SUCCESS) {
		printf("Cannot claim device: %s\n", libusb_error_name(ret));
	}

	ep = conf->ep;
	ep |= (conf->dir_out ? LIBUSB_ENDPOINT_OUT : LIBUSB_ENDPOINT_IN);

	if (conf->async)
		return do_async_bench(ctx,handle, ep, conf->size, conf->queue);
	else
		return do_sync_bench(handle, ep, conf->size);

	return EXIT_SUCCESS;
}
