/* Copyright 2017 Florian Fleissner
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

#define PPG_QMK_ERGODOX

extern keypos_t ppg_qmk_keypos_lookup[];

extern PPG_Input_Id ppg_qmk_input_id_from_keypos(uint8_t row, uint8_t col);

void ppg_qmk_process_event_callback(   
                        PPG_Event *key_event,
                        void *user_data);

void ppg_qmk_signal_callback(PPG_Slot_Id slot_id, void *user_data);

void ppg_qmk_flush_key_events(void);

void ppg_qmk_process_keycode(void *user_data);

bool ppg_qmk_process_event(
                        uint16_t keycode, 
                        keyrecord_t *record);

void ppg_qmk_time(         PPG_Time *time);

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

#ifdef PPG_QMK_ERGODOX
// Methods for LED-signals (e.g. to use on ErgoDox EZ)
//
void ppg_qmk_led_signal(void);
void ppg_qmk_led_flash(void);
void ppg_qmk_led_superflash(void);
#endif

enum { PPG_QMK_Empty_Input = (PPG_Input_Id)-1 };

// Note: Preprocessor macro functions can be 
//       hard to debug.
//
//       One approach is to take a look at
//       the preprocessed code to find out 
//       what goes wrong. 
//
//       Unfortunatelly macro replacement cannot deal with newlines.
//       Because of this, code that emerges from excessive macro
//       replacement looks very unreadable due to the absence of 
//       any line breaks.
//
//       To deal with this restriction, comment the
//       definition of the __NL__ macro below, during
//       preprocessor debugging. In that case, the
//       occurrences of __NL__ will not be replaced by
//       and empty string as in the compiled version but
//       will still be present in the preprocessed code.
//       Replace all occurrences of __NL__ in the preprocessed
//       with newlines using a text editor to gain a
//       readable version of the generated code.

#define __NL__

#define PPG_QMK_KEYPOS_HEX(COL_HEX, ROW_HEX, S___) \
   S___(COL_HEX, ROW_HEX)

// #define PPG_QMK_MATRIX_KEY_HEX(COL_HEX, ROW_HEX) 
//    (PPG_Input_Id) { 
//       .input_id = ppg_qmk_create_key_data(PPG_QMK_KEYPOS_HEX(COL_HEX, ROW_HEX), 0) 
//    }
   
// #define PPG_QMK_KEYCODE_KEY(KK) 
//    (PPG_Input) { 
//       .input_id = ppg_qmk_create_key_data(PPG_QMK_KEYPOS_HEX(0, 0), KK) 
//    }  

#define PPG_QMK_ACTION_KEYCODE(KK) \
__NL__   (PPG_Action) { \
__NL__      .flags = PPG_Action_Default, \
__NL__      .callback = (PPG_Action_Callback) { \
__NL__         .func = (PPG_Action_Callback_Fun)ppg_qmk_process_keycode,  \
__NL__         .user_data = (void*)(uint16_t)KK \
__NL__      } \
__NL__   }

#define PPG_QMK_INIT \
__NL__   \
__NL__   ppg_global_init(); \
__NL__   \
__NL__   ppg_global_set_default_event_processor( \
__NL__      (PPG_Event_Processor_Fun)ppg_qmk_process_event_callback); \
__NL__   \
__NL__   ppg_global_set_signal_callback( \
__NL__      (PPG_Signal_Callback) { \
__NL__            .func = (PPG_Signal_Callback_Fun)ppg_qmk_signal_callback, \
__NL__            .user_data = NULL \
__NL__      } \
__NL__   ); \
__NL__   \
__NL__   ppg_global_set_time_manager( \
__NL__      (PPG_Time_Manager) { \
__NL__         .time \
__NL__            = (PPG_Time_Fun)ppg_qmk_time, \
__NL__         .time_difference \
__NL__            = (PPG_Time_Difference_Fun)ppg_qmk_time_difference, \
__NL__         .compare_times \
__NL__            = (PPG_Time_Comparison_Fun)ppg_qmk_time_comparison \
__NL__      } \
__NL__   ); \
__NL__   \
__NL__   ppg_global_set_number_of_inputs( \
__NL__      sizeof(ppg_qmk_keypos_lookup)/sizeof(keypos_t));
   
#define PPG_QMK_KEYS(...) PPG_INPUTS(__VA_ARGS__)
   
#define PPG_QMK_CONVERT_TO_CASE_LABEL(COL_HEX, ROW_HEX) \
   256*0x##ROW_HEX + 0x##COL_HEX
   
/** @param KEYPOS_ALIAS An alias that is define by the user 
 *            that consumes a parameter S that is 
 *            the name of a macro function that consumes
 *            two parameters ROW and COL
 */
