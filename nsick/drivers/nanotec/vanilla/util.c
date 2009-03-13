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

#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>

#include "util.h"

int nanotec_util_serial_open_cfg(const char * device, unsigned long baud,
  size_t nbits, size_t stopbits, nanotec_util_parity_t parity) {
  const int fd = nanotec_util_serial_open(device);
  if(fd == -1)
    return -1;
  if(0 != nanotec_util_serial_cfg(fd, baud, nbits, stopbits, parity)){
    close(fd);
    return -2;
  }
  return fd;
}

int nanotec_util_serial_open(const char * device) {
  const int fd = open(device, O_RDWR | O_NDELAY);
  if(fd == -1){
    fprintf(stderr, "nanotec_util_serial_open: ");
    perror(device);
    return -1;
  }
  return fd;
}

int nanotec_util_serial_cfg(int fd, unsigned long baud, size_t nbits,
  size_t stopbits, nanotec_util_parity_t parity) {
  struct termios tio;
  if(memset(&tio, 0, sizeof(struct termios)) != &tio){
    perror("nanotec_util_serial_setbaud: memset");
    return -1;
  }
  
  switch(baud){
  case 50L    : tio.c_cflag |= B50;     break;
  case 75L    : tio.c_cflag |= B75;     break;
  case 110L   : tio.c_cflag |= B110;    break;
  case 134L   : tio.c_cflag |= B134;    break;
  case 150L   : tio.c_cflag |= B150;    break;
  case 200L   : tio.c_cflag |= B200;    break;
  case 300L   : tio.c_cflag |= B300;    break;
  case 600L   : tio.c_cflag |= B600;    break;
  case 1200L  : tio.c_cflag |= B1200;   break;
  case 1800L  : tio.c_cflag |= B1800;   break;
  case 2400L  : tio.c_cflag |= B2400;   break;
  case 4800L  : tio.c_cflag |= B4800;   break;
  case 9600L  : tio.c_cflag |= B9600;   break;
  case 19200L : tio.c_cflag |= B19200;  break;
  case 38400L : tio.c_cflag |= B38400;  break;
  case 57600L : tio.c_cflag |= B57600;  break;
  case 115200L: tio.c_cflag |= B115200; break;
  case 230400L: tio.c_cflag |= B230400; break;
  default:
    fprintf(stderr, "nanotec_util_serial_setbaud: invalid baudrate %lu\n",
      baud);
    return -2;
  }
  
  switch(nbits){
  case 5: tio.c_cflag |= CS5; break;
  case 6: tio.c_cflag |= CS6; break;
  case 7: tio.c_cflag |= CS7; break;
  case 8: tio.c_cflag |= CS8; break;
  default:
    fprintf(stderr, "nanotec_util_serial_setbaud: invalid nbits %zd\n", nbits);
    return -3;
  }
  
  switch(stopbits){
  case 1: break;
  case 2: tio.c_cflag |= CSTOPB; break;
  default:
    fprintf(stderr, "nanotec_util_serial_setbaud: invalid stopbits %zd\n",
      stopbits);
    return -4;
  }
  
  switch(parity){
  case noparity: break;
  case evenparity: tio.c_cflag |= PARENB; break;
  case oddparity:  tio.c_cflag |= PARENB | PARODD; break;
  default:
    fprintf(stderr, "nanotec_util_serial_setbaud: invalid parity %d\n",
      parity);
    return -5;
  }
  
  // CLOCAL: ignore modem status lines
  tio.c_cflag |= CLOCAL;
  
  // IGNPAR: ignore (discard) parity errors
  // [see man termios(4) if you think you want to handle parity generically]
  tio.c_iflag = IGNPAR;
  
  if(tcflush(fd, TCIOFLUSH) < 0){
    perror("nanotec_util_serial_setbaud: tcflush");
    return -6;
  }
  
  if(tcsetattr(fd, TCSANOW, &tio) < 0){
    perror("nanotec_util_serial_setbaud: tcsetattr");
    return -7;
  }
  
  return 0;
}

int nanotec_util_serial_close(int filedescriptor) {
  if(tcdrain(filedescriptor) < 0){
    perror("nanotec_util_serial_close: tcdrain");
    return -1;
  }
  if(tcflush(filedescriptor, TCIOFLUSH) < 0){
    perror("nanotec_util_serial_close: tcflush");
    return -2;
  }
  if(close(filedescriptor) < 0){
    perror("nanotec_util_serial_close: close");
    return -3;
  }
  return 0;
}

