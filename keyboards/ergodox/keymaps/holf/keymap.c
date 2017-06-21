#define PERMISSIVE_HOLD

#include "ergodox.h"
#include "debug.h"
#include "action_layer.h"
#include "version.h"

#include "keymap_german.h"

#include "keymap_nordic.h"

#include "process_magic_melodies.h"

enum custom_keycodes {
  PLACEHOLDER = SAFE_RANGE, // can always be here
  EPRM,
  VRSN,
  RGB_SLD,
  
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [0] = KEYMAP(KC_GRAVE,KC_1,KC_2,KC_3,KC_4,KC_5,LCTL(KC_X),KC_EQUAL,KC_Q,KC_W,KC_D,KC_F,KC_K,LCTL(KC_C),KC_DELETE,KC_A,ALT_T(KC_S),CTL_T(KC_E),SFT_T(KC_T),LT(1,KC_G),MO(1),KC_Z,KC_X,KC_C,ALGR_T(KC_V),KC_B,LCTL(KC_V),TG(1),KC_WWW_BACK,KC_WWW_FORWARD,KC_LEFT,KC_RIGHT,KC_F1,KC_F3,KC_HOME,KC_BSPACE,SFT_T(KC_TAB),KC_END,KC_TRANSPARENT,KC_6,KC_7,KC_8,KC_9,KC_0,KC_MINUS,KC_F8,KC_J,KC_U,KC_R,KC_L,KC_SCOLON,KC_BSLASH,LT(1,KC_Y),SFT_T(KC_N),CTL_T(KC_I),ALT_T(KC_O),LT(2,KC_H),KC_QUOTE,KC_F7,KC_P,ALGR_T(KC_M),KC_COMMA,KC_DOT,KC_SLASH,MO(1),KC_UP,KC_DOWN,KC_LBRACKET,KC_RBRACKET,TG(3),KC_F2,KC_ESCAPE,KC_PGUP,KC_PGDOWN,SFT_T(KC_ENTER),KC_SPACE),

  [1] = KEYMAP(M(0),KC_F1,KC_F2,KC_F3,KC_F4,KC_F5,KC_TRANSPARENT,KC_TRANSPARENT,KC_EXLM,KC_AT,KC_LCBR,KC_RCBR,KC_PIPE,KC_LALT,KC_TRANSPARENT,KC_HASH,KC_DLR,KC_LPRN,KC_RPRN,KC_TRANSPARENT,KC_TRANSPARENT,KC_PERC,KC_CIRC,KC_LBRACKET,KC_RBRACKET,KC_TILD,KC_RALT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_F1,KC_F3,KC_HOME,KC_BSPACE,KC_TAB,KC_END,KC_TRANSPARENT,KC_F6,KC_F7,KC_F8,KC_F9,KC_F10,KC_F11,KC_TRANSPARENT,KC_UP,KC_KP_7,KC_KP_8,KC_KP_9,KC_ASTR,KC_F12,KC_TRANSPARENT,KC_KP_4,KC_KP_5,KC_KP_6,KC_PLUS,KC_TRANSPARENT,KC_TRANSPARENT,KC_AMPR,KC_KP_1,KC_KP_2,KC_KP_3,KC_BSLASH,KC_TRANSPARENT,KC_TRANSPARENT,KC_DOT,KC_KP_0,KC_EQUAL,KC_TRANSPARENT,KC_F2,KC_ESCAPE,KC_PGUP,KC_PGDOWN,KC_ENTER,KC_SPACE),

  [2] = KEYMAP(KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_MS_UP,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_MS_LEFT,KC_MS_DOWN,KC_MS_RIGHT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_MS_BTN1,KC_MS_BTN2,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_MEDIA_PLAY_PAUSE,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_MEDIA_PREV_TRACK,KC_MEDIA_NEXT_TRACK,KC_TRANSPARENT,KC_TRANSPARENT,KC_AUDIO_VOL_UP,KC_AUDIO_VOL_DOWN,KC_AUDIO_MUTE,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_WWW_BACK),

