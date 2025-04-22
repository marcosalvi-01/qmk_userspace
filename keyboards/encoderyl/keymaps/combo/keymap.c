// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "keymap_italian.h"
#include "sendstring_italian.h"
#include "print.h"
#include "features/custom_shift_keys.h"
#include "features/achordion.h"

enum layer_names {
    _BASE       = 0,
    _NAVIGATION = 1,
    _NUMBERS    = 2,
    _SYMBOLS    = 3,
    _BUTTON     = 4,
    _FUNCTION   = 5,
    _GAME       = 7,    // Needs to be 7 because that's what the Autohotkey script expects
};

enum custom_keycodes {
    CLIP = SAFE_RANGE,
    BACKTICK,
};

// Tap dance keycodes
enum td_keycodes {
    TD_MPLY_MNXT_MPRV,  // Tap dance for media play, next and previous
    TD_Y_CLIP,          // Tap dance for y and clip with nvidia shadowplay
    TD_EGRV_SFT,
};

// Used by Autohotkey to display the current layer info (only game and default)
layer_state_t layer_state_set_user(layer_state_t state) {
    // If the highest layer is either game or base coming from game, print the layer
    if(get_highest_layer(state) == _GAME || (get_highest_layer(layer_state) == _GAME && get_highest_layer(state) == _BASE))
        uprintf("KBHLayer%u%s\n", get_highest_layer(state), "");
    return state;
}

// Pause next previous behavior
void pause_next_previous(tap_dance_state_t *state, void *user_data){
    switch (state->count){
        case 1:
            tap_code16(KC_MPLY);
            break;
        case 2:
            tap_code16(KC_MNXT);
            break;
        case 3:
            tap_code16(KC_MPRV);
            break;
    }
}
// Y and clip macro used in shadowplay
void y_clip(tap_dance_state_t *state, void *user_data){
    switch (state->count){
        case 1:
            tap_code16(KC_Y);
            break;
        case 2: // alt + shift + ctrl + f11
            register_code(KC_LALT);
            register_code(KC_LSFT);
            register_code(KC_LCTL);
            tap_code(KC_F11);
            unregister_code(KC_LALT);
            unregister_code(KC_LSFT);
            unregister_code(KC_LCTL);
            break;
    }
}

// tap: è, hold: é, shift + tap: È
void egrv_sft_finished(tap_dance_state_t *state, void *user_data){
        if (state->interrupted || !state->pressed) { // single tap
            const uint8_t mods = get_mods();
            if ((mods | get_weak_mods()) & MOD_MASK_SHIFT) {  // Shift is active
                del_mods(MOD_MASK_SHIFT);   // Disable shift
                del_weak_mods(MOD_MASK_SHIFT);
                send_keyboard_report();
                register_code(KC_LALT);     // È
                tap_code(KC_P0);
                tap_code(KC_P2);
                tap_code(KC_P0);
                tap_code(KC_P0);
                set_mods(mods);  // Restore shift
                unregister_code(KC_LALT);
                return;
            } else
                tap_code(IT_EGRV);
        } else // hold
            tap_code16(S(IT_EGRV)); // é
}

// Tap dance definitions
tap_dance_action_t tap_dance_actions[] = {
    [TD_MPLY_MNXT_MPRV] = ACTION_TAP_DANCE_FN(pause_next_previous),
    [TD_Y_CLIP] = ACTION_TAP_DANCE_FN(y_clip),
    [TD_EGRV_SFT] = ACTION_TAP_DANCE_FN(egrv_sft_finished)
};

// Custom tapping term for specific keys
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LT(_SYMBOLS, KC_SPC):
            return 300;
        default:
            return TAPPING_TERM;
    }
}

// Combos
const uint16_t PROGMEM game_layer_combo[] = {KC_C, KC_X, COMBO_END};
const uint16_t PROGMEM caps_word_combo[] = {IT_COMM, IT_DOT, COMBO_END};
const uint16_t PROGMEM escape_combo_game[] = {KC_R, KC_S, KC_T, COMBO_END};
const uint16_t PROGMEM escape_combo_base[] = {LALT_T(KC_R), LSFT_T(KC_S), LCTL_T(KC_T), COMBO_END};
const uint16_t PROGMEM caps_lock_combo[] = {KC_D, KC_L, COMBO_END};
// F + P = B
const uint16_t PROGMEM f_p_combo[] = {KC_F, KC_P, COMBO_END};
// S + T = G
const uint16_t PROGMEM s_t_combo[] = {LSFT_T(KC_S), LCTL_T(KC_T), COMBO_END};
// C + D = V
const uint16_t PROGMEM c_d_combo[] = {KC_C, KC_D, COMBO_END};
// A + R = Q
const uint16_t PROGMEM a_r_combo[] = {LGUI_T(KC_A), LALT_T(KC_R), COMBO_END};

