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

#include "quantum.h"

#ifdef MAGIC_MELODIES_ENABLE

#define MM_MAX_KEYCHANGES 100

#define MM_DEFAULT_ROW_LENGTH 7

static bool mm_keypos_equal(keypos_t kp1, keypos_t kp2)
{
	return 		(kp1.row == kp2.row)
				&&	(kp1.col == kp2.col);
}

typedef struct {
	
	MM_Phrase_Consider_Keychange_Fun 
									consider_keychange;
									
	MM_Phrase_Subphrase_Consider_Keychange_Fun 
									successor_consider_keychange;
									
	MM_Phrase_Reset_Fun
									reset;
									
	MM_Phrase_Trigger_Action_Fun
									trigger_action;
									
	MM_Phrase_Destroy_Fun	
									destroy;
} MM_Phrase_Vtable;

typedef struct MM_PhraseStruct {
	
    MM_MM_Phrase_Vtable vtable;
	 
	 struct MM_PhraseStruct *parent;
	 
	 struct MM_PhraseStruct **successors;
	 
	 uint8_t n_allocated_successors;
	 uint8_t n_successors;
	 
	 uint16_t action_keycode;
	 
	 uint8_t state;
	 
} MM_Phrase;

typedef struct
{
  uint16_t melody_start_timer;
  uint16_t n_keys_changed;
  bool pressed[MM_MAX_KEYCHANGES];
  keypos_t keypos[MM_MAX_KEYCHANGES];
  uint16_t keytimers[MM_MAX_KEYCHANGES];
  
  MM_Phrase melody_root;
  bool melody_root_initialized;
  
  MM_Phrase *current_phrase;
  
  bool magic_melodies_temporarily_disabled;
  
  keypos_t abort_keypos;
  
} MM_Magic_Melody_State;

static MM_Magic_Melody_State mm_state = 
{
	.timer = 0,
	.n_keys_changed = 0,
	.melody_root_initialized = false,
	.current_phrase = NULL,
	.magic_melodies_temporarily_disabled = false,
	.abort_keypos = { .row = 100, .col = 100 }
};

enum {
	MM_Phrase_In_Progress = 0,
	MM_Phrase_Completed,
	MM_Phrase_Invalid
};

static void mm_store_keychange(uint16_t keycode, 
										keyrecord_t *record)
{
	assert(mm_state.n_keys_changed < MM_MAX_KEYCHANGES);
	
	mm_state.pressed[n_keys_changed] = record->pressed;
	mm_state.keypos[n_keys_changed] = record->event.key;
	mm_state.keytimers[n_keys_changed] = record->event.timer;
	
	++mm_state.n_keys_changed;
}

static void mm_flush_stored_keychanges()
{
	if(mm_state.n_keys_changed == 0) { return; }
	
	/* Process all events as if they had happened just now */
	
	mm_state.magic_melodies_temporarily_disabled = true;
	
	uint16_t i;
	
	keyrecord_t record;
       
	uint16_t cur_time = timer_read ();
	
	uint16_t first_key_timer = mm_state.keytimers[0];
	
	uint16_t actualOffset = cur_time - mm_state.keytimers[mm_state.n_keys_changed - 1];
	
	for(i = 0; i < mm_state.n_keys_changed; ++i) {
		
		record.event.timer = mm_state.keytimers[i] + actualOffset;
		record.event.key = mm_state.keypos[i];
		record.event.pressed = mm_state.pressed[i];
		
		process_record_quantum(&record);
	}
	
	mm_state.magic_melodies_temporarily_disabled = false;
}

static void mm_cleanup()
{
	mm_flush_stored_keychanges();
	
	mm_state.n_keys_changed = 0;
	mm_state.current_phrase = NULL;
}

typedef uint8_t (*MM_Phrase_Consider_Keychange_Fun)(	MM_Phrase *a_This, 
														uint16_t keycode, 
														keyrecord_t *record);

typedef uint8_t (*MM_Phrase_Subphrase_Consider_Keychange_Fun)(
														MM_Phrase *a_This, 
														uint16_t keycode, 
														keyrecord_t *record);

typedef void (*MM_Phrase_Reset_Fun)(	MM_Phrase *a_This);

typedef void (*MM_Phrase_Trigger_Action_Fun)(	MM_Phrase *a_This);

typedef MM_Phrase * (*MM_Phrase_Destroy_Fun)(MM_Phrase *a_This);

static MM_Phrase *mm_phrase_alloc(){
    return (MM_Phrase*)malloc(sizeof(MM_Phrase));
}

