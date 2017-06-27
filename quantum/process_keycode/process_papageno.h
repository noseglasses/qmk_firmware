/* Copyright 2016 Jack Humbert
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

#ifndef PROCESS_PAPAGENO_H
#define PROCESS_PAPAGENO_H

#include "papageno.h"

bool ppg_qmk_process_key_event_callback(	
								PPG_Key_Event *key_event,
								uint8_t state_flag, 
								void *user_data);

void ppg_qmk_flush_key_events(void);

void ppg_qmk_process_keycode(
								void *user_data);

bool ppg_qmk_process_key_event(
								uint16_t keycode, 
								keyrecord_t *record);

void ppg_qmk_time(			PPG_Time *time);

void  ppg_qmk_time_difference(
								PPG_Time time1,
								PPG_Time time2,
								PPG_Time *delta);

int8_t ppg_qmk_time_comparison(
								PPG_Time time1,
								PPG_Time time2);

void ppg_qmk_set_timeout_ms(uint16_t timeout);

/* Call this to flush key events that
 * were encountered by papageno
 */
void ppg_qmk_flush_key_events(void);

#define PPG_QMK_ACTION_KEYCODE(KK) \
	(PPG_Action) {	\
		.flags = PPG_Action_Undefined, \
		.user_callback = (PPG_User_Callback) { \
			.func = ppg_qmk_process_keycode,  \
			.user_data = (void*)KK \
		} \
	}
	
#ifdef DEBUG_PAPAGENO
#define PPG_QMK_SET_PRINTF \
	ppg_set_printf((PPG_Printf_Fun)__xprintf);
#else
#define PPG_QMK_SET_PRINTF
#endif

#define QMK_INIT_PAPAGENO \
	\
	ppg_set_key_processor((PPG_Key_Event_Processor_Fun)ppg_qmk_process_key_event_callback); \
	\
	ppg_set_time_function((PPG_Time_Fun)ppg_qmk_time); \
	ppg_set_time_difference_function((PPG_Time_Difference_Fun)ppg_qmk_time_difference); \
	\
	PPG_QMK_SET_PRINTF

#define PPG_QMK_KEYPOS_HEX(COL, ROW) (*(PPG_Key_Id*)(&(keypos_t){ .row = 0x##ROW, .col = 0x##COL}))

#endif
