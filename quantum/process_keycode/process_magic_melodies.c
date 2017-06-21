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

#ifdef MAGIC_MELODIES_ENABLE

#include "process_magic_melodies.h"
#include "assert.h"
#include <inttypes.h>

#ifdef DEBUG_MAGIC_MELODIES
#include "debug.h"
#else
#include "nodebug.h"
#endif

#define MM_MAX_KEYCHANGES 100

#define MM_DEFAULT_KEYPRESS_TIMEOUT 200

#ifdef DEBUG_MAGIC_MELODIES
#define MM_PRINTF(...) uprintf(__VA_ARGS__)
#else
#define MM_PRINTF(...)
#endif

/* This function is defined in quantum/keymap_common.c 
 */
action_t action_for_configured_keycode(uint16_t keycode);

static bool mm_keypos_equal(keypos_t kp1, keypos_t kp2)
{
	MM_PRINTF("kp1: %d, %d\n", kp1.row, kp1.col);
	MM_PRINTF("kp2: %d, %d\n", kp2.row, kp2.col);
	
	return 		(kp1.row == kp2.row)
				&&	(kp1.col == kp2.col);
}

struct MM_PhraseStruct;

typedef uint8_t (*MM_Phrase_Consider_Keychange_Fun)(	struct MM_PhraseStruct *a_This, 
														uint16_t keycode, 
														keyrecord_t *record);

typedef uint8_t (*MM_Phrase_Successor_Consider_Keychange_Fun)(
														struct MM_PhraseStruct *a_This, 
														uint16_t keycode, 
														keyrecord_t *record);

typedef void (*MM_Phrase_Reset_Fun)(	struct MM_PhraseStruct *a_This);

typedef void (*MM_Phrase_Trigger_Action_Fun)(	struct MM_PhraseStruct *a_This);

typedef struct MM_PhraseStruct * (*MM_Phrase_Destroy_Fun)(struct MM_PhraseStruct *a_This);

typedef bool (*MM_Phrase_Equals_Fun)(struct MM_PhraseStruct *p1, struct MM_PhraseStruct *p2);

typedef void (*MM_Phrase_Print_Self_Fun)(struct MM_PhraseStruct *p);

typedef struct {
	
	MM_Phrase_Consider_Keychange_Fun 
									consider_keychange;
									
	MM_Phrase_Successor_Consider_Keychange_Fun 
									successor_consider_keychange;
									
	MM_Phrase_Reset_Fun
									reset;
									
	MM_Phrase_Trigger_Action_Fun
									trigger_action;
									
	MM_Phrase_Destroy_Fun	
									destroy;
			
	MM_Phrase_Equals_Fun
									equals;
									
	MM_Phrase_Print_Self_Fun
									print_self;
									
} MM_Phrase_Vtable;

typedef struct MM_PhraseStruct {
	
    MM_Phrase_Vtable *vtable;
	 
	 struct MM_PhraseStruct *parent;
	 
	 struct MM_PhraseStruct **successors;
	 
	 uint8_t n_allocated_successors;
	 uint8_t n_successors;
	 
	 uint16_t action_keycode;
	 
	 uint8_t state;
	 
} MM_Phrase;

typedef struct
{
  uint16_t n_keys_changed;
  bool pressed[MM_MAX_KEYCHANGES];
  keypos_t keypos[MM_MAX_KEYCHANGES];
  uint16_t keytimes[MM_MAX_KEYCHANGES];
  
  MM_Phrase melody_root;
  bool melody_root_initialized;
  
  MM_Phrase *current_phrase;
  
  bool magic_melodies_temporarily_disabled;
  
  keypos_t abort_keypos;
  
  uint16_t time_last_keypress;
  
  uint16_t keypress_timeout;
  
} MM_Magic_Melody_State;

static MM_Magic_Melody_State mm_state = 
{
	.n_keys_changed = 0,
	.melody_root_initialized = false,
	.current_phrase = NULL,
	.magic_melodies_temporarily_disabled = false,
	.abort_keypos = { .row = 100, .col = 100 },
	.keypress_timeout = MM_DEFAULT_KEYPRESS_TIMEOUT
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
	
	mm_state.pressed[mm_state.n_keys_changed] = record->event.pressed;
	mm_state.keypos[mm_state.n_keys_changed] = record->event.key;
	mm_state.keytimes[mm_state.n_keys_changed] = record->event.time;
	
	++mm_state.n_keys_changed;
}