  [3] = KEYMAP(KC_GRAVE,KC_1,KC_2,KC_3,KC_4,KC_5,KC_TRANSPARENT,KC_EQUAL,KC_Q,KC_W,KC_E,KC_R,KC_T,KC_TRANSPARENT,KC_DELETE,KC_A,KC_S,KC_D,KC_F,KC_G,KC_LSHIFT,KC_Z,KC_X,KC_C,KC_V,KC_B,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_HOME,KC_BSPACE,KC_TAB,KC_END,KC_TRANSPARENT,KC_6,KC_7,KC_8,KC_9,KC_0,KC_MINUS,KC_TRANSPARENT,KC_Y,KC_U,KC_I,KC_O,KC_P,KC_BSLASH,KC_H,KC_J,KC_K,KC_L,KC_SCOLON,KC_TRANSPARENT,KC_TRANSPARENT,KC_N,KC_M,KC_COMMA,KC_DOT,KC_SLASH,KC_RSHIFT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_TRANSPARENT,KC_ESCAPE,KC_PGUP,KC_PGDOWN,KC_ENTER,KC_SPACE),

};

typedef struct {
	uint8_t DDRD__;
	uint8_t DDRB__;
	uint8_t PORTD__;
	uint8_t PORTB__;
} Led_Registers;

static Led_Registers led_registers;

#define SAFE_REG(S) led_registers.S##__ = S;
#define RESTORE_REG(S) S = led_registers.S##__;

static void ff_safe_led_state(void)
{
	SAFE_REG(DDRD)
	SAFE_REG(DDRB)
	SAFE_REG(PORTD)
	SAFE_REG(PORTB)
}

static void ff_restore_led_state(void)
{
	RESTORE_REG(DDRD)
	RESTORE_REG(DDRB)
	RESTORE_REG(PORTD)
	RESTORE_REG(PORTB)
}

static void ff_led_signal(void)
{
	ff_safe_led_state();
	
	ergodox_led_all_on();
	
	wait_ms (500);
	
	ff_restore_led_state();
}

#define FF_LED_FLASH(LED_ID) \
	ergodox_right_led_##LED_ID##_on(); \
	wait_ms(100);	\
	ergodox_right_led_##LED_ID##_off();

static void ff_led_flash(void)
{
	ff_safe_led_state();
	
	ergodox_led_all_off();
	
	FF_LED_FLASH(1)
	FF_LED_FLASH(2)
	FF_LED_FLASH(3)
	
	ff_restore_led_state();
}

static void ff_led_superflash(void)
{
	ff_led_flash();
	wait_ms(200);
	ff_led_flash();
	wait_ms(200);
	ff_led_flash();
}

const uint16_t PROGMEM fn_actions[] = {
  [1] = ACTION_LAYER_TAP_TOGGLE(1),
  [2] = ACTION_FUNCTION(2),
  [3] = ACTION_FUNCTION(3),
  [4] = ACTION_FUNCTION(4),
  [5] = ACTION_FUNCTION(5),
  [6] = ACTION_FUNCTION(6),
  [7] = ACTION_FUNCTION(7)
};

void action_function(keyrecord_t *record, uint8_t id, uint8_t opt) {

  switch (id) {
    case 2:
		 ff_led_signal();
		 uprintf("f2\n");
		 break;
    case 3:
		 uprintf("f3\n");
		 ff_led_flash();
		 break;
    case 4:
		 uprintf("chord and cluster\n");
		 ff_led_superflash();
		 break;
    case 5:
		 uprintf("noteline\n");
		 ff_led_superflash();
		 break;
    case 6:
		 uprintf("double tap\n");
		 ff_led_superflash();
		 break;
    case 7:
		 uprintf("simplified tap dance\n");
		 ff_led_superflash();
		 break;
  }
}

// leaving this in place for compatibilty with old keymaps cloned and re-compiled.
const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
      switch(id) {
        case 0:
        if (record->event.pressed) {
          SEND_STRING (QMK_KEYBOARD "/" QMK_KEYMAP " @ " QMK_VERSION);
        }
        break;
      }
    return MACRO_NONE;
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	
// 	uprintf("process_record_user\n");
// 	uprintf("   keycode: %d\n", keycode);
// 	uprintf("   pressed: %d\n", record->event.pressed);
// 	uprintf("   row: %d\n", record->event.key.row);
// 	uprintf("   col: %d\n", record->event.key.col);
	
	bool mm_process_result = mm_process_magic_melodies(keycode, record);
	
	if(!mm_process_result) { return false; }
	
  switch (keycode) {
    // dynamically generate these.
    case EPRM:
      if (record->event.pressed) {
        eeconfig_init();
      }
      return false;
      break;
    case VRSN:
      if (record->event.pressed) {
        SEND_STRING (QMK_KEYBOARD "/" QMK_KEYMAP " @ " QMK_VERSION);
      }
      return false;
      break;
    case RGB_SLD:
      if (record->event.pressed) {
        rgblight_mode(1);
      }
      return false;
      break;
    
  }
  return true;
}

