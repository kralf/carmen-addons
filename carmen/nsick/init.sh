#!/bin/bash
############################################################################
#    Copyright (C) 2007 by Ralf 'Decan' Kaestner                           #
#    ralf.kaestner@gmail.com                                               #
#                                                                          #
#    This program is free software; you can redistribute it and#or modify  #
#    it under the terms of the GNU General Public License as published by  #
#    the Free Software Foundation; either version 2 of the License, or     #
#    (at your option) any later version.                                   #
#                                                                          #
#    This program is distributed in the hope that it will be useful,       #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#    GNU General Public License for more details.                          #
#                                                                          #
#    You should have received a copy of the GNU General Public License     #
#    along with this program; if not, write to the                         #
#    Free Software Foundation, Inc.,                                       #
#    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             #
############################################################################

# Initialize the nodding SICK connected through
# Moxa UPort 1450 USB to 4 Port RS-232/422/485 Serial Hub

. ../../ubash/global.sh

EPOSDEV=`grep epos_dev nsick.ini | grep -o [[:alnum:]/]*$`
SICKID=`grep epos_laser_id nsick.ini | grep -o [0-9]$`
SICKDEV=`grep laser_laser${SICKID}_dev nsick.ini | grep -o [[:alnum:]/]*$`

script_init "initialize the nodding SICK 3D laser device"

script_setopt "--epos-dev|-m" "DEV" "EPOSDEV" "$EPOSDEV" \
  "device corresponding to the EPOS node"
script_setopt "--sick-dev|-s" "DEV" "SICKDEV" "$SICKDEV" \
  "device corresponding to the SICK LMS unit"

script_checkopts $*

[ -c "$EPOSDEV" ] || message_exit "EPOS device $EPOSDEV not found"
[ -c "$SICKDEV" ] || message_exit "SICK device $SICKDEV not found"

execute "setserial $EPOSDEV port 0"
execute "setserial $SICKDEV port 2"

log_clean
