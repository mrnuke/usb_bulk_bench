##
##  usb_bulk_bench - Bulk endpoint usb transfer benchmark
##
##  Copyright (C) 2013 Alexandru Gagniuc <mr.nuke.me@gmail.com>
##
##  This program is free software: you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation, either version 3 of the License, or
##  (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##  along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

cmake_minimum_required(VERSION 2.8)

project(usb_bulk_bench)

include(FindPkgConfig)

#===============================================================================
#= Dependencies
#-------------------------------------------------------------------------------
find_package(PkgConfig)
pkg_check_modules(PKGDEPS REQUIRED libusb-1.0)

include_directories(${PKGDEPS_INCLUDE_DIRS})
link_directories(${PKGDEPS_LIBRARY_DIRS})

add_definitions(-Wall)
#===============================================================================
#= Sources and build
#-------------------------------------------------------------------------------
set(usb_bulk_bench_SOURCES
	src/usb_bulk_bench.c
)

add_executable(${PROJECT_NAME}
	${usb_bulk_bench_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
	${PKGDEPS_LIBRARIES}
)