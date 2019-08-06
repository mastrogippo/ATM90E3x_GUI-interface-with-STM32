# This STM32F103 firmware interfaces an ATM90E3x with Atmel's GUI program
# Copyright (C) 2019 Mastro Gippo
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.


#define pin_rly2 PB_0
#define pin_aux3 PB_1
#define pin_sclk PB_3
#define pin_miso PB_4
#define pin_mosi PB_5
#define pin_cs PB_6
#define pin_can_rx PB_8
#define pin_can_tx PB_9
#define pin_scl PB_10
#define pin_sda PB_11
#define pin_rly1 PB_12
#define pin_sck2 PB_13
#define pin_hv_pres PB_14
#define pin_hv_pres1 PB_15
#define pin_pilot_r1 PA_0
#define pin_pilot_d1 PA_1
#define pin_485tx PA_2
#define pin_485rx PA_3
#define pin_485en PA_4
#define pin_pilot_r PA_5
#define pin_pilot_d PA_6
#define pin_rcd PA_7
#define pin_u_tx PA_9
#define pin_u_rx PA_10
#define pin_usbn PA_11
#define pin_usbp PA_12
#define pin_tp1 PA_13
#define pin_tp2 PA_14
#define pin_ledstrip PA_15
#define pin_rcd_t PC_13

#define fw_ver_major 0x00
#define fw_ver_minor 0x01

//TODO: add configuration check for single/dual
#define num_ports 1