static void mm_phrase_reset(	MM_Phrase *a_This)
{
	state = MM_Phrase_In_Progress;
}

/* Phrases have states. This method resets the states
 * after processing a melody. On our way through the
 * melody tree we only need to reset thoses phrases
 * that were candidates.
 */
static void mm_phrase_reset_recursively(	MM_Phrase *a_This)
{
	/* Reset all successor phrases if there are any
	 */
	uint8_t i;
	for(i = 0; i < a_This.n_successors; ++i) {
		a_This->successors[i]->vtable.reset(a_This->successors[i]);
	}
		
	if(a_This->parent) {
		mm_phrase_reset_recursively(a_This->parent);
	}		
}

static uint8_t mm_phrase_consider_keychange(	MM_Phrase **current_phrase,
												uint16_t keycode, 
												keyrecord_t *record) 
{
	/* Loop over all phrases and inform them about the 
	 * keychange 
	 */
	uint8_t i;
	uint8_t successor_process_result;
	bool all_successors_invalid = true;
	
	MM_Phrase *a_current_phrase = *current_phrase;
	
	mm_store_keychange(keycode, record);
	
	for(i = 0; i < a_current_phrase->n_successors; ++i) {
		
		if(a_current_phrase->successors[i].phrase_inventory.state == MM_Phrase_Invalid) {
			continue;
		}
		
		successor_process_result 
			= a_current_phrase->successors[i]
					->vtable.successor_consider_keychange(	
																	a_current_phrase, 
																	keycode,
																	record
																	);
			
		switch(successor_process_result) {
			
			case MM_Phrase_In_Progress:
				all_successors_invalid = false;
				break;
				
			case MM_Phrase_Completed:
				
				/* The successor that is first completed becomes the 
				 * new current phrase
				 */
				current_phrase = &a_current_phrase->successors[i]);
				a_current_phrase = *current_phrase;
				
				/* The melody is finished. Trigger the associated action.
				 */
				if(0 == a_current_phrase->n_successors) {
					
					a_current_phrase->vtable.trigger_action(a_current_phrase);
					
					mm_phrase_reset_recursively(a_current_phrase.parent);
					
					return MM_Phrase_Completed;
				}
				break;
		}
	}
		
	if(all_successors_invalid) {
		
		/* The frase could not be parsed. Reset any previous phrases.
		 */
		mm_phrase_reset_recursively(a_current_phrase);
		
		/* Cleanup and issue all keypresses as if they happened without parsing a melody
			*/
		mm_cleanup();
		
		return MM_Phrase_Invalid;
	}
	
	return MM_Phrase_In_Progress;
}

static void mm_phrase_trigger_action(MM_Phrase *a_MM_Phrase) {
	
	if(a_MM_Phrase->action_keycode != 0) {
		Trigger the action that is associated with the keycode
	}
}

static void mm_phrase_allocate_successors(MM_Phrase *a_This, uint8_t n_successors) {

	 a_This->successors 
		= (struct MM_PhraseStruct **)malloc(n_successors*sizeof(struct MM_PhraseStruct*));
	 a_This->n_allocated_successors = n_successors;
}

static void mm_phrase_free_successors(MM_Phrase *a_This)
{
	if(!a_This->successors) { return; }
	
	int i;
	
	for(i = 0; i < a_This->n_allocated_successors; ++i) {
		a_This->successors[i]->vtable.destroy(a_This->successors[i]);
		free(a_This->successors[i]);
	}
	
	free(a_This->successors);
	
	a_This->successors = NULL;
	a_This->n_allocated_successors = 0;
}

static void mm_phrase_add_successor(MM_Phrase *a_This, MM_Phrase *successor)
{
	assert(a_This->n_successors < a_This->n_allocated_successors);
	assert(a_This->successors);
	
	a_This->successors[n_successors] = successor;
}

static MM_Phrase* mm_phrase_destroy(MM_Phrase *a_MM_Phrase) {

	mm_phrase_free_successors(a_MM_Phrase);
	
	return a_MM_Phrase;
}

