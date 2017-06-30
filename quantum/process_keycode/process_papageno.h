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

/**
 * This is an interface to the Papageno library. It allows for using 
 * Papageno magic melodies as part of the qmk firmware.
 * 
 * As Papageno's definition of what is a key is very arbitrary, it is possible to
 * use keyboard matix coordinates as well as qmk keycodes to define keys (notes in Papageno jargon).
 * Use the macro PPG_QMK_MATRIX_KEY_HEX to specify a physical key with respect to its
 * hexadecimal row and column ids or the macro PPG_QMK_KEYCODE_KEY to specify a qmk keycode
 * as key identifier.
 * 
 * To allow all this to work, magic melodies are partially independent of keymaps and implemented
 * on top of them. 
 * 
 * Be careful with layer switching actions. As a general rule these are acceptable in case of melody completion.
 * They may be acceptable on fall back. And finally, it is very important not to assign the flag PPG_Action_Immediate
 * to any layer switching actions as Papageno cannot deal with layer switching while a melody is in progress.
*/

#include "papageno.h"
#include "quantum.h"
#include "tmk_core/common/keyboard.h"

bool ppg_qmk_process_key_event_callback(	
								PPG_Key_Event *key_event,
								uint8_t slot_id, 
								void *user_data);

void ppg_qmk_flush_key_events(void);

void ppg_qmk_process_keycode(
								uint8_t slot_id, 
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

typedef struct {
	keypos_t keypos;
	uint16_t keycode;
} PPG_QMK_Key_Data;

PPG_QMK_Key_Data *ppg_qmk_create_key_data(
								keypos_t keypos,
								uint16_t keycode);

enum PPG_QMK_Key_State {
	PPG_QMK_Key_Released = 0,
	PPG_QMK_Key_Pressed
};

bool ppg_qmk_check_key_active(PPG_Key_Id key_id,
										PPG_Key_State state);


bool ppg_qmk_key_id_equal(PPG_Key_Id key_id1, PPG_Key_Id key_id2);

uint16_t ppg_qmk_key_id_from_keypos(uint8_t row, uint8_t col);

#define PPG_QMK_KEYPOS_HEX(COL, ROW) \
	(keypos_t){ .row = 0x##ROW, .col = 0x##COL }

#define PPG_QMK_MATRIX_KEY_HEX(COL, ROW) \
	(PPG_Key) { \
		.key_id = ppg_qmk_create_key_data(PPG_QMK_KEYPOS_HEX(COL, ROW), 0), \
		.check_active = (PPG_Key_Activation_Check_Fun)ppg_qmk_check_key_active \
	}
	
#define PPG_QMK_KEYCODE_KEY(KK) \
	(PPG_Key) { \
		.key_id = ppg_qmk_create_key_data(PPG_QMK_KEYPOS_HEX(0, 0), KK), \
		.check_active = (PPG_Key_Activation_Check_Fun)ppg_qmk_check_key_active, \
	}	

#define PPG_QMK_ACTION_KEYCODE(KK) \
	(PPG_Action) {	\
		.flags = PPG_Action_Default, \
		.user_callback = (PPG_User_Callback) { \
			.func = (PPG_User_Callback_Fun)ppg_qmk_process_keycode,  \
			.user_data = (void*)(uint16_t)KK \
		} \
	}

#define PPG_QMK_INIT \
	\
	ppg_init(); \
	\
	ppg_set_default_key_processor((PPG_Key_Event_Processor_Fun)ppg_qmk_process_key_event_callback); \
	\
	ppg_set_time_function((PPG_Time_Fun)ppg_qmk_time); \
	ppg_set_time_difference_function((PPG_Time_Difference_Fun)ppg_qmk_time_difference); \
	ppg_set_time_comparison_function((PPG_Time_Comparison_Fun)ppg_qmk_time_comparison); \
	ppg_set_key_id_equal_function((PPG_Key_Id_Equal_Fun)ppg_qmk_key_id_equal); 

#endif
