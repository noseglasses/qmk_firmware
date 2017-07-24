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
   
//    [ff_layer_aux] = KEYMAP(
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
//    ),
};


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

// Define a bunch of key position alias that are going to be used to define 
// Papageno inputs. 
//
// Important: Every key macro must feature 
//            the auxiliary macro parameter S
//
#define KEY_52(S) PPG_QMK_KEYPOS_HEX(5, 2, S)
#define KEY_5B(S) PPG_QMK_KEYPOS_HEX(5, B, S)
#define KEY_5A(S) PPG_QMK_KEYPOS_HEX(5, A, S)

#define CHORD_KEY_1(S) PPG_QMK_KEYPOS_HEX(2, 2, S)
#define CHORD_KEY_2(S) PPG_QMK_KEYPOS_HEX(2, 3, S)
#define CHORD_KEY_3(S) PPG_QMK_KEYPOS_HEX(2, 4, S)

#define CLUSTER_KEY_1(S) PPG_QMK_KEYPOS_HEX(2, 9, S)
#define CLUSTER_KEY_2(S) PPG_QMK_KEYPOS_HEX(2, A, S)
#define CLUSTER_KEY_3(S) PPG_QMK_KEYPOS_HEX(2, B, S)

#define SINGLE_NOTE_LINE_KEY_1(S) PPG_QMK_KEYPOS_HEX(1, 2, S)
#define SINGLE_NOTE_LINE_KEY_2(S) PPG_QMK_KEYPOS_HEX(1, 3, S)
#define SINGLE_NOTE_LINE_KEY_3(S) PPG_QMK_KEYPOS_HEX(1, 4, S)

#define PPG_ABORT_KEY(S) PPG_QMK_KEYPOS_HEX(5, 6, S)

#define FF_BACK_LINE_1(S) PPG_QMK_KEYPOS_HEX(4, 2, S)
#define FF_BACK_LINE_2(S) PPG_QMK_KEYPOS_HEX(4, 3, S)
#define FF_BACK_LINE_3(S) PPG_QMK_KEYPOS_HEX(4, 4, S)
#define FF_BACK_LINE_4(S) PPG_QMK_KEYPOS_HEX(4, 9, S)
#define FF_BACK_LINE_5(S) PPG_QMK_KEYPOS_HEX(4, A, S)
#define FF_BACK_LINE_6(S) PPG_QMK_KEYPOS_HEX(4, B, S)

// Define a set of Papageno inputs that are associated with
// keyboard matrix positions.
//
// Important: - The macro must be named PPG_QMK_MATRIX_POSITION_INPUTS!
//            - If no inputs are supposed to be associated with
//              matrix positions, define an empty PPG_QMK_MATRIX_POSITION_INPUTS
//
#define PPG_QMK_MATRIX_POSITION_INPUTS(OP) \
__NL__      OP(KEY_52) \
__NL__      OP(KEY_5B) \
__NL__      OP(KEY_5A) \
__NL__      \
__NL__      OP(CHORD_KEY_1) \
__NL__      OP(CHORD_KEY_2) \
__NL__      OP(CHORD_KEY_3) \
__NL__      \
__NL__      OP(CLUSTER_KEY_1) \
__NL__      OP(CLUSTER_KEY_2) \
__NL__      OP(CLUSTER_KEY_3) \
__NL__      \
__NL__      OP(SINGLE_NOTE_LINE_KEY_1) \
__NL__      OP(SINGLE_NOTE_LINE_KEY_2) \
__NL__      OP(SINGLE_NOTE_LINE_KEY_3) \
__NL__      \
__NL__      OP(PPG_ABORT_KEY) \
__NL__      \
__NL__      OP(FF_BACK_LINE_1) \
__NL__      OP(FF_BACK_LINE_2) \
__NL__      OP(FF_BACK_LINE_3) \
__NL__      OP(FF_BACK_LINE_4) \
__NL__      OP(FF_BACK_LINE_5) \
__NL__      OP(FF_BACK_LINE_6)

// Define a bunch of qmk keycode alias that are going to
// be used to define Papageno inputs.
//
#define FF_KC_7 KC_7
#define FF_KC_8 KC_8
#define FF_KC_9 KC_9

#define FF_KC_A KC_A
#define FF_KC_B KC_B
   
// Define a set of Papageno inputs that are associated with
// qmk keycodes.
//
// Important: - The macro must be named PPG_QMK_KEYCODE_INPUTS!
//            - If no inputs are supposed to be associated with
//              keycodes, define an empty PPG_QMK_KEYCODE_INPUTS
//
#define PPG_QMK_KEYCODE_INPUTS(OP) \
__NL__      \
__NL__      OP(FF_KC_7) \
__NL__      OP(FF_KC_8) \
__NL__      OP(FF_KC_9) \
\
__NL__      OP(FF_KC_A) \
__NL__      OP(FF_KC_B)  
   
