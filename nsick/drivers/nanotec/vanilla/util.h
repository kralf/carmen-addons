/* 
 * Copyright (C) 2005 Roland Philippsen <roland dot philippsen at gmx dot net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#ifndef NANOTEC_UTIL_H
#define NANOTEC_UTIL_H

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
  
typedef enum {
  noparity,
  evenparity,
  oddparity
} nanotec_util_parity_t;
    
/** \return -1 if nanotec_util_serial_open() failed, -2 if
    nanotec_util_serial_setbaud() failed, a valid file descriptor on
    success. */
int nanotec_util_serial_open_cfg(const char * device, unsigned long baud,
  size_t nbits, size_t stopbits, nanotec_util_parity_t parity);

/** \return -1 if open() failed, a valid file descriptor on
    success. */
int nanotec_util_serial_open(const char * device);

/** \return -1 if memset() failed, -2 if invalid baudrate, -3 if
    invalid nbits, -4 if invalid stopbits, -5 if invalid parity, -6
    if tcflush() failed, -7 if tcsetattr() failed, 0 on success. */
int nanotec_util_serial_cfg(int fd, unsigned long baud,
        /** 5, 6, 7, or 8 bits per character */
        size_t nbits,
        /** 1 or 2 stop bits between characters */
        size_t stopbits, nanotec_util_parity_t parity);

/** \return -1 if tcdrain() failed, -2 if tcflush() failed, -3 if
    close() failed, 0 on success. */
int nanotec_util_serial_close(int fd);

/** \return -1 if socket() failed, -2 if hostinfo() failed, -3 if
    connect() failed, a valid file descriptor on success. */
int nanotec_util_tcp_open(uint32_t portnum, const char * server);

/** -1 if close() failed, 0 on success. */
int nanotec_util_tcp_close(int fd);

/** \return -1 if write() failed, 0 on success. */
int nanotec_util_buffer_write(int fd, const uint8_t * buffer, ssize_t n_bytes,
  ssize_t * n_done, FILE * dbg);

/** \return -1 if read() failed, 0 on success. */
int nanotec_util_buffer_read(int fd, uint8_t * buffer, ssize_t n_bytes,
  ssize_t * n_done, int timeout, FILE * dbg);

/**
    Set the function for cleaning up after your program. Also sets up
    signal handlers for SIGINT, SIGHUP, and SIGTERM to call that ceanup
    function.

    \note If you use this function, you should put your cleanup all
    into the function passed as argument and NOT call that function
    yourself. It is automatically called upon calls to exit() or return
    from main.

    \return -1 if atexit() failed, -2 if signal(SIGINT) failed, -3 if
    signal(SIGHUP) failed, -4 if signal(SIGTERM) failed, 0 on
    success.
*/
int nanotec_util_set_cleanup(void (*function)());
    
#endif