static void mm_flush_stored_keychanges(void)
{
	if(mm_state.n_keys_changed == 0) { return; }
	
	/* Process all events as if they had happened just now */
	
	mm_state.magic_melodies_temporarily_disabled = true;
       
	uint16_t cur_time = timer_read();
	
	uint16_t actualOffset = cur_time - mm_state.keytimes[mm_state.n_keys_changed - 1];
	
	for(uint16_t i = 0; i < mm_state.n_keys_changed; ++i) {
		
		keyrecord_t record;
		
		record.event.time = mm_state.keytimes[i] + actualOffset;
		record.event.key = mm_state.keypos[i];
		record.event.pressed = mm_state.pressed[i];
		
		process_record_quantum(&record);
	}
	
	mm_state.magic_melodies_temporarily_disabled = false;
}

static void mm_cleanup(void)
{
	mm_flush_stored_keychanges();
	
	mm_state.n_keys_changed = 0;
	mm_state.current_phrase = NULL;
}

static void mm_phrase_reset(	MM_Phrase *a_This)
{
	a_This->state = MM_Phrase_In_Progress;
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
	for(uint8_t i = 0; i < a_This->n_successors; ++i) {
		a_This->successors[i]->vtable->reset(a_This->successors[i]);
	}
		
	if(a_This->parent) {
		mm_phrase_reset_recursively(a_This->parent);
	}		
}
	
static void mm_abort_magic_melody(void)
{		
	if(!mm_state.current_phrase) { return; }
	
	MM_PRINTF("Aborting magic melody\n");
	
	/* The frase could not be parsed. Reset any previous phrases.
	*/
	mm_phrase_reset_recursively(mm_state.current_phrase);
	
	/* Cleanup and issue all keypresses as if they happened without parsing a melody
	*/
	mm_cleanup();
}

static uint8_t mm_phrase_consider_keychange(	MM_Phrase **current_phrase,
												uint16_t keycode, 
												keyrecord_t *record) 
{
	/* Loop over all phrases and inform them about the 
	 * keychange 
	 */
	bool all_successors_invalid = true;
	
	MM_Phrase *a_current_phrase = *current_phrase;
	
	mm_store_keychange(keycode, record);
	
	MM_PRINTF("Processing key\n");
	
	for(uint8_t i = 0; i < a_current_phrase->n_successors; ++i) {
		
		a_current_phrase->successors[i]->vtable->print_self(
			a_current_phrase->successors[i]);
		
		if(a_current_phrase->successors[i]->state == MM_Phrase_Invalid) {
			continue;
		}
		
		uint8_t successor_process_result 
			= a_current_phrase->successors[i]
					->vtable->successor_consider_keychange(	
								a_current_phrase->successors[i], 
								keycode,
								record
						);
								
		switch(successor_process_result) {
			
			case MM_Phrase_In_Progress:
				all_successors_invalid = false;
				
				MM_PRINTF("Phrase in progress\n");
				
				break;
				
			case MM_Phrase_Completed:
				
				MM_PRINTF("Phrase completed\n");
				
				/* The successor that is first completed becomes the 
				* new current phrase
				*/
				*current_phrase = a_current_phrase->successors[i];
				a_current_phrase = *current_phrase;
					
				/* The melody is finished. Trigger the associated action.
				 */
				if(0 == a_current_phrase->n_successors) {
					
					a_current_phrase->vtable->trigger_action(a_current_phrase);
					
					mm_phrase_reset_recursively(a_current_phrase->parent);
					
					mm_state.current_phrase = NULL;
					
					MM_PRINTF("Melody completed\n");
					
					return MM_Phrase_Completed;
				}
				else {
					
					return MM_Phrase_In_Progress;
				}
				break;
			case MM_Phrase_Invalid:
				MM_PRINTF("Phrase invalid\n");
				break;
		}
	}
		
	if(all_successors_invalid) {
		
		MM_PRINTF("Melody invalid\n");
		
		mm_abort_magic_melody();
					
		return MM_Phrase_Invalid;
	}
	
	return MM_Phrase_In_Progress;
}

