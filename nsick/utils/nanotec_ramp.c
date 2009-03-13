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

#include <stdio.h>
#include <stdlib.h>
#include <nanotec.h>

int main(int argc, char **argv) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s FREQ0 FREQ1 RAMP\n", argv[0]);
    return -1;
  }

  nanotec_settings_t settings;
  nanotec_settings_default(&settings, 1);

  double f0 = atof(argv[1]);
  double f1 = atof(argv[2]);
  double r = atof(argv[3]);

  double dt = 1e-3*r;
  double df = 100.0;
  double b = settings.step_res/settings.step_size;
  double dw = b*df;

  fprintf(stderr, "start frequency: %.1fHz\n", f0);
  fprintf(stderr, "end frequency: %.1fHz\n", f1);
  fprintf(stderr, "linear ramp factor: %.1f\n", r);
  fprintf(stderr, "calculating ramp...\n");
  fprintf(stdout, "%4s  %8s  %8s  %8s  %10s  %12s\n",
    "step", "time[s]", "freq[Hz]", "pos[deg]", "vel[deg/s]", "acc[deg/s^2]");

  int step = 1;
  double f = f0;
  double t = 0.0;
  double a = 0.0;
  double w = 0.0;
  double w_dot = 0.0;

  while (f <= f1) {
    w = b*f;
    a = w*t;
    w_dot = dw/dt;

    fprintf(stdout,
      "%4d  %8.5f  %8.1f  %8.3f  %10.3f  %12.3f\n",
      step, t, f, nanotec_rad_to_deg(a), nanotec_rad_to_deg(w),
      nanotec_rad_to_deg(w_dot));

    ++step;
    t += dt;
    f += df;
  }

  fprintf(stderr, "... done\n");

  return 0;
}
