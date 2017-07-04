#define PERMISSIVE_HOLD

#include "ergodox.h"
#include "debug.h"
#include "action_layer.h"
#include "version.h"

#include "keymap_german.h"

#include "keymap_nordic.h"

#include "process_papageno.h"

enum custom_keycodes {
  PLACEHOLDER = SAFE_RANGE, // can always be here
  EPRM,
  VRSN,
  RGB_SLD,
};

enum ff_layers {
	ff_layer_norman = 0,
	ff_layer_symbol,
	ff_layer_media,
	ff_layer_qwerty/*,
	ff_layer_aux*/
};

enum {
	ff_layer_base = 0
};

#define ___________ KC_TRANSPARENT

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

	/*Norman
	 */
  [ff_layer_norman] = KEYMAP(
      
      KC_GRAVE   ,KC_1       ,KC_2       ,KC_3       ,KC_4       ,KC_5       ,LCTL(KC_X) ,
      KC_EQUAL   ,KC_Q       ,KC_W       ,KC_D       ,KC_F       ,KC_K       ,LCTL(KC_C) ,
      KC_DELETE  ,KC_A       ,ALT_T(KC_S),CTL_T(KC_E),SFT_T(KC_T),LT(1,KC_G) ,
      MO(1)      ,KC_Z       ,KC_X       ,KC_C       ,ALGR_T(KC_V),KC_B      ,LCTL(KC_V) ,
      TG(1)      ,RESET      ,KC_WWW_FORWARD,KC_LEFT ,KC_RIGHT,
                                                                              KC_F1      ,KC_F3      ,
                                                                                          KC_HOME    ,
                                                                  KC_BSPACE  ,SFT_T(KC_TAB),KC_END   ,

                  ___________,KC_6       ,KC_7       ,KC_8       ,KC_9       ,KC_0       ,KC_MINUS   ,
                  KC_F8      ,KC_J       ,KC_U       ,KC_R       ,KC_L       ,KC_SCOLON  ,KC_BSLASH  ,
                              LT(1,KC_Y) ,SFT_T(KC_N),CTL_T(KC_I),ALT_T(KC_O),LT(2,KC_H) ,KC_QUOTE   ,
                  KC_F7      ,KC_P       ,ALGR_T(KC_M),KC_COMMA  ,KC_DOT     ,KC_SLASH   ,MO(1)      ,
                                          KC_UP      ,KC_DOWN    ,KC_LBRACKET,KC_RBRACKET,TG(3)      ,
      KC_F2      ,KC_ESCAPE  ,
      KC_PGUP    ,
      KC_PGDOWN  ,SFT_T(KC_ENTER),KC_SPACE
	),

	/*Symbol
	 */
  [ff_layer_symbol] = KEYMAP(
      M(0)       ,KC_F1      ,KC_F2      ,KC_F3      ,KC_F4      ,KC_F5      ,___________,
      ___________,KC_EXLM    ,KC_AT      ,KC_LCBR    ,KC_RCBR    ,KC_PIPE    ,KC_LALT    ,
      ___________,KC_HASH    ,KC_DLR     ,KC_LPRN    ,KC_RPRN    ,___________,
      ___________,KC_PERC    ,KC_CIRC    ,KC_LBRACKET,KC_RBRACKET,KC_TILD    ,KC_RALT    ,
      ___________,___________,___________,___________,___________,
                                                                              KC_F1      ,KC_F3      ,
                                                                                          KC_HOME    ,
                                                                  KC_BSPACE  ,KC_TAB     ,KC_END,

                  ___________,KC_F6      ,KC_F7      ,KC_F8      ,KC_F9      ,KC_F10     ,KC_F11     ,
                  ___________,KC_UP      ,KC_KP_7    ,KC_KP_8    ,KC_KP_9    ,KC_ASTR    ,KC_F12     ,
                              ___________,KC_KP_4    ,KC_KP_5    ,KC_KP_6    ,KC_PLUS    ,___________,
                  ___________,KC_AMPR    ,KC_KP_1    ,KC_KP_2    ,KC_KP_3    ,KC_BSLASH  ,___________,
                                          ___________,KC_DOT     ,KC_KP_0    ,KC_EQUAL   ,___________,

      KC_F2      ,KC_ESCAPE  ,
      KC_PGUP,
      KC_PGDOWN  ,KC_ENTER   ,KC_SPACE
	),

	/* Media
	 */
  [ff_layer_media] = KEYMAP(
      ___________,___________,___________,___________,___________,___________,___________,
      ___________,___________,___________,KC_MS_UP   ,___________,___________,___________,
      ___________,___________,KC_MS_LEFT ,KC_MS_DOWN ,KC_MS_RIGHT,___________,
      ___________,___________,___________,___________,___________,___________,___________,
      ___________,___________,___________,KC_MS_BTN1 ,KC_MS_BTN2 ,
                                                                              ___________,___________,
                                                                                          ___________,
                                                                  ___________,___________,___________,

                  ___________,___________,___________,___________,___________,___________,___________,
                  ___________,___________,___________,___________,___________,___________,___________,
                              ___________,___________,___________,___________,___________,KC_MEDIA_PLAY_PAUSE,
                  ___________,___________,___________,KC_MEDIA_PREV_TRACK,KC_MEDIA_NEXT_TRACK,___________,___________,
                                          KC_AUDIO_VOL_UP,KC_AUDIO_VOL_DOWN,KC_AUDIO_MUTE,___________,___________,
      ___________,___________,
      ___________,
      ___________,___________,KC_WWW_BACK),

	/* QWERTY
	 */
  [ff_layer_qwerty] = KEYMAP(
      KC_GRAVE   ,KC_1       ,KC_2        ,KC_3      ,KC_4       ,KC_5       ,___________,	
      KC_EQUAL   ,KC_Q       ,KC_W        ,KC_E      ,KC_R       ,KC_T       ,___________,
      KC_DELETE  ,KC_A       ,KC_S        ,KC_D      ,KC_F       ,KC_G       ,
      KC_LSHIFT  ,KC_Z       ,KC_X        ,KC_C      ,KC_V       ,KC_B       ,___________,
      ___________,___________,___________,___________,___________,
                                                                              ___________,___________,
                                                                                          KC_HOME    ,
                                                                  KC_BSPACE  ,KC_TAB     ,KC_END     ,

                  ___________,KC_6       ,KC_7       ,KC_8       ,KC_9       ,KC_0       ,KC_MINUS   ,
                  ___________,KC_Y       ,KC_U       ,KC_I       ,KC_O       ,KC_P       ,KC_BSLASH  ,
                              KC_H       ,KC_J       ,KC_K       ,KC_L       ,KC_SCOLON  ,___________,
                  ___________,KC_N       ,KC_M       ,KC_COMMA   ,KC_DOT     ,KC_SLASH   ,KC_RSHIFT  ,
                                          ___________,___________,___________,___________,___________,
      ___________,KC_ESCAPE,
      KC_PGUP    ,
      KC_PGDOWN  ,KC_ENTER ,KC_SPACE
	),
	