// Initialize Papageno data structures for qmk
//
PPG_QMK_INIT_DATA_STRUCTURES

enum {
   ff_a_pattern_1 = 2,
   ff_a_pattern_2,
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
  
  FF_ADD_ACTION_FUNC(ff_a_pattern_1),
  FF_ADD_ACTION_FUNC(ff_a_pattern_2),
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
    case ff_a_pattern_1:
       uprintf("pattern 1\n");
       break;
    case ff_a_pattern_2:
       uprintf("pattern 2\n");
       break;
    case ff_a_chord_and_cluster:
       uprintf("chord and cluster\n");
       break;
    case ff_a_single_note_line:
       uprintf("noteline\n");
       break;
    case ff_a_single_note_line_double_key:
       uprintf("single note line with double tap\n");
       break;
    case ff_a_tap_dance:
       uprintf("tap dance\n");
       break;
    case ff_a_single_chord:
       uprintf("isolated chord\n");
       break;
    case ff_a_single_cluster:
       uprintf("isolated cluster\n");
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
   
//    PPG_LOG("process_record_user\n");
//    PPG_LOG("   keycode: %d\n", keycode);
//    PPG_LOG("   pressed: %d\n", record->event.pressed);
//    PPG_LOG("   row: %d\n", record->event.key.row);
//    PPG_LOG("   col: %d\n", record->event.key.col);is

   bool key_processed = ppg_qmk_process_event(keycode, record);
   