#define PPG_QMK_KEYPOS_CASE_LABEL(KEYPOS_ALIAS) \
__NL__   case KEYPOS_ALIAS(PPG_QMK_CONVERT_TO_CASE_LABEL): \
__NL__      return __COUNTER__ - counter_offset - 1; \
__NL__      break;
   
#define PPG_INIT_INPUT_ID_FROM_KEYPOS_FUNCTION \
__NL__   \
__NL__   PPG_Input_Id ppg_qmk_input_id_from_keypos(uint8_t row, uint8_t col) \
__NL__   { \
__NL__      \
__NL__      enum { counter_offset = __COUNTER__ }; \
__NL__      \
__NL__      uint16_t id = 256*row + col; \
__NL__      \
__NL__      switch(id) { \
__NL__         \
__NL__         PPG_QMK_KEY_SET(PPG_QMK_KEYPOS_CASE_LABEL) \
__NL__      } \
__NL__      \
__NL__      return PPG_QMK_Empty_Input; \
__NL__   }
   
#define PPG_QMK_CONVERT_TO_KEYPOS_ARRAY_ENTRY_AUX(COL_HEX, ROW_HEX) \
   /*(keypos_t)*/{ .row = 0x##ROW_HEX, .col = 0x##COL_HEX }
   
#define PPG_QMK_CONVERT_TO_KEYPOS_ARRAY_ENTRY(COL_HEX, ROW_HEX) \
   PPG_QMK_CONVERT_TO_KEYPOS_ARRAY_ENTRY_AUX(COL_HEX, ROW_HEX),
   
#define PPG_QMK_KEYPOS_TO_LOOKUP_ENTRY(KEYPOS_ALIAS) \
   KEYPOS_ALIAS(PPG_QMK_CONVERT_TO_KEYPOS_ARRAY_ENTRY)
   
#define PPG_QMK_INIT_KEYPOS_LOOKUP \
__NL__   \
__NL__   keypos_t ppg_qmk_keypos_lookup[] = { \
__NL__      \
__NL__      PPG_QMK_KEY_SET(PPG_QMK_KEYPOS_TO_LOOKUP_ENTRY) \
__NL__      \
__NL__      PPG_QMK_CONVERT_TO_KEYPOS_ARRAY_ENTRY_AUX(FF, FF) \
__NL__   };
   
#define PPG_QMK_INPUT_FROM_KEYPOS_CALL(COL_HEX, ROW_HEX) \
   ppg_qmk_input_id_from_keypos(0x##ROW_HEX, 0x##COL_HEX)
   
#define PPG_QMK_INPUT_FROM_KEYPOS(KEYPOS_ALIAS) \
   KEYPOS_ALIAS(PPG_QMK_INPUT_FROM_KEYPOS_CALL)
   
#define PPG_QMK_ADD_ONE(KEYPOS_ALIAS) \
   + 1
   
#define PPG_QMK_STORE_N_INPUTS \
__NL__   enum { PPG_QMK_N_Inputs = 0 \
__NL__      PPG_QMK_KEY_SET(PPG_QMK_ADD_ONE) \
__NL__   };

#define PPG_QMK_INIT_DATA_STRUCTURES \
__NL__   \
__NL__   PPG_INIT_INPUT_ID_FROM_KEYPOS_FUNCTION \
__NL__   \
__NL__   PPG_QMK_INIT_KEYPOS_LOOKUP \
__NL__   \
__NL__   PPG_QMK_STORE_N_INPUTS
   

#endif