// 	[ff_layer_aux] = KEYMAP(
//       ___________,___________,___________,___________,___________,___________,___________,	
//       ___________,___________,___________,___________,___________,___________,___________,	
//       ___________,___________,___________,___________,___________,___________,
//       ___________,___________,___________,___________,___________,___________,___________,	
//       ___________,___________,___________,___________,___________,
//                                                                               ___________,___________,
//                                                                                           ___________,
//                                                                   ___________,___________,___________,
// 
//                   ___________,___________,___________,___________,___________,___________,___________,
//                   ___________,___________,___________,___________,___________,___________,___________,
//                               ___________,___________,___________,___________,___________,___________,
//                   ___________,___________,___________,___________,___________,___________,___________,
//                                           ___________,___________,___________,___________,___________,
//       ___________,___________,
//       ___________,
//       ___________,___________,___________
// 	),
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

void ff_led_signal(void)
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

void ff_led_flash(void)
{
	ff_safe_led_state();
	
	ergodox_led_all_off();
	
	FF_LED_FLASH(1)
	FF_LED_FLASH(2)
	FF_LED_FLASH(3)
	
	ff_restore_led_state();
}

void ff_led_superflash(void)
{
	ff_led_flash();
	wait_ms(200);
	ff_led_flash();
	wait_ms(200);
	ff_led_flash();
}

