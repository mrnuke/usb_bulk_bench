# USB device bulk transfer benchmark #

## About ##

usb_dev_bulk is a utility for measuring the performance of USB bulk transfers to
and from any USB device. It was originally written to test the performance of
libopencm3 drivers for the USB stack.

## Usage ##
* -c | --copyright: -- print license and exit
* -d | --device -- device to test, in the form VID:PID (i.e. c03e:b007)
* -e | --endpoint -- endpoint to use for bulk transfers
* -I | --IN -- use IN transfers (default)
* -O | --OUT -- use OUT transfers
* -s | --sync -- run transfers synchronously (default)
* -a | --async -- run transfers asynchronously
* -q | --queue-depth -- number of simultaneous transfers to issue in async mode
* -t | --transfer-size -- length of each transfer

### Examples ###

Asynchronous EP2IN, with 1024 byte transfers and a queue depth of 32
> $ usb_bulk_bench  -d c03e:b007 -e 2 -I -a -t 1024 -q 32

Synchronous EP2IN, with 1024 byte transfers
> $ usb_bulk_bench  -d c03e:b007 -e 3 -I -s -t 1024

## Building ##

### Prerequisites ###
* gcc or any working compiler
* cmake
* libusb

### Working with cmake ###

1. Create a build directory and cd into it:
> $ mkdir build; cd build

1. Now tell cmake to crate the build system, pointing it to the source directory:
> $ cmake ..

1. Compile:
> & make

1. Profit
