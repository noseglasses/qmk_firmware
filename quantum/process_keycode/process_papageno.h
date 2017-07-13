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
 * As Papageno's definition of what is an input is very arbitrary, it is possible to
 * use keyboard matix coordinates as well as qmk keycodes to define keys (notes in Papageno jargon).
 * 
 * Key press and release actions are passed to Papageno.
 * Use the macro PPG_QMK_MATRIX_KEY_HEX to specify a physical key with respect to its
 * hexadecimal row and column ids or the macro PPG_QMK_KEYCODE_KEY to specify a qmk keycode
 * as key identifier.
 * 
 * Be careful with layer switching actions as they abort
 * pattern matching.
*/

#include "papageno.h"
#include "quantum.h"
#include "tmk_core/common/keyboard.h"

extern keypos_t ppg_qmk_keypos_lookup[];

extern PPG_Input_Id ppg_qmk_input_id_from_keypos(uint8_t row, uint8_t col);

void ppg_qmk_process_event_callback(	
								PPG_Event *key_event,
								void *user_data);

void ppg_qmk_flush_key_events(void);

void ppg_qmk_process_keycode(
								uint8_t slot_id, 
								void *user_data);

void ppg_qmk_process_event(
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

#define PPG_QMK_KEYPOS_HEX(COL, ROW, S___) \
	S___(COL, ROW)

#define PPG_QMK_KEYPOS_HEX_1(COL, ROW) \
	(keypos_t){ .row = 0x##ROW, .col = 0x##COL }

// #define PPG_QMK_MATRIX_KEY_HEX(COL, ROW) 
// 	(PPG_Input_Id) { 
// 		.input_id = ppg_qmk_create_key_data(PPG_QMK_KEYPOS_HEX(COL, ROW), 0) 
// 	}
	
// #define PPG_QMK_KEYCODE_KEY(KK) 
// 	(PPG_Input) { 
// 		.input_id = ppg_qmk_create_key_data(PPG_QMK_KEYPOS_HEX(0, 0), KK) 
// 	}	

#define PPG_QMK_ACTION_KEYCODE(KK) \
	(PPG_Action) {	\
		.flags = PPG_Action_Default, \
		.callback = (PPG_Action_Callback) { \
			.func = (PPG_Action_Callback_Fun)ppg_qmk_process_keycode,  \
			.user_data = (void*)(uint16_t)KK \
		} \
	}

#define PPG_QMK_INIT \
	\
	ppg_global_init(); \
	\
	ppg_global_set_default_event_processor((PPG_Event_Processor_Fun)ppg_qmk_process_event_callback); \
	\
	ppg_global_set_time_function((PPG_Time_Fun)ppg_qmk_time); \
	ppg_global_set_time_difference_function((PPG_Time_Difference_Fun)ppg_qmk_time_difference); \
	ppg_global_set_time_comparison_function((PPG_Time_Comparison_Fun)ppg_qmk_time_comparison);
	
#define PPG_QMK_KEYS(...) PPG_INPUTS(__VA_ARGS__)
	
#define PPG_QMK_CONVERT_TO_CASE_LABEL(ROW_HEX, COL_HEX) \
	256*0x##ROW_HEX + 0x##COL_HEX
	
/** @param KEYPOS_ALIAS An alias that is define by the user 
 *            that consumes a parameter S that is 
 *            the name of a macro function that consumes
 *            two parameters ROW and COL
 */
#define PPG_QMK_KEYPOS_CASE_LABEL(RET_VAL, KEYPOS_ALIAS) \
	case KEYPOS_ALIAS(PPG_QMK_CONVERT_TO_CASE_LABEL): \
		return RET_VAL; \
		break;
	
#define PPG_INIT_INPUT_ID_FROM_KEYPOS_FUNCTION \
	\
	PPG_Input_Id ppg_qmk_input_id_from_keypos(uint8_t row, uint8_t col) \
	{ \
		uint16_t id = 256*row + col; \
		\
		switch(id) { \
			\
			PPG_QMK_KEY_SET(PPG_QMK_KEYPOS_CASE_LABEL) \
			\
			default: \
				PPG_ERROR("Undefined keypos\n"); \
				break; \
		} \
		\
		return (PPG_Input_Id)-1; \
	}
	
#define PPG_QMK_CONVERT_TO_KEYPOS_ARRAY_ENTRY_AUX(ROW_HEX, COL_HEX) \
	/*(keypos_t)*/{ .row = 0x##ROW_HEX, .col = 0x##COL_HEX },
	
#define PPG_QMK_CONVERT_TO_KEYPOS_ARRAY_ENTRY(ROW_HEX, COL_HEX) \
	PPG_QMK_CONVERT_TO_KEYPOS_ARRAY_ENTRY_AUX(ROW_HEX, COL_HEX)
	
#define PPG_QMK_KEYPOS_TO_LOOKUP_ENTRY(RET_VAL, KEYPOS_ALIAS) \
	KEYPOS_ALIAS(PPG_QMK_CONVERT_TO_KEYPOS_ARRAY_ENTRY)
	
#define PPG_QMK_INIT_KEYPOS_LOOKUP \
	\
	keypos_t ppg_qmk_keypos_lookup[] = { \
		\
		PPG_QMK_KEY_SET(PPG_QMK_KEYPOS_TO_LOOKUP_ENTRY) \
		\
		PPG_QMK_CONVERT_TO_KEYPOS_ARRAY_ENTRY_AUX(FF, FF) \
	};
	
#define PPG_QMK_INIT_DATA_STRUCTURES \
	\
	PPG_INIT_INPUT_ID_FROM_KEYPOS_FUNCTION \
	\
	PPG_QMK_INIT_KEYPOS_LOOKUP
	
#define PPG_QMK_INPUT_FROM_KEYPOS_CALL(ROW_HEX, COL_HEX) \
	ppg_qmk_input_id_from_keypos(0x##ROW_HEX, 0x##COL_HEX)
	
#define PPG_QMK_INPUT_FROM_KEYPOS(KEYPOS_ALIAS) \
	KEYPOS_ALIAS(PPG_QMK_INPUT_FROM_KEYPOS_CALL)

#endif