int nanotec_util_buffer_write(int fd, const uint8_t * buffer, ssize_t n_bytes,
  ssize_t * n_done, FILE * dbg) {
  if(dbg != 0){
    ssize_t i;
    fprintf(dbg, "DEBUG nanotec_util_buffer_write():\n ");
    for(i = 0; i < n_bytes; ++i){
      fprintf(dbg, " %02X", buffer[i]);
      if(i % 4 == 3)
	fprintf(dbg, "  ");
      if(i % 16 == 15)
	fprintf(dbg, "\n ");
    }
    fprintf(dbg, "\n");
  }
  
  *n_done = 0;
  while(n_bytes > 0){
    ssize_t n;
    while((n = write(fd, buffer, n_bytes)) == 0);
    if(n < 0){
      perror("nanotec_util_buffer_write: write");
      return -1;
    }
    n_bytes   -= n;
    buffer    += n;
    (*n_done) += n;
  }
  
  return 0;
}

int nanotec_util_buffer_read(int fd, uint8_t * buffer, ssize_t n_bytes,
  ssize_t * n_done, int timeout, FILE * dbg) {
  ssize_t remain = n_bytes;
  uint8_t * bp = buffer;
  struct timeval t;
  fd_set set;
  int err;
  *n_done = 0;

  while(remain > 0) {
    t.tv_sec = 0;
    t.tv_usec = timeout*1e3;
    FD_ZERO(&set);
    FD_SET(fd, &set);
    err = select(fd+1, &set, NULL, NULL, &t);
    if (err == 0) {
      perror("nanotec_util_buffer_read: select timeout");
      return -2;
    }

    ssize_t n;
    n = read(fd, bp, remain);
    if ((n < 0) && (errno != EWOULDBLOCK)) {
      perror("nanotec_util_buffer_read: read");
      return -1;
    }
    else if (n > 0) {
      remain    -= n;
      bp        += n;
      (*n_done) += n;
    }
  }
  
  if(dbg != 0){
    ssize_t i;
    fprintf(dbg, "DEBUG buffer_read():\n ");
    for(i = 0; i < n_bytes; ++i){
      fprintf(dbg, " %02X", buffer[i]);
      if(i % 4 == 3)
	fprintf(dbg, "  ");
      if(i % 16 == 15)
	fprintf(dbg, "\n ");
    }
    fprintf(dbg, "\n");
  }
  
  return 0;
}

int nanotec_util_tcp_open(uint32_t portnum, const char * server) {
  int socket_fd;
  struct sockaddr_in name;
  struct hostent * hostinfo;
  
  socket_fd = socket(PF_INET, SOCK_STREAM, 0);
  if(socket_fd < 0){
    perror("nanotec_util_tcp_open: socket");
    return -1;
  }
  
  name.sin_family = AF_INET;
  hostinfo = gethostbyname(server);
  if(hostinfo == NULL){
    fprintf(stderr, "nanotec_util_tcp_open: couldn't get hostinfo for %s: ",
      server);
    switch(h_errno){
    case HOST_NOT_FOUND: fprintf(stderr, "host not found\n"); break;
    case NO_ADDRESS:     fprintf(stderr, "no address\n"); break;
    case NO_RECOVERY:    fprintf(stderr, "no recovery\n"); break;
    case TRY_AGAIN:      fprintf(stderr, "try again\n"); break;
    default:             fprintf(stderr, "unknown error\n");
    }
    return -2;
  }
  name.sin_addr = * ((struct in_addr *) hostinfo->h_addr);
  name.sin_port = htons(portnum);
  
  if(connect(socket_fd, (struct sockaddr *) & name, INET_ADDRSTRLEN) < 0){
    perror("nanotec_util_tcp_open: connect");
    return -3;
  }
  
  return socket_fd;
}

int nanotec_util_tcp_close(int fd) {
  if(close(fd) < 0){
    perror("nanotec_util_tcp_close: close");
    return -1;
  }
  return 0;
}

static void handle(int signum) {
  // The cleanup function is called implcitly through exit().
  exit(EXIT_SUCCESS);
}

int nanotec_util_set_cleanup(void (*function)()) {
  if(atexit(function)){
    perror("nanotec_util_set_cleanup(): atexit() failed");
    return -1;
  }
  if(signal(SIGINT, handle) == SIG_ERR){
    perror("nanotec_util_set_cleanup(): signal(SIGINT) failed");
    return -2;
  }
  if(signal(SIGHUP, handle) == SIG_ERR){
    perror("nanotec_util_set_cleanup(): signal(SIGHUP) failed");
    return -3;
  }
  if(signal(SIGTERM, handle) == SIG_ERR){
    perror("nanotec_util_set_cleanup(): signal(SIGTERM) failed");
    return -4;
  }
  return 0;
}