#define Key_52 MM_KEYPOS(0x2, 0x5)
#define Key_5B MM_KEYPOS(0xB, 0x5)
#define Key_5A MM_KEYPOS(0xA, 0x5)

#define CHORD_KEY_1 MM_KEYPOS(0x2, 0x2)
#define CHORD_KEY_2 MM_KEYPOS(0x3, 0x2)
#define CHORD_KEY_3 MM_KEYPOS(0x4, 0x2)

#define CLUSTER_KEY_1 MM_KEYPOS(0x9, 0x2)
#define CLUSTER_KEY_2 MM_KEYPOS(0xA, 0x2)
#define CLUSTER_KEY_3 MM_KEYPOS(0xB, 0x2)

#define NODE_LINE_KEY_1 MM_KEYPOS(0x2, 0x1)
#define NODE_LINE_KEY_2 MM_KEYPOS(0x3, 0x1)
#define NODE_LINE_KEY_3 MM_KEYPOS(0x4, 0x1)

#define MM_ABORT_KEY MM_KEYPOS(0x6, 0x5)

 void init_magic_melodies(void)
{
	mm_set_abort_keypos(MM_ABORT_KEY);
	
	mm_set_timeout_ms(20000);
	
	mm_add_melody(
		2,
		mm_create_note(Key_52, MM_NO_ACTION),
		mm_create_note(Key_5B, F(2) /* led signal */)
	);
	
	mm_add_melody(
		2,
		mm_create_note(Key_52, MM_NO_ACTION),
		mm_create_note(Key_5A, F(3) /* led flash */)
	);
	
	keypos_t chord_keys[3] 
		= {	CHORD_KEY_1,
				CHORD_KEY_2,
				CHORD_KEY_3
			};
	
	keypos_t cluster_keys[3] 
		= {	CLUSTER_KEY_1,
				CLUSTER_KEY_2,
				CLUSTER_KEY_3
			};
			
	mm_add_melody(
		2,
		mm_create_chord(
			chord_keys,
// 			sizeof(chord_keys),
							 3,
			MM_NO_ACTION
		),
		mm_create_cluster(
			cluster_keys,
// 			sizeof(cluster_keys),
							 3,
			F(4) /* super flash */
		)
	);
	
	mm_add_note_line(
		F(5),
		3,
		NODE_LINE_KEY_1,
		NODE_LINE_KEY_2,
		NODE_LINE_KEY_3
	);

	/* Node lines can also contain the same key several times
	 */
	mm_add_note_line(
		F(6),
		3,
		NODE_LINE_KEY_1,
		NODE_LINE_KEY_1,
		NODE_LINE_KEY_2
	);
	
	mm_add_tap_dance(
		F(7),
		3,
		NODE_LINE_KEY_1
	);
}

void matrix_init_user(void) {
	
// 	 uprintf("matrix_init_user\n");
	
	init_magic_melodies();
}

void matrix_scan_user(void) {
	 
    uint8_t layer = biton32(layer_state);

    ergodox_board_led_off();
    ergodox_right_led_1_off();
    ergodox_right_led_2_off();
    ergodox_right_led_3_off();
    switch (layer) {
        case 1:
            ergodox_right_led_1_on();
            break;
        case 2:
            ergodox_right_led_2_on();
            break;
        case 3:
            ergodox_right_led_3_on();
            break;
        case 4:
            ergodox_right_led_1_on();
            ergodox_right_led_2_on();
            break;
        case 5:
            ergodox_right_led_1_on();
            ergodox_right_led_3_on();
            break;
        case 6:
            ergodox_right_led_2_on();
            ergodox_right_led_3_on();
            break;
        case 7:
            ergodox_right_led_1_on();
            ergodox_right_led_2_on();
            ergodox_right_led_3_on();
            break;
        default:
            break;
    }
    
    mm_check_timeout();
};