// H + U = J
const uint16_t PROGMEM h_u_combo[] = {KC_H, KC_U, COMBO_END};
// N + E = M
const uint16_t PROGMEM n_e_combo[] = {LCTL_T(KC_N), LSFT_T(KC_E), COMBO_END};
// L + . = K
const uint16_t PROGMEM l_dot_combo[] = {KC_L, IT_DOT, COMBO_END};
// I + O = ?
const uint16_t PROGMEM i_o_combo[] = {LALT_T(KC_I), LGUI_T(KC_O), COMBO_END};

// * + @ = `
const uint16_t PROGMEM star_at_combo[] = {IT_ASTR, IT_AT, COMBO_END};
// = + " = !
const uint16_t PROGMEM equal_quote_combo[] = {IT_EQL, IT_DQUO, COMBO_END};
// / + < = %
const uint16_t PROGMEM slash_less_combo[] = {IT_SLSH, IT_LABK, COMBO_END};
// { + [ = #
const uint16_t PROGMEM lcb_lbr_combo[] = {IT_LCBR, IT_LBRC, COMBO_END};

combo_t key_combos[] = {
    COMBO(game_layer_combo, TG(_GAME)),
    COMBO(caps_word_combo, CW_TOGG),
    COMBO(escape_combo_game, KC_ESC),
    COMBO(caps_lock_combo, KC_CAPS),
    COMBO(escape_combo_base, KC_ESC),

    COMBO(f_p_combo, KC_B),
    COMBO(s_t_combo, KC_G),
    COMBO(c_d_combo, KC_V),
    COMBO(a_r_combo, KC_Q),

    COMBO(h_u_combo, KC_J),
    COMBO(n_e_combo, KC_M),
    COMBO(l_dot_combo, KC_K),
    COMBO(i_o_combo, IT_QUES),

    COMBO(star_at_combo, BACKTICK),
    COMBO(equal_quote_combo, IT_EXLM),
    COMBO(slash_less_combo, IT_PERC),
    COMBO(lcb_lbr_combo, IT_HASH),
};

bool get_combo_must_tap(uint16_t index, combo_t *combo) {
    // If you want *all* combos, that have Mod-Tap/Layer-Tap/Momentary keys in its chord, to be tap-only, this is for you:
    uint16_t key;
    uint8_t idx = 0;
    while ((key = pgm_read_word(&combo->keys[idx])) != COMBO_END) {
        switch (key) {
            case QK_MOD_TAP...QK_MOD_TAP_MAX:
            case QK_LAYER_TAP...QK_LAYER_TAP_MAX:
            case QK_MOMENTARY...QK_MOMENTARY_MAX:
                return true;
        }
        idx += 1;
    }
    return false;
}

// Caps Word behavior
bool caps_word_press_user(uint16_t keycode) {
    switch (keycode) {
        // Keycodes that continue Caps Word, with shift applied.
        case KC_A ... KC_Z:
            add_weak_mods(MOD_BIT(KC_LSFT));  // Apply shift to next key.
            return true;

        // Keycodes that continue Caps Word, without shifting.
        case KC_1 ... KC_0:
        case KC_BSPC:
        case KC_DEL:
        case IT_UNDS:
        case IT_DOT:
        case IT_COMM:
        case IT_QUOT:
        case IT_SLSH:
        case IT_BSLS:
        case IT_MINS:
            return true;

        default:
            return false;  // Deactivate Caps Word.
    }
}