static MM_Phrase *mm_phrase_new(MM_Phrase *a_MM_Phrase) {
	
    a_MM_Phrase->vtable.consider_keychange 
		= (MM_Phrase_Consider_Keychange_Fun) mm_phrase_consider_keychange;
    a_MM_Phrase->vtable.successor_consider_keychange = NULL;
    a_MM_Phrase->vtable.reset = (MM_Phrase_Reset_Fun) mm_phrase_reset;
    a_MM_Phrase->vtable.trigger_action = (MM_Phrase_Trigger_Action_Fun) mm_phrase_trigger_action;
    a_MM_Phrase->vtable.destroy = (MM_Phrase_Destroy_Fun) mm_phrase_destroy;
	 
    a_MM_Phrase->parent = NULL;
    a_MM_Phrase->successors = NULL;
	 a_MM_Phrase->n_allocated_successors = 0;
    a_MM_Phrase->n_successors = 0;
    a_MM_Phrase->action_keycode = 0;
    a_MM_Phrase->state = MM_Phrase_In_Progress;
	 
    return a_MM_Phrase;
}

static void mm_phrase_free(MM_Phrase *a_This) {
    a_This->vtable.destroy(a_This);

    free(a_This);
}

/*****************************************************************************
 Notes 
*****************************************************************************/

typedef struct {
	
	MM_Phrase phrase_inventory;
	 
	keypos_t keypos;
	 
} MM_Note;

static uint8_t mm_note_subphrase_consider_keychange(	
											MM_Note *a_This,
											uint16_t keycode, 
											keyrecord_t *record) 
{
	/* Only react on pressed keys 
	 */
	if(!record->event.pressed) return;
	
	assert(a_This->phrase_inventory->state == MM_Phrase_In_Progress);
	
	/* Set state appropriately 
	 */
	if(mm_keypos_equal(&a_This->keypos, &record->event.key)) {
		a_This->phrase_inventory->state = MM_Phrase_Completed;
	}
	else {
		a_This->phrase_inventory->state = MM_Phrase_Invalid;
	}
	
	return a_This->phrase_inventory->state;
}

static MM_Note *mm_note_alloc(){
    return (MM_Note*)malloc(sizeof(MM_Note));
}

static MM_Note *mm_note_new(MM_Note *a_note)
{
    /* Explict call to parent constructor 
	  */
    mm_phrase_new((MM_Phrase*)a_note);

    a_note->vtable.successor_consider_keychange 
		= (MM_Phrase_Subphrase_Consider_Keychange_Fun) mm_note_subphrase_consider_keychange;
    a_note->vtable.trigger_action 
		= (MM_Phrase_Trigger_Action_Fun) mm_note_trigger_action;
    a_note->vtable.destroy 
		= (MM_Phrase_Destroy_Fun) mm_note_destroy;
		
	 a_note->keypos.row = 100;
	 a_note->keypos.col = 100;
	 
    return a_note;
}

static MM_Note *mm_init_and_register_note(
									MM_Phrase *end_of_melody, 
									keypos_t keypos)
{
    MM_Note *a_note = (MM_Note*)mm_note_new(mm_note_alloc());
	
	 a_note->phrase_inventory.parent = end_of_melody;
	 
	 a_note->keypos = keypos;
	 
	 mm_phrase_add_successor(end_of_melody, a_note);
	 
	 /* Return the new end of the melody */
	 return a_note;
}

/*****************************************************************************
 Chords 
*****************************************************************************/

typedef struct {
	
	MM_Phrase phrase_inventory;
	 
	uint8_t n_members;
	keypos_t *keypos;
	bool *member_active;
	uint8_t n_chord_keys_pressed;
	 
} MM_Chord;