static void mm_phrase_trigger_action(MM_Phrase *a_MM_Phrase) {
	
	if(a_MM_Phrase->action_keycode != 0) {
		
		/* Construct a dummy record
		 */
		keyrecord_t record;
			record.event.key.row = 0;
			record.event.key.col = 0;
			record.event.pressed = true;
			record.event.time = timer_read();
		
		uint16_t configured_keycode = keycode_config(a_MM_Phrase->action_keycode);
		
		action_t action = action_for_configured_keycode(configured_keycode); 
    
		process_action(&record, action);
	}
}

static void mm_phrase_allocate_successors(MM_Phrase *a_This, uint8_t n_successors) {

	 a_This->successors 
		= (struct MM_PhraseStruct **)malloc(n_successors*sizeof(struct MM_PhraseStruct*));
	 a_This->n_allocated_successors = n_successors;
}

static void mm_phrase_grow_successors(MM_Phrase *a_This) {

	if(a_This->n_allocated_successors == 0) {
		
		mm_phrase_allocate_successors(a_This, 1);
	}
	else {
		MM_Phrase **oldSucessors = a_This->successors;
		
		mm_phrase_allocate_successors(a_This, 2*a_This->n_allocated_successors);
			
		for(uint8_t i = 0; i < a_This->n_successors; ++i) {
			a_This->successors[i] = oldSucessors[i];
		}
		
		free(oldSucessors); 
	}
}

static void mm_phrase_add_successor(MM_Phrase *a_This, MM_Phrase *successor) {
	
	if(a_This->n_allocated_successors == a_This->n_successors) {
		mm_phrase_grow_successors(a_This);
	}
	
	a_This->successors[a_This->n_successors] = successor;
	
	successor->parent = a_This;
	
	++a_This->n_successors;
}

static void mm_phrase_free(MM_Phrase *a_This);

static void mm_phrase_free_successors(MM_Phrase *a_This)
{
	if(!a_This->successors) { return; }
	
	for(uint8_t i = 0; i < a_This->n_allocated_successors; ++i) {
		
		mm_phrase_free(a_This->successors[i]);
	}
	
	free(a_This->successors);
	
	a_This->successors = NULL;
	a_This->n_allocated_successors = 0;
}

static MM_Phrase* mm_phrase_destroy(MM_Phrase *a_MM_Phrase) {

	mm_phrase_free_successors(a_MM_Phrase);
	
	return a_MM_Phrase;
}

static bool mm_phrase_equals(MM_Phrase *p1, MM_Phrase *p2) 
{
	if(p1->vtable != p2->vtable) { return false; }
	
	return p1->vtable->equals(p1, p2);
}

static void mm_phrase_free(MM_Phrase *a_This) {
    a_This->vtable->destroy(a_This);

    free(a_This);
}

static MM_Phrase* mm_phrase_get_equivalent_successor(
														MM_Phrase *a_MM_Phrase,
														MM_Phrase *sample) {
	
	if(a_MM_Phrase->n_successors == 0) { return NULL; }
	
	for(uint8_t i = 0; i < a_MM_Phrase->n_successors; ++i) {
		if(mm_phrase_equals(
											a_MM_Phrase->successors[i], 
											sample)
		  ) {
			return a_MM_Phrase->successors[i];
		}
	}
	
	return NULL;
}

static void mm_phrase_print_self(MM_Phrase *p)
{
	MM_PRINTF("phrase (0x%" PRIXPTR ")\n", (void*)p);
	MM_PRINTF("   parent: 0x%" PRIXPTR "\n", (void*)&p->parent);
	MM_PRINTF("   successors: 0x%" PRIXPTR "\n", (void*)&p->successors);
	MM_PRINTF("   n_allocated_successors: %d\n", p->n_allocated_successors);
	MM_PRINTF("   n_successors: %d\n", p->n_successors);
	MM_PRINTF("   action_keycode: %d\n", p->action_keycode);
	MM_PRINTF("   state: %d\n", p->state);
}

