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

#include "process_papageno.h"

/* This function is defined in quantum/keymap_common.c 
 */
action_t action_for_configured_keycode(uint16_t keycode);

// inline
// static uint16_t ppg_qmk_get_event_state(PPG_Input_State *state)
// {
// 	return *(uint16_t*)&state;
// }

// inline 
// static void ppg_qmk_set_event_state(PPG_Input_State *target,
// 												uint16_t source)
// {
// 	*(uint16_t*)target = source;
// }
// 
// inline 
// static PPG_Input_State ppg_qmk_to_event_state(uint16_t source)
// {
// 	return *(PPG_Input_State*)&source;
// }

void ppg_qmk_process_event_callback(	
										PPG_Event *event,
										void *user_data)
{
	// Ignore events that were considered
	//
	if(event->flags & PPG_Event_Considered) { return; }
	
	keypos_t key = ppg_qmk_keypos_lookup[event->input];
  uint16_t keycode;
	
	#if !defined(NO_ACTION_LAYER) && defined(PREVENT_STUCK_MODIFIERS)
    /* TODO: Use store_or_get_action() or a similar function. */
    if (!disable_action_cache) {
      uint8_t layer;

      if (event->flags & PPG_Event_Active) {
        layer = layer_switch_get_layer(key);
        update_source_layers_cache(key, layer);
      } else {
        layer = read_source_layers_cache(key);
      }
      keycode = keymap_key_to_keycode(layer, key);
    } else
  #endif
    keycode = keymap_key_to_keycode(layer_switch_get_layer(key), key);
				
	PPG_PRINTF("Issuing keycode %u, pressed = %u\n", keycode, event->flags & PPG_Event_Active);
	
	if(event->flags & PPG_Event_Active) {
		register_code16(keycode);
	}
	else {
		unregister_code16(keycode);
	}
}

void ppg_qmk_flush_events(void)
{	
	ppg_event_buffer_iterate(
		ppg_qmk_process_event_callback,
		NULL
	);
}

void ppg_qmk_process_keycode(uint8_t slot_id, void *user_data) {
	
	uint16_t keycode = (uint16_t)user_data;
	
	if(keycode != 0) {
		
// 		PPG_PRINTF("Passing keycode %u to qmk system\n", keycode);
// 		
// 		register_code(keycode);
// 		unregister_code(keycode);
		
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
		
// 		PPG_PRINTF("Passing keycode %u to qmk system\n", configured_keycode);
		
		action_t action = action_for_configured_keycode(configured_keycode); 
	
		process_action(&record, action);
		
		record.event.pressed = false;
		record.event.time = timer_read();
		
		process_action(&record, action);
	}
}

void ppg_qmk_process_event(
				uint16_t keycode, 
				keyrecord_t *record)
{
	PPG_Event event = {
		.input = ppg_qmk_input_id_from_keypos(record->event.key.row, record->event.key.col),
		.time = (PPG_Time)record->event.time,
		.flags = (record->event.pressed) ? PPG_Event_Active : PPG_Event_Flags_Empty
	};
	
	uint8_t cur_layer = biton32(layer_state);
	
	ppg_global_set_layer(cur_layer);
	
	ppg_event_process(
							&event);
}

void ppg_qmk_time(PPG_Time *time)
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
	ppg_global_set_timeout((PPG_Time)timeout);
}

// uint16_t ppg_qmk_input_id_from_keypos(uint8_t row, uint8_t col)
// {
// 	uint16_t result = (*(uint16_t*)(&(keypos_t){ .row = row, .col = col}));
// 		
// // 	PPG_PRINTF("row = %u\n", row);
// // 	PPG_PRINTF("col = %u\n", col);
// // 	PPG_PRINTF("result = %u\n", result);
// 	
// 	return result;
// }

// PPG_QMK_Key_Data *ppg_qmk_create_key_data(
// 										keypos_t keypos,
// 										uint16_t keycode)
// {
// 	PPG_QMK_Key_Data *kd = (PPG_QMK_Key_Data*)malloc(sizeof(PPG_QMK_Key_Data));
// 	
// 	kd->keypos = keypos;
// 	kd->keycode = keycode;
// 	
// // 	PPG_PRINTF("kd->data == %u\n", kd->data);
// // 	PPG_PRINTF("kd->is_keycode == %d\n", kd->is_keycode);
// 	
// 	return kd;
// }

// bool ppg_qmk_check_key_active(PPG_Input_Id input_id,
// 										PPG_Input_State state)
// {
// 	return (ppg_qmk_get_event_state(state) == PPG_QMK_Key_Pressed);
// }

// bool ppg_qmk_input_id_equal(PPG_Input_Id input_id1, PPG_Input_Id input_id2)
// {
// 	PPG_QMK_Key_Data *kd1 = (PPG_QMK_Key_Data *)input_id1;
// 	PPG_QMK_Key_Data *kd2 = (PPG_QMK_Key_Data *)input_id2;
// 	
// // 	PPG_PRINTF("kd1->data == %u\n", kd1->data);
// // 	PPG_PRINTF("kd2->data == %u\n", kd2->data);
// // 	PPG_PRINTF("kd1->is_keycode == %d\n", kd1->is_keycode);
// // 	PPG_PRINTF("kd2->is_keycode == %d\n", kd2->is_keycode);
// 	
// 	return 	((kd1->keycode == kd2->keycode) && (kd1->keycode != 0))
// 			||	(		((kd1->keypos.row == kd2->keypos.row) && (kd1->keypos.row != 0))
// 					&&	((kd1->keypos.col == kd2->keypos.col) && (kd1->keypos.col != 0))
// 				);
// }
