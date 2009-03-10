/***************************************************************************
 *   Copyright (C) 2004 by Ralf Kaestner                                   *
 *   ralf.kaestner@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <nanotec.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s DEV\n", argv[0]);
    return -1;
  }

  nanotec_motor_t motor;

  nanotec_set_parameters(&motor, argv[1], 0, 0, 0);
//   nanotec_connect_device(&motor);
  nanotec_start_motor(&motor);

//   nanotec_set_start_freq(&motor, 100);
//   nanotec_set_max_freq(&motor, 100, 1);
//   while (1) {
//     nanotec_move_nsteps(&motor, NANOTEC_RIGHT, 10);
//     nanotec_wait_status(&motor, NANOTEC_STATUS_READY);
// 
//     int pos = nanotec_get_position(&motor);
//     printf("\rposition: %d", pos);
//   }
//   printf("\n");

  nanotec_stop_motor(&motor);
  return 0;
}