enum {
	ff_a_melody_1 = 2,
	ff_a_melody_2,
	ff_a_chord_and_cluster,
	ff_a_single_note_line,
	ff_a_single_note_line_double_key,
	ff_a_tap_dance,
	ff_a_single_chord,
	ff_a_single_cluster
};

#define FF_ADD_ACTION_FUNC(S) [S] = ACTION_FUNCTION(S)

const uint16_t PROGMEM fn_actions[] = {
	
  [1] = ACTION_LAYER_TAP_TOGGLE(1),
  
  FF_ADD_ACTION_FUNC(ff_a_melody_1),
  FF_ADD_ACTION_FUNC(ff_a_melody_2),
  FF_ADD_ACTION_FUNC(ff_a_chord_and_cluster),
  FF_ADD_ACTION_FUNC(ff_a_single_note_line),
  FF_ADD_ACTION_FUNC(ff_a_single_note_line_double_key),
  FF_ADD_ACTION_FUNC(ff_a_tap_dance),
  FF_ADD_ACTION_FUNC(ff_a_single_chord)
};

void action_function(keyrecord_t *record, uint8_t id, uint8_t opt) {
	
	/* Only react on keyup
	 */
	if(!record->event.pressed) { return; }

  switch (id) {
    case ff_a_melody_1:
		 //ff_led_signal();
		 PPG_PRINTF("melody 1\n");
		 break;
    case ff_a_melody_2:
		 PPG_PRINTF("melody 2\n");
		 //ff_led_flash();
		 break;
    case ff_a_chord_and_cluster:
		 PPG_PRINTF("chord and cluster\n");
		// ff_led_superflash();
		 break;
    case ff_a_single_note_line:
		 PPG_PRINTF("noteline\n");
		 //ff_led_superflash();
		 break;
    case ff_a_single_note_line_double_key:
		 PPG_PRINTF("single note line with double tap\n");
		// ff_led_superflash();
		 break;
    case ff_a_tap_dance:
		 PPG_PRINTF("tap dance\n");
		 //ff_led_superflash();
		 break;
    case ff_a_single_chord:
		 PPG_PRINTF("isolated chord\n");
		 //ff_led_superflash();
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
	
// 	PPG_PRINTF("process_record_user\n");
// 	PPG_PRINTF("   keycode: %d\n", keycode);
// 	PPG_PRINTF("   pressed: %d\n", record->event.pressed);
// 	PPG_PRINTF("   row: %d\n", record->event.key.row);
// 	PPG_PRINTF("   col: %d\n", record->event.key.col);is
	
	bool ppg_process_result = ppg_qmk_process_event(keycode, record);
	
	if(!ppg_process_result) { return false; }
	
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

static void the_cluster_callback(void *user_data)
{
	PPG_PRINTF("cluster callback: %d\n", (size_t)user_data);
}

#if 0
    /* left hand, spatial positions */                          
    k00,k01,k02,k03,k04,k05,k06,                                
    k10,k11,k12,k13,k14,k15,k16,                                
    k20,k21,k22,k23,k24,k25,                                    
    k30,k31,k32,k33,k34,k35,k36,                                
    k40,k41,k42,k43,k44,                                        
                            k55,k56,                            
                                k54,                            
                        k53,k52,k51,                            
                                                                
    /* right hand, spatial positions */                         
        k07,k08,k09,k0A,k0B,k0C,k0D,                            
        k17,k18,k19,k1A,k1B,k1C,k1D,                            
            k28,k29,k2A,k2B,k2C,k2D,                            
        k37,k38,k39,k3A,k3B,k3C,k3D,                            
                k49,k4A,k4B,k4C,k4D,                            
    k57,k58,                                                    
    k59,                                                        
    k5C,k5B,k5A
#endif

#define KEY_52 PPG_QMK_MATRIX_KEY_HEX(5, 2)
#define KEY_5B PPG_QMK_MATRIX_KEY_HEX(5, B)
#define KEY_5A PPG_QMK_MATRIX_KEY_HEX(5, A)

#define CHORD_KEY_1 PPG_QMK_MATRIX_KEY_HEX(2, 2)
#define CHORD_KEY_2 PPG_QMK_MATRIX_KEY_HEX(2, 3)
#define CHORD_KEY_3 PPG_QMK_MATRIX_KEY_HEX(2, 4)

#define CLUSTER_KEY_1 PPG_QMK_MATRIX_KEY_HEX(2, 9)
#define CLUSTER_KEY_2 PPG_QMK_MATRIX_KEY_HEX(2, A)
#define CLUSTER_KEY_3 PPG_QMK_MATRIX_KEY_HEX(2, B)

#define SINGLE_NOTE_LINE_KEY_1 PPG_QMK_MATRIX_KEY_HEX(1, 2)
#define SINGLE_NOTE_LINE_KEY_2 PPG_QMK_MATRIX_KEY_HEX(1, 3)
#define SINGLE_NOTE_LINE_KEY_3 PPG_QMK_MATRIX_KEY_HEX(1, 4)

#define PPG_ABORT_KEY PPG_QMK_MATRIX_KEY_HEX(5, 6)

void init_papageno(void)
{
	PPG_QMK_INIT
	
	ppg_set_abort_input(PPG_ABORT_KEY);
	
	ppg_qmk_set_timeout_ms(20000);
// 	ppg_qmk_set_timeout_ms(500);
	
	/* Magic melodies are inherited by higher layers unless
	 * overridden.
	 */ 
	
	PPG_PRINTF("Keycode is %u\n", F(ff_a_melody_1));
	
	/* Single note line magic melody: Left inner large thumb key followed by
	 * right inner large thumb key
	 */
	ppg_pattern(
		ff_layer_base, /* Layer id */
		PPG_TOKENS(
			ppg_create_note(KEY_52),
			ppg_token_set_action(
				ppg_create_note(KEY_5B),
				PPG_QMK_ACTION_KEYCODE(  
					F(ff_a_melody_1)
				)
			)
		)
	);
	
	/* Single note line magic melody: Left inner large thumb key followed by
	 * right inner large thumb key
	 */
	ppg_pattern(
		ff_layer_base, /* Layer id */
		PPG_TOKENS(
			ppg_create_note(KEY_52),
			ppg_token_set_action(
				ppg_create_note(KEY_5A),
				PPG_QMK_ACTION_KEYCODE(
					F(ff_a_melody_2)
				)
			)
		)
	);
			
	/* Melody specification: First a chord of the s, d and f keys (QWERTY) then a cluster of
	 * j, k and l (QWERTY).
	 */
	ppg_pattern(
		ff_layer_base, /* Layer id */
		PPG_TOKENS(
			PPG_CREATE_CHORD(
				CHORD_KEY_1,
				CHORD_KEY_2,
				CHORD_KEY_3
			),
			ppg_token_set_action(
				PPG_CREATE_CLUSTER(
					CLUSTER_KEY_1,
					CLUSTER_KEY_2,
					CLUSTER_KEY_3
				),
				PPG_QMK_ACTION_KEYCODE(
					F(ff_a_chord_and_cluster)
				)
			)
		)
	);
	
	/* A single note line of w, e and r (QWERTY).
	 */
	ppg_single_note_line(
		ff_layer_base,
		PPG_QMK_ACTION_KEYCODE(
			F(ff_a_single_note_line)
		),
		PPG_QMK_KEYS(
			SINGLE_NOTE_LINE_KEY_1,
			SINGLE_NOTE_LINE_KEY_2,
			SINGLE_NOTE_LINE_KEY_3
		)
	);

	/* Node lines can also contain the same key several times.
	 * Here: w->w->e (QWERTY)
	 */
	ppg_single_note_line(
		ff_layer_base,
		PPG_QMK_ACTION_KEYCODE(
			F(ff_a_single_note_line_double_key)
		),
		PPG_QMK_KEYS(
			SINGLE_NOTE_LINE_KEY_1,
			SINGLE_NOTE_LINE_KEY_1,
			SINGLE_NOTE_LINE_KEY_2
		)
	);
	
	/* Triple tap on q (QWERTY)
	 */
	ppg_tap_dance(
		ff_layer_base,
		SINGLE_NOTE_LINE_KEY_1, /* The tap key */
		PPG_Action_Fall_Back,
							/* Use PPG_Action_Fall_Back if you want fall back, 
								e.g. if something happens after three and five keypresses
								and you want to fall back to the three keypress action
								if only four keypresses arrived before timeout. */
		PPG_TAP_DEFINITIONS(
			PPG_TAP(3, 
					  PPG_QMK_ACTION_KEYCODE(
						  F(ff_a_tap_dance)
					  )
			)
		)
	);
	
	/* Single chord of left thumb inner large key, right thumb both large keys
	 */
	ppg_chord(
		ff_layer_qwerty,
		PPG_QMK_ACTION_KEYCODE(
			F(ff_a_single_chord)
		),
		PPG_QMK_KEYS(
			KEY_52,
			KEY_5B,
			KEY_5A
		)
	);
	
	/* A single cluster of j, k and l (QWERTY).
	 */
	ppg_cluster(
		ff_layer_qwerty,
		PPG_ACTION_USER_CALLBACK(
			the_cluster_callback,
			(void*)(size_t)13 /*user data*/
		),
		PPG_QMK_KEYS(
			CLUSTER_KEY_1,
			CLUSTER_KEY_2,
			CLUSTER_KEY_3
		)
	);
	
   #define FF_BACK_LINE_1 PPG_QMK_MATRIX_KEY_HEX(4, 2)
   #define FF_BACK_LINE_2 PPG_QMK_MATRIX_KEY_HEX(4, 3)
   #define FF_BACK_LINE_3 PPG_QMK_MATRIX_KEY_HEX(4, 4)
   #define FF_BACK_LINE_4 PPG_QMK_MATRIX_KEY_HEX(4, 9)
   #define FF_BACK_LINE_5 PPG_QMK_MATRIX_KEY_HEX(4, A)
   #define FF_BACK_LINE_6 PPG_QMK_MATRIX_KEY_HEX(4, B)
	
	/* A magic melody to switch to the aux layer.
	 */
	ppg_single_note_line(
		ff_layer_base,
		PPG_QMK_ACTION_KEYCODE(
			TG(ff_layer_qwerty)
		),
			PPG_QMK_KEYS(
				FF_BACK_LINE_1,
				FF_BACK_LINE_2,
				FF_BACK_LINE_3,
				FF_BACK_LINE_4,
				FF_BACK_LINE_5,
				FF_BACK_LINE_6
			)
	);

	PPG_PRINTF("Setup completed\n");
}

void matrix_init_user(void) {	
	init_papageno();
}

void matrix_scan_user(void) {
	 
// 	PPG_PRINTF("Matrix scan user\n");
	
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
    
    ppg_check_timeout();
};