static uint8_t mm_chord_subphrase_consider_keychange(	
											MM_Chord *a_This,
											uint16_t keycode, 
											keyrecord_t *record) 
{
	bool key_part_of_chord = false;
	uint8_t i;
	
	assert(a_This->n_members != 0);
	
	/* Only react on pressed keys 
	 */
	if(!record->event.pressed) return;
	
	assert(a_This->phrase_inventory->state == MM_Phrase_In_Progress);
	
	/* Check it the key is part of the current chord 
	 */
	for(i = 0; i < a_This->n_members; ++i) {
		
		if(mm_keypos_equal(a_This->keypos[i] == record->event.key) {
			
			key_part_of_chord = true;
			
			if(record->event.pressed) {
				if(!member_active[i]) {
					member_active[i] = true;
					++n_chord_keys_pressed;
				}
			}
			else {
				if(member_active[i]) {
					member_active[i] = false;
					--n_chord_keys_pressed;
				}
			}
			break;
		}
	}
	
	if(!key_part_of_chord) {
		if(record->event.pressed) {
			a_This->phrase_inventory->state = MM_Phrase_Invalid;
			return a_This->phrase_inventory->state;
		}
	}
	
	if(n_chord_keys_pressed == n_members) {
		
		/* Chord completed
		 */
		a_This->phrase_inventory->state = MM_Phrase_Completed;
	}
	
	return a_This->phrase_inventory->state;
}

static void mm_chord_reset(MM_Chord *a_This) 
{
	uint8_t i;
	
	mm_phrase_reset(a_This);
	
	a_This->n_chord_keys_pressed = 0;
	
	for(i = 0; i < a_This->n_members; ++i) {
		a_This->member_active[i] = false;
	}
}

static void mm_chord_deallocate_member_storage(MM_Chord *a_This) {	
	
	if(a_This->keypos) {
		free(a_This->keypos);
		a_This->keypos = NULL;
	}
	if(a_This->member_active) {
		free(a_This->member_active);
		a_This->member_active = NULL;
	}
}

static void mm_chord_resize(MM_Chord *a_This, 
							uint8_t n_members)
{
	mm_chord_deallocate_member_storage(a_This);
	
	a_This->n_members = n_members;
	a_This->keypos = (keypos_t *)malloc(n_members*sizeof(keypos_t));
	a_This->member_active = (bool *)malloc(n_members*sizeof(bool));
	a_This->n_chord_keys_pressed = 0;
	
	for(i = 0; i < n_members; ++i) {
		a_This->member_active[i] = false;
	}
}

static MM_Chord* mm_chord_destroy(MM_Chord *a_This) {
	
	mm_chord_deallocate_member_storage(a_This);

	return a_This;
}

static MM_Chord *mm_chord_alloc(){
    return (MM_Chord*)malloc(sizeof(MM_Chord));
}

static MM_Chord *mm_chord_new(MM_Chord *a_chord){
    
	/* Explict call to parent constructor
	 */
	mm_phrase_new((MM_Phrase*)a_chord);

	a_chord->vtable.successor_consider_keychange 
		= (MM_Phrase_Subphrase_Consider_Keychange_Fun) mm_chord_subphrase_consider_keychange;
	a_chord->vtable.reset 
		= (MM_Phrase_Reset_Fun) mm_chord_reset;
	a_chord->vtable.trigger_action 
		= (MM_Phrase_Trigger_Action_Fun) mm_chord_trigger_action;
	a_chord->vtable.destroy 
		= (MM_Phrase_Destroy_Fun) mm_chord_destroy;
		
	/* Initialize the chord
	 */
	a_chord->n_members = 0;
	a_chord->keypos = NULL;
	a_chord->member_active = NULL;
	a_chord->n_chord_keys_pressed = 0;

	return a_chord;
}

static MM_Chord *mm_init_and_register_chord(
									MM_Phrase *end_of_melody, 
									keypos_t *keypos,
									uint8_t n_members)
{
	int i;
	
    MM_Chord *a_chord = (MM_Chord*)mm_chord_new(mm_chord_alloc());
	
	 a_chord->phrase_inventory.parent = end_of_melody;
	 
	 mm_chord_resize(a_chord, n_members);
	 
	for(i = 0; i < n_members; ++i) {
		a_chord->keypos[i] = keypos[i];
	}
	
	 mm_phrase_add_successor(end_of_melody, a_chord);
	 
	 /* Return the new end of the melody */
	 return a_chord;
}

/*****************************************************************************
 Clusters 
*****************************************************************************/

typedef struct {
	
	MM_Phrase phrase_inventory;
	 
	uint8_t n_members;
	keypos_t *keypos;
	bool *member_active;
	uint8_t n_cluster_keys_pressed;
	 
} MM_Cluster;

static uint8_t mm_cluster_subphrase_consider_keychange(	
											MM_Cluster *a_This,
											uint16_t keycode, 
											keyrecord_t *record) 
{
	bool key_part_of_cluster = false;
	uint8_t i;
	
	assert(a_This->n_members != 0);
	
	/* Only react on pressed keys 
	 */
	if(!record->event.pressed) return;
	
	assert(a_This->phrase_inventory->state == MM_Phrase_In_Progress);
	
	/* Check it the key is part of the current chord 
	 */
	for(i = 0; i < a_This->n_members; ++i) {
		
		if(mm_keypos_equal(a_This->keypos[i] == record->event.key)) {
			
			key_part_of_cluster = true;
			
			if(record->event.pressed) {
				if(!member_active[i]) {
					member_active[i] = true;
					++n_cluster_keys_pressed;
				}
			}
			/* Note: We do not care for released keys here. Every cluster member must be pressed only once
			 */

			break;
		}
	}
	
	if(!key_part_of_cluster) {
		if(record->event.pressed) {
			a_This->phrase_inventory->state = MM_Phrase_Invalid;
			return a_This->phrase_inventory->state;
		}
	}
	
	if(n_cluster_keys_pressed == n_members) {
		
		/* Cluster completed
		 */
		a_This->phrase_inventory->state = MM_Phrase_Completed;
	}
	
	return a_This->phrase_inventory->state;
}

static void mm_custer_reset(MM_Cluster *a_This) 
{
	uint8_t i;
	
	mm_phrase_reset(a_This);
	
	a_This->n_cluster_keys_pressed = 0;
	
	for(i = 0; i < a_This->n_members; ++i) {
		a_This->member_active[i] = false;
	}
}

static void mm_cluster_deallocate_member_storage(MM_Cluster *a_This) {	
	
	if(a_This->keypos) {
		free(a_This->keypos);
		a_This->keypos = NULL;
	}
	if(a_This->member_active) {
		free(a_This->member_active);
		a_This->member_active = NULL;
	}
}

static void mm_cluster_resize(MM_Cluster *a_This, 
							uint8_t n_members)
{
	mm_cluster_deallocate_member_storage(a_This);
	
	a_This->n_members = n_members;
	a_This->keypos = (keypos_t *)malloc(n_members*sizeof(keypos_t));
	a_This->member_active = (bool *)malloc(n_members*sizeof(bool));
	a_This->n_chord_keys_pressed = 0;
	
	for(i = 0; i < n_members; ++i) {
		a_This->member_active[i] = false;
	}
}

static MM_Cluster* mm_cluster_destroy(MM_Cluster *a_This) {
	
	mm_cluster_deallocate_member_storage(a_This);

    return a_This;
}

static MM_Cluster *mm_cluster_alloc(){
    return (MM_Cluster*)malloc(sizeof(MM_Cluster));
}

static MM_Cluster *mm_cluster_new(MM_Cluster *a_cluster) {
	
	/*Explict call to parent constructor
	*/
	mm_phrase_new((MM_Phrase*)a_cluster);

	a_cluster->vtable.successor_consider_keychange 
		= (MM_Phrase_Subphrase_Consider_Keychange_Fun) mm_cluster_subphrase_consider_keychange;
	a_cluster->vtable.reset 
		= (MM_Phrase_Reset_Fun) mm_custer_reset;
	a_cluster->vtable.trigger_action 
		= (MM_Phrase_Trigger_Action_Fun) mm_cluster_trigger_action;
	a_cluster->vtable.destroy 
		= (MM_Phrase_Destroy_Fun) mm_cluster_destroy;
	
	a_cluster->n_members = 0;
	a_cluster->keypos = NULL;
	a_cluster->member_active = NULL;
	a_cluster->n_cluster_keys_pressed = 0;
	
	return a_cluster;
}

static MM_Chord *mm_init_and_register_cluster(
									MM_Phrase *end_of_melody, 
									keypos_t *keypos,
									uint8_t n_members)
{
	int i;
	
    MM_Cluster *a_cluster = (MM_Cluster*)mm_cluster_new(mm_cluster_alloc());
	
	 a_cluster->phrase_inventory.parent = end_of_melody;
	 
	 mm_cluster_resize(a_cluster, n_members);
	 	
	 for(i = 0; i < n_members; ++i) {
		a_cluster->keypos[i] = keypos[i];
	}
	 mm_phrase_add_successor(end_of_melody, a_cluster);
	 
	 /* Return the new end of the melody */
	 return a_cluster;
}

/*****************************************************************************
 * Public access functions for creation and destruction of the melody tree
 *****************************************************************************/

void *mm_init_melody() {

	if(!mm_state.melody_root_initialized) {
		
		/* Initialize the melody root
		 */
		mm_phrase_new(&mm_state.melody_root);
		
		mm_state.melody_root_initialized = true;
	}
	
	return &mm_state.melody_root;
}

void mm_finalize() {
	mm_phrase_free_successors(mm_get_melody_root());
}

void *mm_add_note(	
									void *end_of_melody__, 
									keypos_t keypos, 
									uint8_t n_successors)
{
    MM_Note *a_note = (MM_Note*)mm_init_and_register_note(end_of_melody, keypos);
	 
	 MM_Phrase *end_of_melody = (MM_Phrase *)end_of_melody__;
	
	 mm_phrase_allocate_successors(a_note, n_successors);
	 
	 /* Return the new end of the melody */
	 return a_note;
}

void *mm_add_terminal_note(
									void *end_of_melody__, 
									keypos_t keypos,
									uint16_t action_keycode)
{
    MM_Note *a_note = (MM_Note*)mm_init_and_register_note(end_of_melody, keypos);
	
	 MM_Phrase *end_of_melody = (MM_Phrase *)end_of_melody__;
	 
	 a_note->phrase_inventory.action_keycode = action_keycode;
	 
	 /* Return the new end of the melody */
	 return a_note;
}

void *mm_add_chord(
									void *end_of_melody__, 
									keypos_t *keypos,
									uint8_t n_members,
									uint8_t n_successors)
{
    MM_Chord *a_chord = (MM_Chord*)mm_chord_nmm_chord_new(end_of_melody, keypos, n_members);
	 
	 MM_Phrase *end_of_melody = (MM_Phrase *)end_of_melody__;
	 
	 mm_phrase_allocate_successors(a_chord, n_successors);
	 
	 /* Return the new end of the melody */
	 return a_chord;
}

void *mm_add_terminal_chord(void *end_of_melody__, 
								  keypos_t *keypos,
									uint8_t n_members,
								  uint16_t action_keycode)
{
    MM_Chord *a_chord = (MM_Chord*)mm_chord_nmm_chord_new(end_of_melody, keypos, n_members);
	
	 MM_Phrase *end_of_melody = (MM_Phrase *)end_of_melody__;
	 
	 a_chord->phrase_inventory.action_keycode = action_keycode;
	 
	 /* Return the new end of the melody */
	 return a_note;
}

void *mm_add_cluster(
									void *end_of_melody__, 
									keypos_t *keypos,
									uint8_t n_members,
									uint8_t n_successors)
{
    MM_Cluster *a_cluster = (MM_Cluster*)mm_init_and_register_cluster(end_of_melody, keypos, n_members);

	 MM_Phrase *end_of_melody = (MM_Phrase *)end_of_melody__;
	 
	 mm_phrase_allocate_successors(a_cluster, n_successors);
	 
	 /* Return the new end of the melody */
	 return a_cluster;
}

void *mm_add_terminal_cluster(void *end_of_melody__, 
									keypos_t *keypos,
									uint8_t n_members,
								  uint16_t action_keycode)
{
    MM_Cluster *a_cluster = (MM_Cluster*)mm_init_and_register_cluster(end_of_melody, keypos, n_members);

	 MM_Phrase *end_of_melody = (MM_Phrase *)end_of_melody__;
	 
	 a_cluster->phrase_inventory.action_keycode = action_keycode;
	 
	 /* Return the new end of the melody */
	 return a_note;
}

static void mm_reset_magic_melodies()
{		
	if(!mm_state.current_phrase) { return; }
	
	/* The frase could not be parsed. Reset any previous phrases.
	*/
	mm_phrase_reset_recursively(mm_state.current_phrase);
	
	/* Cleanup and issue all keypresses as if they happened without parsing a melody
	*/
	mm_cleanup();
}

bool process_magic_melodies(uint16_t keycode, keyrecord_t *record)
{
	/* When a melody could not be finished, all keystrokes are
	 * processed through the process_record_quantum method.
	 * To prevent infinite recursion, we have to temporarily disable 
	 * processing magic melodies.
	 */
	if(mm_state.magic_melodies_temporarily_disabled) { return true; }
	
	if(!mm_state.melody_root_initialized) { return true; }
	
	/* Check if the melody is aborted
	 */
	if(	(mm_state.abort_keypos.row == record->event.key.row)
		&&	(mm_state.abort_keypos.col == record->event.key.col)) {
		
		/* If a melody is in progress, we abort it and consume the abort key.
		 */
		if(mm_state.current_phrase) {
			mm_abort_magic_melody();
			return false;
		}
	
		return true;
	}
	
	if(!mm_state.current_phrase) {
		mm_state.current_phrase = &mm_state.melody_root;
	}
	
	uint8_t result 
		= mm_phrase_consider_keychange(	&mm_state.current_phrase,
											keycode,
											record);
		
	switch(result) {
		case MM_Phrase_In_Progress:
		case MM_Phrase_Completed:
			return false;
		case MM_Phrase_Invalid:
			return true;
	}
	
	return true;
}

/* Use this function to define a keypos that always aborts a magic melody
 */
void mm_set_abort_keypos(keypos_t keypos)
{
	mm_state.abort_keypos = keypos;
}

#endif /*ifdef MAGIC_MELODIES_ENABLE*/
