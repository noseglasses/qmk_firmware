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

void *mm_init_melody();

void *mm_add_note(	
									void *end_of_melody, 
									keypos_t keypos, 
									uint8_t n_successors);

void *mm_add_terminal_note(
									void *end_of_melody, 
									keypos_t keypos,
									uint16_t action_keycode);

void *mm_add_chord(
									void *end_of_melody, 
									keypos_t *keypos,
									uint8_t n_members,
									uint8_t n_successors);

void *mm_add_terminal_chord(void *end_of_melody, 
								  keypos_t *keypos,
									uint8_t n_members,
								  uint16_t action_keycode);

void *mm_add_cluster(
									void *end_of_melody, 
									keypos_t *keypos,
									uint8_t n_members,
									uint8_t n_successors);

void *mm_add_terminal_cluster(void *end_of_melody,  
									keypos_t *keypos,
									uint8_t n_members,
								  uint16_t action_keycode);

void mm_set_abort_keypos(keypos_t keypos);

/* Do not call functions defined below this line when defining keymaps. 
 * Those methods are reserved for quantums internal use.
 */

/* Is finalization required? 
 */
void finalize_magic_melody();

bool process_magic_melodies(uint16_t keycode, keyrecord_t *record)

#endif /*ifdef MAGIC_MELODIES_ENABLE*/

#endif
