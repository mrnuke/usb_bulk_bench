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
#include <getopt.h>

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
	int opt;
	
	static struct option long_options[] = {
		{"copyright",	no_argument,	0, 'c'},
		{0,		0,		0,  0 }
	};

	while(1) {
		opt = getopt_long(argc, argv, "c",
				  long_options, &option_index);

		if (opt == EOF)
			break;

		switch (opt) {
		case 'c':
			print_copyright();
			exit(0);
			break;
		default:
			exit(0);
			break;
		}
	}
	
	printf("  usb_bulk_bench  Copyright (C) 2013 Alexandru Gagniuc\n\n"
	"This program comes with ABSOLUTELY NO WARRANTY;\n"
	"This is free software, and you are welcome to redistribute it\n"
	"under certain conditions; invoke with `-c' for details.\n" );
}