static MM_Phrase_Vtable mm_phrase_vtable =
{
	.consider_keychange 
		= (MM_Phrase_Consider_Keychange_Fun) mm_phrase_consider_keychange,
	.successor_consider_keychange 
		= NULL,
	.reset 
		= (MM_Phrase_Reset_Fun) mm_phrase_reset,
	.trigger_action 
		= (MM_Phrase_Trigger_Action_Fun) mm_phrase_trigger_action,
	.destroy 
		= (MM_Phrase_Destroy_Fun) mm_phrase_destroy,
	.equals
		= NULL,
	.print_self
		= (MM_Phrase_Print_Self_Fun) mm_phrase_print_self,
};

static MM_Phrase *mm_phrase_new(MM_Phrase *a_MM_Phrase) {
	
    a_MM_Phrase->vtable = &mm_phrase_vtable;
	 
    a_MM_Phrase->parent = NULL;
    a_MM_Phrase->successors = NULL;
	 a_MM_Phrase->n_allocated_successors = 0;
    a_MM_Phrase->n_successors = 0;
    a_MM_Phrase->action_keycode = 0;
    a_MM_Phrase->state = MM_Phrase_In_Progress;
	 
    return a_MM_Phrase;
}

/*****************************************************************************
 Notes 
*****************************************************************************/

typedef struct {
	
	MM_Phrase phrase_inventory;
	 
	keypos_t keypos;
	 
} MM_Note;

static uint8_t mm_note_successor_consider_keychange(	
											MM_Note *a_This,
											uint16_t keycode, 
											keyrecord_t *record) 
{
	/* Only react on pressed keys 
	 */
	if(!record->event.pressed) return MM_Phrase_In_Progress;
	
	assert(a_This->phrase_inventory.state == MM_Phrase_In_Progress);
	
	/* Set state appropriately 
	 */
	if(mm_keypos_equal(a_This->keypos, record->event.key)) {
		MM_PRINTF("note successfully finished\n");
		a_This->phrase_inventory.state = MM_Phrase_Completed;
	}
	else {
		MM_PRINTF("note invalid\n");
		a_This->phrase_inventory.state = MM_Phrase_Invalid;
	}
	
	return a_This->phrase_inventory.state;
}

static MM_Note *mm_note_alloc(void) {
    return (MM_Note*)malloc(sizeof(MM_Note));
}

static bool mm_note_equals(MM_Note *n1, MM_Note *n2) 
{
	return mm_keypos_equal(n1->keypos, n2->keypos);
}

static void mm_note_print_self(MM_Note *p)
{
	mm_phrase_print_self((MM_Phrase*)p);
	
	MM_PRINTF("note\n");
	MM_PRINTF("   row: %d\n", p->keypos.row);
	MM_PRINTF("   col: %d\n", p->keypos.col);
}

static MM_Phrase_Vtable mm_note_vtable =
{
	.consider_keychange 
		= (MM_Phrase_Consider_Keychange_Fun) mm_phrase_consider_keychange,
	.successor_consider_keychange 
		= (MM_Phrase_Successor_Consider_Keychange_Fun) mm_note_successor_consider_keychange,
	.reset 
		= (MM_Phrase_Reset_Fun) mm_phrase_reset,
	.trigger_action 
		= (MM_Phrase_Trigger_Action_Fun) mm_phrase_trigger_action,
	.destroy 
		= (MM_Phrase_Destroy_Fun) mm_phrase_destroy,
	.equals
		= (MM_Phrase_Equals_Fun) mm_note_equals,
	.print_self
		= (MM_Phrase_Print_Self_Fun) mm_note_print_self
};