   if(key_processed) { return false; }


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

void the_cluster_callback(void *user_data)
{
   uprintf("cluster callback: %d\n", (size_t)user_data);
}

#ifdef PPG_QMK_ERGODOX_EZ
// Note: The following method depens on the 
//       enum PPG_QMK_N_Inputs that is defined 
//       by PPG_QMK_INIT_DATA_STRUCTURES
//
void ff_signal_n_inputs(void)
{
   wait_ms(1000);
   for(uint8_t i = 0; i < PPG_QMK_N_Inputs; ++i) {
      ppg_qmk_led_flash();
      wait_ms(200);
   }
}
#endif // PPG_QMK_ERGODOX_EZ

void init_papageno(void)
{
   PPG_QMK_INIT
   
   // When the abort key is pressed, pattern recognition is aborted
   // the same as if timeout had occured.
   //
   ppg_global_set_abort_trigger(PPG_QMK_INPUT_FROM_KEYPOS_ALIAS(PPG_ABORT_KEY));
   
   // You could also use a keycode to define abort...
   //
   //ppg_global_set_abort_trigger(PPG_QMK_INPUT_FROM_KEYCODE_ALIAS(FF_KC_7));
   
//    ppg_qmk_set_timeout_ms(20000);
   ppg_qmk_set_timeout_ms(500);
   
   /* Single note line pattern: ErgoDox left inner large thumb key followed by
    * right inner large thumb key
    */
   PPG_QMK_KEYPOS_NOTE_LINE_ACTION_KEYCODE(
      ff_layer_base, // Layer
      F(ff_a_pattern_1), // Keycode action
      // Matrix keypos...
      KEY_52, // left inner large thumb key
      //KEY_52, //    ...keys could also occur several times
      KEY_5B  // right inner large thumb key
   );

      
   // Equivalent definition ...
   //
//    ppg_pattern(
//       ff_layer_base,
//       PPG_TOKENS(
//          ppg_note_create_standard(PPG_QMK_INPUT_FROM_KEYPOS_ALIAS(KEY_52)),
//          ppg_token_set_action(
//             ppg_note_create_standard(PPG_QMK_INPUT_FROM_KEYPOS_ALIAS(KEY_5B)),
//             PPG_QMK_ACTION_KEYCODE(  
//                F(ff_a_pattern_1)
//             )
//          )
//       )
//    );
   
   // We can also define single note lines based on keycodes
   //
   PPG_QMK_KEYCODE_NOTE_LINE_ACTION_KEYCODE(
      ff_layer_base, // Layer
      F(ff_a_pattern_1), // Keycode action
      // Matrix keypos...
      FF_KC_A, // left inner large thumb key
      FF_KC_B  // right inner large thumb key
   );
         
   /* Pattern specification: First a chord of the s, d and f keys (QWERTY) then a cluster of
    * j, k and l (QWERTY).
    */
   ppg_pattern(
      ff_layer_base, /* Layer id */
      PPG_TOKENS(
         PPG_QMK_KEYPOS_CHORD_TOKEN(
            CHORD_KEY_1, // s (QWERTY)
            CHORD_KEY_2, // d (QWERTY)
            CHORD_KEY_3  // f (QWERTY)
         ),
         
         // Note: If necesarry, keypos- and keycode-inputs can be intermixed
         //
         PPG_CHORD_CREATE(
            PPG_QMK_INPUT_FROM_KEYPOS_ALIAS(CHORD_KEY_1),
            PPG_QMK_INPUT_FROM_KEYCODE_ALIAS(FF_KC_A)
         ),
         ppg_token_set_action(
            PPG_QMK_KEYCODE_CLUSTER_TOKEN(
               FF_KC_A,
               FF_KC_B
            ),
            PPG_QMK_ACTION_KEYCODE(
               F(ff_a_chord_and_cluster)
            )
         )
      )
   );
   
   /* Triple tap on w (QWERTY)
    */
   ppg_tap_dance(
      ff_layer_base,
      PPG_QMK_INPUT_FROM_KEYPOS_ALIAS(SINGLE_NOTE_LINE_KEY_1), /* The tap key
         could also be a keycode using PPG_QMK_INPUT_FROM_KEYCODE_ALIAS */
      PPG_TAP_DEFINITIONS(
         PPG_TAP(
            3, 
            PPG_QMK_ACTION_KEYCODE(
               F(ff_a_tap_dance)
            )
         )
      )
   );
   
   /* A single cluster of j, k and l (QWERTY).
    * Note how a user callback is used as action.
    */
   ppg_token_set_action(
      PPG_QMK_KEYPOS_CLUSTER_ACTION_KEYCODE(
         ff_layer_base,
         0, // Dummy, overwitten by explicit action definition
         CLUSTER_KEY_1, // j (QWERTY)
         CLUSTER_KEY_2, // k (QWERTY)
         CLUSTER_KEY_3  // l (QWERTY)
      ), 
      PPG_ACTION_USER_CALLBACK(
         the_cluster_callback,
         (void*)(size_t)13 /*user data*/
      )
   );
      
   // Alternative definition ...
   //
//    ppg_cluster(
//       ff_layer_base,
//       PPG_ACTION_USER_CALLBACK(
//          the_cluster_callback,
//          (void*)(size_t)13 /*user data*/
//       ),
//       PPG_QMK_KEYS(
//          PPG_QMK_INPUT_FROM_KEYPOS_ALIAS(CLUSTER_KEY_1),
//          PPG_QMK_INPUT_FROM_KEYPOS_ALIAS(CLUSTER_KEY_2),
//          PPG_QMK_INPUT_FROM_KEYPOS_ALIAS(CLUSTER_KEY_3)
//       )
//    );
   
   /* A pattern to switch to the qwerty layer.
    */
   PPG_QMK_KEYPOS_NOTE_LINE_ACTION_KEYCODE(
      ff_layer_base, // Layer
      TG(ff_layer_qwerty), // Keycode action
      FF_BACK_LINE_1, // Matrix keypos
      FF_BACK_LINE_2, // ...
      FF_BACK_LINE_3, // ...
      FF_BACK_LINE_4, // ...
      FF_BACK_LINE_5, // ...
      FF_BACK_LINE_6
   );
   
   // Equivalent definition ...
   //
//    ppg_single_note_line(
//       ff_layer_base,
//       PPG_QMK_ACTION_KEYCODE(
//          TG(ff_layer_qwerty)
//       ),
//          PPG_QMK_KEYS(
//             PPG_QMK_INPUT_FROM_KEYPOS_ALIAS(FF_BACK_LINE_1),
//             PPG_QMK_INPUT_FROM_KEYPOS_ALIAS(FF_BACK_LINE_2),
//             PPG_QMK_INPUT_FROM_KEYPOS_ALIAS(FF_BACK_LINE_3),
//             PPG_QMK_INPUT_FROM_KEYPOS_ALIAS(FF_BACK_LINE_4),
//             PPG_QMK_INPUT_FROM_KEYPOS_ALIAS(FF_BACK_LINE_5),
//             PPG_QMK_INPUT_FROM_KEYPOS_ALIAS(FF_BACK_LINE_6)
//          )
//    );
   
   ppg_global_compile();
   
//    #ifdef PPG_QMK_ERGODOX_EZ
//    ff_signal_n_inputs();
//    #endif

//    PPG_LOG("Setup completed\n");
}

void matrix_init_user(void) { 
   init_papageno();
}

void matrix_scan_user(void) {
    
//    PPG_LOG("Matrix scan user\n");
   
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
    
    ppg_qmk_matrix_scan();
};
