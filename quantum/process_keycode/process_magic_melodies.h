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
#ifndef PROCESS_MAGIC_MELODIES_H
#define PROCESS_MAGIC_MELODIES_H

#ifdef MAGIC_MELODIES_ENABLE

#include "quantum.h"
#include "tmk_core/common/keyboard.h"

void *mm_create_note(keypos_t keypos, 
							uint16_t action_keycode);

void *mm_create_chord(	keypos_t *keypos,
								uint8_t n_members, 
								uint16_t action_keycode);

void *mm_create_cluster(
									keypos_t *keypos,
									uint8_t n_members, 
									uint16_t action_keycode);

void mm_add_melody(int count, ...);

/* This method simplifies defining node lines or tap dances
 */
void mm_add_note_line(uint16_t action_keycode, 
							 int count, ...);

void mm_add_tap_dance(uint16_t action_keycode, 
							 int n_taps, 
							 keypos_t curKeypos);

void mm_set_abort_keypos(keypos_t keypos);

void mm_set_timeout_ms(uint16_t timeout);

#define MM_NO_ACTION 0

#define MM_KEYPOS(ROW, COL) (keypos_t){.row = ROW, .col = COL}

/* Is finalization required?
 */
void mm_finalize_magic_melodies(void);

/* Call this function from process_record_user
 */
bool mm_process_magic_melodies(uint16_t keycode, keyrecord_t *record);

/* Add this function to your implementation of matrix_scan_user.
 * It returns true on timeout.
 */
bool mm_check_timeout(void);

#endif /*ifdef MAGIC_MELODIES_ENABLE*/

#endif
