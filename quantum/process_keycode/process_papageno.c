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

#include "papageno.h"
#include "quantum.h"
#include "tmk_core/common/keyboard.h"

#ifdef DEBUG_PAPAGENO
#include "debug.h"
#define PPG_PRINTF(...) \
	uprintf(__VA_ARGS__);
#define PPG_ERROR(...) PPG_PRINTF("*** Error: ", ##__VA_ARGS__)
#else
#define PPG_PRINTF(...)
#define PPG_ERROR(...)
#endif

/* This function is defined in quantum/keymap_common.c 
 */
action_t action_for_configured_keycode(uint16_t keycode);

typedef struct {
	uint16_t time_offset;
} PPG_QMK_Keycode_Data;

bool ppg_qmk_process_key_event_callback(	PPG_Key_Event *key_event,
										uint8_t state_flag, 
										void *user_data)
{
	PPG_QMK_Keycode_Data *kd = (PPG_QMK_Keycode_Data *)user_data;
	
	if(kd->time_offset == 0) {
		kd->time_offset = timer_read() - (uint16_t)key_event->time;
	};
	
	/* keypos_t and key_id are assumed to be 16 bit */
	keypos_t keypos = *((keypos_t*)&key_event->key_id);
	
	keyrecord_t record = {
		.event = {
			.time = (uint16_t)key_event->time + kd->time_offset,
			.key = keypos,
			.pressed = key_event->pressed
		}
	};
		
	PPG_PRINTF("Issuing keystroke at %d, %d\n", record.event.key.row, record.event.key.col);
		
	process_record_quantum(&record);
	
	return true;
}

void ppg_qmk_flush_key_events(void)
{
	PPG_QMK_Keycode_Data kd = {
		.time_offset = 0
	};
	
	ppg_flush_stored_key_events(
		PPG_Key_Flush_User,
		ppg_qmk_process_key_event_callback,
		(void*)&kd
	);
}

void ppg_qmk_process_keycode(void *user_data) {
	
	uint16_t keycode = (uint16_t)user_data;
	
	if(keycode != 0) {
		
		/* Construct a dummy record
		*/
		keyrecord_t record;
			record.event.key.row = 0;
			record.event.key.col = 0;
			record.event.pressed = true;
			record.event.time = timer_read();
			
		/* Use the quantum/tmk system to trigger the action
			* thereby using a fictituous a key (0, 0) with which the action 
			* keycode is associated. We pretend that the respective key
			* was hit and released to make sure that any action that
			* requires both events is correctly processed.
			* Unfortunatelly this means that some actions that
			* require keys to be held do not make sense, e.g.
			* modifier keys or MO(...), etc.is
			*/
		
		uint16_t configured_keycode = keycode_config(keycode);
		
		action_t action = action_for_configured_keycode(configured_keycode); 
	
		process_action(&record, action);
		
		record.event.pressed = false;
		record.event.time = timer_read();
		
		process_action(&record, action);
	}
}

bool ppg_qmk_process_key_event(
				uint16_t keycode, 
				keyrecord_t *record)
{
	PPG_Key_Event key_event = {
		.key_id = *((PPG_Key_Id*)&record->event.key),
		.time = (PPG_Time)record->event.time,
		.pressed = record->event.pressed
	};
	
	uint8_t cur_layer = biton32(layer_state);
	
	return ppg_process_key_event(
							&key_event, cur_layer);
}

void  ppg_qmk_time(PPG_Time *time)
{
	uint16_t time_tmp = timer_read();
	*time = *((PPG_Time*)&time_tmp);
}

void  ppg_qmk_time_difference(PPG_Time time1, PPG_Time time2, PPG_Time *delta)
{
	uint16_t *delta_t = (uint16_t *)delta;
	
	*delta_t = (uint16_t)time2 - (uint16_t)time1; 
}

int8_t ppg_qmk_time_comparison(
								PPG_Time time1,
								PPG_Time time2)
{
	if((uint16_t)time1 > (uint16_t)time2) {
		return 1;
	}
	else if((uint16_t)time1 == (uint16_t)time2) {
		return 0;
	}
	 
	return -1;
}

void ppg_qmk_set_timeout_ms(uint16_t timeout)
{
	ppg_set_timeout((PPG_Time)timeout);
}