// Custom shift keys
const custom_shift_key_t custom_shift_keys[] = {
    {IT_SLSH, IT_BSLS},     // Shift / is '\'
    {IT_LCBR, IT_RCBR},     // Shift { is }
    {IT_LBRC, IT_RBRC},     // Shift [ is ]
    {IT_LPRN, IT_RPRN},     // Shift ( is )
    {IT_LABK, IT_RABK},     // Shift < is >
    {IT_DLR, IT_EURO},      // Shift $ is €
    {IT_QUES, IT_QUOT},     // Shift ? is '
};
uint8_t NUM_CUSTOM_SHIFT_KEYS = sizeof(custom_shift_keys) / sizeof(custom_shift_key_t);

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Achordion
    if (!process_achordion(keycode, record)) { return false; }
    // Handle custom shift keys
    if (!process_custom_shift_keys(keycode, record)) { return false; }

    // Handle custom keycodes
    switch (keycode) {
        case BACKTICK:
            if (record->event.pressed) {
                // Alt + 96
                register_code(KC_LALT);
                tap_code16(KC_P9);
                tap_code16(KC_P6);
            } else {
                unregister_code(KC_LALT);
            }
            return false;
        default:
            return true;
    }
}

//Encoder alt tab
bool is_alt_tab_active = false;
uint16_t alt_tab_timer = 0;

// Encoder timer for alt tab
void matrix_scan_user(void) {
    // Achordion
    achordion_task();

    // Encoder alt tab
    if (is_alt_tab_active)
        if (timer_elapsed(alt_tab_timer) > ALT_TAB_TIMER) {
            unregister_code(KC_LALT);
            is_alt_tab_active = false;
        }
}

bool achordion_chord(uint16_t tap_hold_keycode, keyrecord_t* tap_hold_record, uint16_t other_keycode, keyrecord_t* other_record) {
    // If the highest layer is game, never block.
    if (get_highest_layer(layer_state) == _GAME)
        return true;
    // Skip the achordion if the key is not a normal key (combo)
    if (!IS_KEYEVENT(other_record->event))
        return true;

    // Exceptionally consider the following chords as holds, even though they are on the same hand.
    switch (tap_hold_keycode) {
        case LT(_BUTTON, KC_Z):             // cut copy paste redo
            if ((other_keycode == KC_X)     ||
                (other_keycode == KC_C)     ||
                (other_keycode == KC_D)     ||
                (other_keycode == KC_V))
                return true;
            break;
    }

    // If the other key is a thumb key, don't block.
    if (other_record->event.key.row % (MATRIX_ROWS / 2) >= 3)
        return true;

    // Otherwise, follow the opposite hands rule.
    return achordion_opposite_hands(tap_hold_record, other_record);
}

// Achordion timeout
uint16_t achordion_timeout(uint16_t tap_hold_keycode) {
    switch (tap_hold_keycode) {
        // Skip the achordion timeout
        // Use this for the mod tap layer keys that modify the encoder behavior.
        case LT(_SYMBOLS, KC_SPC):
        case LT(_NUMBERS, KC_DEL):
        // Ignore the timeout for the game layer mod taps
        case LALT_T(KC_2):
        case LSFT_T(KC_1):
            return 0;
    }
  return ACHORDION_TIMEOUT;
}