static MM_Note *mm_note_new(MM_Note *a_note)
{
    /* Explict call to parent constructor 
	  */
    mm_phrase_new((MM_Phrase*)a_note);

    a_note->phrase_inventory.vtable = &mm_note_vtable;
		
	 a_note->keypos.row = 100;
	 a_note->keypos.col = 100;
	 
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

static uint8_t mm_chord_successor_consider_keychange(	
											MM_Chord *a_This,
											uint16_t keycode, 
											keyrecord_t *record) 
{
	bool key_part_of_chord = false;
	
	assert(a_This->n_members != 0);
	
	assert(a_This->phrase_inventory.state == MM_Phrase_In_Progress);
	
	/* Check it the key is part of the current chord 
	 */
	for(uint8_t i = 0; i < a_This->n_members; ++i) {
		
		if(mm_keypos_equal(a_This->keypos[i], record->event.key)) {
			
			key_part_of_chord = true;
			
			if(record->event.pressed) {
				if(!a_This->member_active[i]) {
					a_This->member_active[i] = true;
					++a_This->n_chord_keys_pressed;
				}
			}
			else {
				if(a_This->member_active[i]) {
					a_This->member_active[i] = false;
					--a_This->n_chord_keys_pressed;
				}
			}
			break;
		}
	}
	
	if(!key_part_of_chord) {
		if(record->event.pressed) {
			a_This->phrase_inventory.state = MM_Phrase_Invalid;
			return a_This->phrase_inventory.state;
		}
	}
	
	if(a_This->n_chord_keys_pressed == a_This->n_members) {
		
		/* Chord completed
		 */
		a_This->phrase_inventory.state = MM_Phrase_Completed;
	}
	
	return a_This->phrase_inventory.state;
}

static void mm_chord_reset(MM_Chord *a_This) 
{
	mm_phrase_reset((MM_Phrase*)a_This);
	
	a_This->n_chord_keys_pressed = 0;
	
	for(uint8_t i = 0; i < a_This->n_members; ++i) {
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
	
	for(uint8_t i = 0; i < n_members; ++i) {
		a_This->member_active[i] = false;
	}
}

static MM_Chord* mm_chord_destroy(MM_Chord *a_This) {
	
	mm_chord_deallocate_member_storage(a_This);

	return a_This;
}

static MM_Chord *mm_chord_alloc(void){
    return (MM_Chord*)malloc(sizeof(MM_Chord));
}

static bool mm_chord_equals(MM_Chord *c1, MM_Chord *c2) 
{
	if(c1->n_members != c2->n_members) { return false; }
	
	for(uint8_t i = 0; i < c1->n_members; ++i) {
		if(!mm_keypos_equal(c1->keypos[i], c2->keypos[i])) { return false; }
	}
	
	return true;
}

static void mm_chord_print_self(MM_Chord *c)
{
	mm_phrase_print_self((MM_Phrase*)c);
	
	MM_PRINTF("chord\n");
	MM_PRINTF("   n_members: %d\n", c->n_members);
	MM_PRINTF("   n_chord_keys_pressed: %d\n", c->n_chord_keys_pressed);
	
	for(uint8_t i = 0; i < c->n_members; ++i) {
		MM_PRINTF("      row: %d, col: %d, active: %d\n", 
				  c->keypos[i].row, c->keypos[i].col, c->member_active[i]);
	}
}

static MM_Phrase_Vtable mm_chord_vtable =
{
	.consider_keychange 
		= (MM_Phrase_Consider_Keychange_Fun) mm_phrase_consider_keychange,
	.successor_consider_keychange 
		= (MM_Phrase_Successor_Consider_Keychange_Fun) mm_chord_successor_consider_keychange,
	.reset 
		= (MM_Phrase_Reset_Fun) mm_chord_reset,
	.trigger_action 
		= (MM_Phrase_Trigger_Action_Fun) mm_phrase_trigger_action,
	.destroy 
		= (MM_Phrase_Destroy_Fun) mm_chord_destroy,
	.equals
		= (MM_Phrase_Equals_Fun) mm_chord_equals,
	.print_self
		= (MM_Phrase_Print_Self_Fun) mm_chord_print_self
};

static MM_Chord *mm_chord_new(MM_Chord *a_chord){
    
	/* Explict call to parent constructor
	 */
	mm_phrase_new((MM_Phrase*)a_chord);

	a_chord->phrase_inventory.vtable = &mm_chord_vtable;
		
	/* Initialize the chord
	 */
	a_chord->n_members = 0;
	a_chord->keypos = NULL;
	a_chord->member_active = NULL;
	a_chord->n_chord_keys_pressed = 0;

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

static uint8_t mm_cluster_successor_consider_keychange(	
											MM_Cluster *a_This,
											uint16_t keycode, 
											keyrecord_t *record) 
{
	bool key_part_of_cluster = false;
	
	assert(a_This->n_members != 0);
	
	/* Only react on pressed keys. Allow cluster members to be released. 
	 * Every key only must be pressed once.
	 */
	if(!record->event.pressed) { return MM_Phrase_In_Progress; }
	
	assert(a_This->phrase_inventory.state == MM_Phrase_In_Progress);
	
	/* Check it the key is part of the current chord 
	 */
	for(uint8_t i = 0; i < a_This->n_members; ++i) {
		
		if(mm_keypos_equal(a_This->keypos[i], record->event.key)) {
			
			key_part_of_cluster = true;
			
			if(record->event.pressed) {
				if(!a_This->member_active[i]) {
					a_This->member_active[i] = true;
					++a_This->n_cluster_keys_pressed;
				}
			}
			/* Note: We do not care for released keys here. Every cluster member must be pressed only once
			 */

			break;
		}
	}
	
	if(!key_part_of_cluster) {
		if(record->event.pressed) {
			a_This->phrase_inventory.state = MM_Phrase_Invalid;
			return a_This->phrase_inventory.state;
		}
	}
	
	if(a_This->n_cluster_keys_pressed == a_This->n_members) {
		
		/* Cluster completed
		 */
		a_This->phrase_inventory.state = MM_Phrase_Completed;
	}
	
	return a_This->phrase_inventory.state;
}

static void mm_cluster_reset(MM_Cluster *a_This) 
{
	mm_phrase_reset((MM_Phrase*)a_This);
	
	a_This->n_cluster_keys_pressed = 0;
	
	for(uint8_t i = 0; i < a_This->n_members; ++i) {
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
	a_This->n_cluster_keys_pressed = 0;
	
	for(uint8_t i = 0; i < n_members; ++i) {
		a_This->member_active[i] = false;
	}
}

static bool mm_is_cluster_member(MM_Cluster *c, keypos_t key)
{
	for(uint8_t i = 0; i < c->n_members; ++i) {
		if(mm_keypos_equal(c->keypos[i], key)) {
			return true;
		}
	}
	
	return false;
}

static bool mm_cluster_equals(MM_Cluster *c1, MM_Cluster *c2) 
{
	if(c1->n_members != c2->n_members) { return false; }
	
	for(uint8_t i = 0; i < c1->n_members; ++i) {
		if(!mm_is_cluster_member(c1, c2->keypos[i])) { return false; }
	}
	
	return true;
}

static MM_Cluster* mm_cluster_destroy(MM_Cluster *a_This) {
	
	mm_cluster_deallocate_member_storage(a_This);

    return a_This;
}

static MM_Cluster *mm_cluster_alloc(void){
    return (MM_Cluster*)malloc(sizeof(MM_Cluster));
} 

static void mm_cluster_print_self(MM_Cluster *c)
{
	mm_phrase_print_self((MM_Phrase*)c);
	
	MM_PRINTF("cluster\n");
	MM_PRINTF("   n_members: %d\n", c->n_members);
	MM_PRINTF("   n_cluster_keys_pressed: %d\n", c->n_cluster_keys_pressed);
	
	for(uint8_t i = 0; i < c->n_members; ++i) {
		MM_PRINTF("      row: %d, col: %d, active: %d\n", 
				  c->keypos[i].row, c->keypos[i].col, c->member_active[i]);
	}
}

static MM_Phrase_Vtable mm_cluster_vtable =
{
	.consider_keychange 
		= (MM_Phrase_Consider_Keychange_Fun) mm_phrase_consider_keychange,
	.successor_consider_keychange 
		= (MM_Phrase_Successor_Consider_Keychange_Fun) mm_cluster_successor_consider_keychange,
	.reset 
		= (MM_Phrase_Reset_Fun) mm_cluster_reset,
	.trigger_action 
		= (MM_Phrase_Trigger_Action_Fun) mm_phrase_trigger_action,
	.destroy 
		= (MM_Phrase_Destroy_Fun) mm_cluster_destroy,
	.equals
		= (MM_Phrase_Equals_Fun) mm_cluster_equals,
	.print_self
		= (MM_Phrase_Print_Self_Fun) mm_cluster_print_self
};

static MM_Cluster *mm_cluster_new(MM_Cluster *a_cluster) {
	
	/*Explict call to parent constructor
	*/
	mm_phrase_new((MM_Phrase*)a_cluster);

	a_cluster->phrase_inventory.vtable = &mm_cluster_vtable;
	
	a_cluster->n_members = 0;
	a_cluster->keypos = NULL;
	a_cluster->member_active = NULL;
	a_cluster->n_cluster_keys_pressed = 0;
	
	return a_cluster;
}

/*****************************************************************************
 * Public access functions for creation and destruction of the melody tree
 *****************************************************************************/

static void mm_init(void) {

	if(!mm_state.melody_root_initialized) {
		
		/* Initialize the melody root
		 */
		mm_phrase_new(&mm_state.melody_root);
		
		mm_state.melody_root_initialized = true;
	}
}

void mm_finalize_magic_melodies(void) {
	mm_phrase_free_successors(&mm_state.melody_root);
}

void *mm_create_note(keypos_t keypos, 
							uint16_t action_keycode)
{
    MM_Note *a_note = (MM_Note*)mm_note_new(mm_note_alloc());
	 
	 a_note->keypos = keypos;
	 a_note->phrase_inventory.action_keycode = action_keycode;
	 
	 /* Return the new end of the melody */
	 return a_note;
}

void *mm_create_chord(	keypos_t *keypos,
								uint8_t n_members, 
								uint16_t action_keycode)
{
	MM_Chord *a_chord = (MM_Chord*)mm_chord_new(mm_chord_alloc());
	 	 
	mm_chord_resize(a_chord, n_members);
	 
	for(uint8_t i = 0; i < n_members; ++i) {
		a_chord->keypos[i] = keypos[i];
	}
	 
	a_chord->phrase_inventory.action_keycode = action_keycode;
	 
	/* Return the new end of the melody */
	return a_chord;
}

void *mm_create_cluster(
									keypos_t *keypos,
									uint8_t n_members, 
									uint16_t action_keycode)
{
	MM_Cluster *a_cluster = (MM_Cluster*)mm_cluster_new(mm_cluster_alloc());
	 
	mm_cluster_resize(a_cluster, n_members);
	 	
	for(uint8_t i = 0; i < n_members; ++i) {
		a_cluster->keypos[i] = keypos[i];
	}
	 
	a_cluster->phrase_inventory.action_keycode = action_keycode;
	
	/* Return the new end of the melody */
	return a_cluster;
}

static void mm_add_melody_from_list(MM_Phrase **phrases,
												int n_phrases)
{ 
	MM_Phrase *parent_phrase = &mm_state.melody_root;
	
	MM_PRINTF("Adding magic melody with %d members\n", n_phrases);
	
	for (int i = 0; i < n_phrases; i++) { 
		
		MM_Phrase *curPhrase = phrases[i];
		
		MM_Phrase *equivalent_successor 
			= mm_phrase_get_equivalent_successor(parent_phrase, curPhrase);
			
		MM_PRINTF("   member %d: ", i);
		
		if(equivalent_successor) {
			
			MM_PRINTF("already present\n");
			
			parent_phrase = equivalent_successor;
			
			/* The successor is already registered in the search tree. Delete the newly created version.
			 */			
			mm_phrase_free(curPhrase);
		}
		else {
			
			MM_PRINTF("newly defined\n");
			
			mm_phrase_add_successor(parent_phrase, curPhrase);
			
			parent_phrase = curPhrase;
		}
	}
}

void mm_add_melody(int count, ...)
{ 
	mm_init();
	
	va_list ap;

	va_start (ap, count);         /* Initialize the argument list. */
  	
	MM_Phrase **phrases 
		= (MM_Phrase **)malloc(count*sizeof(MM_Phrase *));
	
	for (int i = 0; i < count; i++) { 
		
		phrases[i] = va_arg (ap, MM_Phrase *);
	}
	
	mm_add_melody_from_list(phrases, count);
	
	free(phrases);

  va_end (ap);                  /* Clean up. */
}

void mm_add_note_line(uint16_t action_keycode, int count, ...)
{
	mm_init();
	
	va_list ap;

	va_start (ap, count);         /* Initialize the argument list. */
  
	MM_Phrase **phrases 
		= (MM_Phrase **)malloc(count*sizeof(MM_Phrase *));
		
	for (int i = 0; i < count; i++) {
		
		keypos_t curKeypos = va_arg (ap, keypos_t); 
		
		uint16_t a_action_keycode = MM_NO_ACTION;
		
		if(i == (count - 1)) {
			a_action_keycode = action_keycode;
		}
		
		void *new_note = mm_create_note(curKeypos,
												  a_action_keycode);
		
		phrases[i] = (MM_Phrase *)new_note;
	}
	
	mm_add_melody_from_list(phrases, count);
	
	free(phrases);

  va_end (ap);                  /* Clean up. */
}

void mm_add_tap_dance(uint16_t action_keycode, 
							 int n_taps, 
							 keypos_t curKeypos)
{
	MM_Phrase **phrases 
		= (MM_Phrase **)malloc(n_taps*sizeof(MM_Phrase *));
		
	for (int i = 0; i < n_taps; i++) {
		
		uint16_t a_action_keycode = MM_NO_ACTION;
		
		if(i == (n_taps - 1)) {
			a_action_keycode = action_keycode;
		}
		
		void *new_note = mm_create_note(curKeypos,
												  a_action_keycode);
		
		phrases[i] = (MM_Phrase *)new_note;
	}
	
	mm_add_melody_from_list(phrases, n_taps);
	
	free(phrases);
}

bool mm_check_timeout(void)
{
	if(mm_state.current_phrase
		&& (timer_elapsed(mm_state.time_last_keypress) 
				> mm_state.keypress_timeout)
	  ) {
		
		MM_PRINTF("Magic melody timeout hit\n");
	
		/* Too late...
			*/
		mm_abort_magic_melody();
	
		return true;
	}
	
	return false;
}

bool mm_process_magic_melodies(uint16_t keycode, keyrecord_t *record)
{
	/* When a melody could not be finished, all keystrokes are
	 * processed through the process_record_quantum method.
	 * To prevent infinite recursion, we have to temporarily disable 
	 * processing magic melodies.
	 */
	if(mm_state.magic_melodies_temporarily_disabled) { return true; }
	
	/* Early exit if no melody was registered 
	 */
	if(!mm_state.melody_root_initialized) { return true; }
			
	/* If there is no melody processed, we ignore keyups.
	*/	
	if(!mm_state.current_phrase && !record->event.pressed) {
		MM_PRINTF("Keyup ignored as no melody currently processed\n");
		return true;
	}
	
	/* Check if the melody is being aborted
	 */
	if(mm_keypos_equal(mm_state.abort_keypos, record->event.key)) {
		
		/* If a melody is in progress, we abort it and consume the abort key.
		 */
		if(mm_state.current_phrase) {
			MM_PRINTF("Processing melodies interrupted by user\n");
			mm_abort_magic_melody();
			return false;
		}
	
		return true;
	}
	
	if(!mm_state.current_phrase) {
		
		mm_state.current_phrase = &mm_state.melody_root;
		mm_state.time_last_keypress = timer_read();
	}
	else {
		
		if(mm_check_timeout()) {
			
			/* Timeout hit. Cleanup already done.
			 */
			return false;
		}
		else {
			mm_state.time_last_keypress = timer_read();
		}
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

void mm_set_timeout_ms(uint16_t timeout)
{
	mm_state.keypress_timeout = timeout;
}

#endif /*ifdef MAGIC_MELODIES_ENABLE*/