// Encoder behavior based on the current layer
bool encoder_update_user(uint8_t index, bool clockwise) {
    switch(get_highest_layer(layer_state)) {
        // Alt tab
        case _BUTTON:
        case _GAME:
        case _BASE:
            if (clockwise) {
                if (!is_alt_tab_active) {
                    is_alt_tab_active = true;
                    register_code(KC_LALT);
                }
                alt_tab_timer = timer_read();
                tap_code16(KC_TAB);
            } else {
                if (!is_alt_tab_active) {
                    is_alt_tab_active = true;
                    register_code(KC_LALT);
                }
                alt_tab_timer = timer_read();
                tap_code16(S(KC_TAB));
            }
            break;
        // Navigation in applications (switch between tabs)
        case _SYMBOLS:
            clockwise ? tap_code16(C(KC_PGDN)) : tap_code16(C(KC_PGUP));
            break;
    }
    return false;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Default Layer
    * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
    * │ Q │ W │ F │ P │ B │       │ J │ H │ U │ Y │ ? │
    * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
    * │ A │ R │ S │ T │ G │       │ M │ N │ E │ I │ O │
    * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
    * │ Z │ X │ C │ D │ V │       │ K │ L │ . │ , │ - │
    * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
    *           ┌───┐                   ┌───┐
    *           │Del├───┐           ┌───┤Esc│
    *           └───┤Spc├───┐   ┌───┤Bsp├───┘
    *               └───┤Tab│   │Ent├───┘
    *                   └───┘   └───┘
    */
    [_BASE] = LAYOUT_split_3x5_3(
        KC_Q,              KC_W,         KC_F,                 KC_P,                 KC_B,                   KC_J,                  KC_H,                     KC_U,         KC_Y,         IT_QUES,
        LGUI_T(KC_A),      LALT_T(KC_R), LSFT_T(KC_S),         LCTL_T(KC_T),         KC_G,                   KC_M,                  LCTL_T(KC_N),             LSFT_T(KC_E), LALT_T(KC_I), LGUI_T(KC_O),
        LT(_BUTTON, KC_Z), KC_X,         KC_C,                 KC_D,                 KC_V,                   KC_K,                  KC_L,                     IT_DOT,      IT_COMM,       LT(_BUTTON, IT_MINS),
                                         LT(_NUMBERS, KC_DEL), LT(_SYMBOLS, KC_SPC), KC_TAB,                 LT(_FUNCTION, KC_ENT), LT(_NAVIGATION, KC_BSPC), KC_ESC
    ),
    /* Navigation Layer
    * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
    * │   │HOM│ ^ │END│   │       │   │   │   │   │   │
    * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
    * │PUP│ < │ v │ > │   │       │   │   │   │   │   │
    * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
    * │PDN│   │   │   │   │       │   │   │   │   │   │
    * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
    *           ┌───┐                   ┌───┐
    *           │   ├───┐           ┌───┤   │
    *           └───┤   ├───┐   ┌───┤   ├───┘
    *               └───┤   │   │   ├───┘
    *                   └───┘   └───┘
    */
    [_NAVIGATION] = LAYOUT_split_3x5_3(
        _______, KC_HOME, KC_UP,   KC_END,  _______,        _______, _______, _______, _______, _______,
        KC_PGUP, KC_LEFT, KC_DOWN, KC_RGHT, _______,        _______, KC_LCTL, KC_LSFT, KC_LALT, KC_LGUI,
        KC_PGDN, _______, IT_LCBR, IT_RCBR, _______,        _______, _______, _______, _______, _______,
                          _______, _______, _______,        _______, _______, _______
    ),
    /* Numbers Layer
    * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
    * │   │   │   │   │   │       │   │ 7 │ 8 │ 9 │   │
    * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
    * │   │   │   │   │   │       │   │ 4 │ 5 │ 6 │ ^ │
    * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
    * │   │   │   │   │   │       │   │ 1 │ 2 │ 3 │   │
    * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
    *           ┌───┐                   ┌───┐
    *           │   ├───┐           ┌───┤   │
    *           └───┤   ├───┐   ┌───┤ 0 ├───┘
    *               └───┤   │   │   ├───┘
    *                   └───┘   └───┘
    */
    [_NUMBERS] = LAYOUT_split_3x5_3(
        _______, _______, _______, _______, _______,        _______, KC_7, KC_8, KC_9, _______,
        KC_LGUI, KC_LALT, KC_LSFT, KC_LCTL, _______,        _______, KC_4, KC_5, KC_6, IT_CIRC,
        _______, _______, _______, _______, _______,        _______, KC_1, KC_2, KC_3, _______,
                          _______, _______, _______,        _______, KC_0, _______
    ),
    /* Symbols Layer
    * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
    * │   │   │   │   │   │       │ ` │ * │ @ │ $ │ # │
    * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
    * │   │   │   │   │   │       │ < │ = │ " │ { │ [ │
    * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
    * │   │   │   │   │   │       │ % │ / │ ! │ & │ | │
    * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
    *           ┌───┐                   ┌───┐
    *           │   ├───┐           ┌───┤   │
    *           └───┤   ├───┐   ┌───┤ ( ├───┘
    *               └───┤   │   │ + ├───┘
    *                   └───┘   └───┘
    */
    [_SYMBOLS] = LAYOUT_split_3x5_3(
        _______, _______, _______, _______, _______,        BACKTICK, IT_ASTR, IT_AT,   IT_DLR,  IT_HASH,  // TODO
        KC_LGUI, KC_LALT, KC_LSFT, KC_LCTL, _______,        IT_EXLM,  IT_EQL,  IT_DQUO, IT_LCBR, IT_LBRC,
        _______, _______, _______, _______, _______,        IT_PERC,  IT_SLSH, IT_LABK, IT_AMPR, IT_PIPE,
                          _______, _______, _______,        IT_PLUS,  IT_LPRN, _______
    ),
    /* Button Layer
    * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
    * │   │   │   │   │   │       │   │   │   │   │   │
    * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
    * │   │   │   │   │   │       │   │PSC│   │   │   │
    * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
    * │UND│CUT│CPY│PST│RDO│       │RDO│PST│CPY│CUT│UND│
    * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
    *           ┌───┐                   ┌───┐
    *           │   ├───┐           ┌───┤   │
    *           └───┤   ├───┐   ┌───┤   ├───┘
    *               └───┤   │   │   ├───┘
    *                   └───┘   └───┘
    */
    [_BUTTON] = LAYOUT_split_3x5_3(
        _______, _______, _______, _______, _______,        LALT(KC_F4), _______,   IT_UGRV, _______, _______,
        IT_AGRV, _______, _______, _______, _______,        _______,     LSG(KC_S), TD(TD_EGRV_SFT), IT_IGRV, IT_OGRV,
        C(KC_Z), C(KC_X), C(KC_C), C(KC_V), C(KC_Y),        C(KC_Y),     C(KC_V),   C(KC_C), C(KC_X), C(KC_Z),
                          _______, _______, _______,        _______,     _______,   _______
    ),
    /* Game Layer
    * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
    * │ Q │ W │ F │ P │ B │       │ J │ H │ U │ Y │_BS│
    * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
    * │ A │ R │ S │ T │ G │       │ M │ N │ E │ I │ O │
    * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
    * │ Z │ X │ C │ D │ V │       │ K │ L │ . │ , │???│
    * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
    *           ┌───┐                   ┌───┐
    *           │ 1 ├───┐           ┌───┤Esc│
    *           └───┤Spc├───┐   ┌───┤Bsp├───┘
    *               └───┤ 2 │   │Ent├───┘
    *                   └───┘   └───┘
    */
    [_GAME] = LAYOUT_split_3x5_3(
        KC_Q, KC_W, KC_F,         KC_P,   KC_B,             KC_J,   KC_H,    KC_U,   TD(TD_Y_CLIP), TO(_BASE),
        KC_A, KC_R, KC_S,         KC_T,   KC_G,             KC_M,   KC_N,    KC_E,   KC_I,          KC_O,
        KC_Z, KC_X, KC_C,         KC_D,   KC_V,             KC_K,   KC_L,    IT_DOT, IT_COMM,       TD(TD_MPLY_MNXT_MPRV),
                    LSFT_T(KC_1), KC_SPC, LALT_T(KC_2),     KC_ENT, KC_BSPC, _______
    ),
    /* Function Layer
    * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
    * │   │   │   │   │   │       │   │   │   │   │   │
    * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
    * │   │   │   │   │   │       │   │   │   │   │   │
    * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
    * │   │   │   │   │   │       │   │   │   │   │   │
    * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
    *           ┌───┐                   ┌───┐
    *           │   ├───┐           ┌───┤   │
    *           └───┤   ├───┐   ┌───┤   ├───┘
    *               └───┤   │   │   ├───┘
    *                   └───┘   └───┘
    */
    [_FUNCTION] = LAYOUT_split_3x5_3(
        _______, KC_F7, KC_F8,   KC_F9,   KC_F12,          _______, _______, _______, _______, _______,
        KC_VOLU, KC_F4, KC_F5,   KC_F6,   KC_F11,          _______, _______, _______, _______, _______,
        KC_VOLD, KC_F1, KC_F2,   KC_F3,   KC_F10,          _______, _______, _______, _______, _______,
                        KC_MPRV, KC_MPLY, KC_MNXT,         _______, _______, _______
    ),
};








// /* ??? Layer
//       * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
//       * │   │   │   │   │   │       │   │   │   │   │   │
//       * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
//       * │   │   │   │   │   │       │   │   │   │   │   │
//       * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
//       * │   │   │   │   │   │       │   │   │   │   │   │
//       * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
//       *           ┌───┐                   ┌───┐
//       *           │   ├───┐           ┌───┤   │
//       *           └───┤   ├───┐   ┌───┤   ├───┘
//       *               └───┤   │   │   ├───┘
//       *                   └───┘   └───┘
//       */
//     [_???] = LAYOUT_split_3x5_3(
//         _______, _______, _______, _______, _______,         _______, _______, _______, _______, _______,
//         _______, _______, _______, _______, _______,         _______, _______, _______, _______, _______,
//         _______, _______, _______, _______, _______,         _______, _______, _______, _______, _______,
//                           _______, _______, _______,         _______, _______, _______
//     ),
