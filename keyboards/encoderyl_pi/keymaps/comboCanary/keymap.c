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
    TILDE,
    ALT_TAB_NAV,
};

bool alt_tab_nav_active = false;

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

    switch (get_highest_layer(state)) {
        case _BASE:
            if (alt_tab_nav_active) {
                unregister_code(KC_LALT);
                alt_tab_nav_active = false;
            }
            break;
        default:
            break;
    }
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
            } else {
                tap_code(IT_EGRV);
            }
        } else {// hold
            tap_code16(S(IT_EGRV)); // é
        }
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
const uint16_t PROGMEM game_layer_combo[] = {KC_V, KC_J, COMBO_END};
const uint16_t PROGMEM caps_word_combo[] = {IT_COMM, IT_DOT, COMBO_END};
const uint16_t PROGMEM escape_combo_game[] = {KC_R, KC_S, KC_T, COMBO_END};
const uint16_t PROGMEM escape_combo_base[] = {LALT_T(KC_R), LSFT_T(KC_S), LCTL_T(KC_T), COMBO_END};
const uint16_t PROGMEM caps_lock_combo[] = {KC_D, KC_H, COMBO_END};
const uint16_t PROGMEM vim_combo[] = {LCTL_T(KC_N), LSFT_T(KC_E), LALT_T(KC_I), COMBO_END};

// Y + P = B
const uint16_t PROGMEM y_p_combo[] = {KC_Y, KC_P, COMBO_END};
// S + T = G
const uint16_t PROGMEM s_t_combo[] = {LSFT_T(KC_S), LCTL_T(KC_T), COMBO_END};
// V + D = K
const uint16_t PROGMEM v_d_combo[] = {KC_V, KC_D, COMBO_END};
// C + R = Q
const uint16_t PROGMEM c_r_combo[] = {LGUI_T(KC_C), LALT_T(KC_R), COMBO_END};

// F + O = Z
const uint16_t PROGMEM f_o_combo[] = {KC_F, KC_O, COMBO_END};
// N + E = M
const uint16_t PROGMEM n_e_combo[] = {LCTL_T(KC_N), LSFT_T(KC_E), COMBO_END};
// H + . = X
const uint16_t PROGMEM h_dot_combo[] = {KC_H, IT_DOT, COMBO_END};
// I + A = ?
const uint16_t PROGMEM i_a_combo[] = {LALT_T(KC_I), LGUI_T(KC_A), COMBO_END};

// < + { = `
const uint16_t PROGMEM labk_lcbr_combo[] = {IT_LABK, IT_LCBR, COMBO_END};
// = + [ = @
const uint16_t PROGMEM eql_lbrc_combo[] = {IT_EQL, IT_LBRC, COMBO_END};
// % + ( = "
const uint16_t PROGMEM perc_lprn_combo[] = {IT_PERC, IT_LPRN, COMBO_END};
// + + | = $
const uint16_t PROGMEM plus_pipe_combo[] = {IT_PLUS, IT_PIPE, COMBO_END};

// FN2 + FN3 = FN10
const uint16_t PROGMEM fn2_fn3_combo[] = {KC_F2, KC_F3, COMBO_END};
// FN5 + FN6 = FN11
const uint16_t PROGMEM fn5_fn6_combo[] = {KC_F5, KC_F6, COMBO_END};
// FN8 + FN9 = FN12
const uint16_t PROGMEM fn8_fn9_combo[] = {KC_F8, KC_F9, COMBO_END};


combo_t key_combos[] = {
    COMBO(game_layer_combo, TG(_GAME)),
    COMBO(caps_word_combo, CW_TOGG),
    COMBO(escape_combo_game, KC_ESC),
    COMBO(caps_lock_combo, KC_CAPS),
    COMBO(escape_combo_base, KC_ESC),
    COMBO(vim_combo, C(KC_F9)),

    COMBO(y_p_combo, KC_B),
    COMBO(s_t_combo, KC_G),
    COMBO(v_d_combo, KC_K),
    COMBO(c_r_combo, KC_Q),

    COMBO(f_o_combo, KC_Z),
    COMBO(n_e_combo, KC_M),
    COMBO(h_dot_combo, KC_X),
    COMBO(i_a_combo, IT_QUES),

    COMBO(labk_lcbr_combo, BACKTICK),
    COMBO(eql_lbrc_combo, IT_AT),
    COMBO(perc_lprn_combo, IT_DQUO),
    COMBO(plus_pipe_combo, IT_DLR),

    COMBO(fn2_fn3_combo, KC_F10),
    COMBO(fn5_fn6_combo, KC_F11),
    COMBO(fn8_fn9_combo, KC_F12),
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
        case KC_LEFT_SHIFT:
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
    {LT(_NAVIGATION, KC_BSPC), KC_DEL},      // Shift Backspace is Delete
    {KC_BSPC, KC_DEL},      // Shift Backspace is Delete
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
                const uint8_t mods = get_mods();
                if ((mods | get_weak_mods()) & MOD_MASK_SHIFT) {  // Shift is active
                    return process_record_user(TILDE, record);
                }
                // Alt + 96
                register_code(KC_LALT);
                tap_code16(KC_P9);
                tap_code16(KC_P6);
            } else {
                unregister_code(KC_LALT);
            }
            return false;
        case TILDE:
            if (record->event.pressed) {
                const uint8_t mods = get_mods();
                if ((mods | get_weak_mods()) & MOD_MASK_SHIFT) {  // Shift is active
                    del_mods(MOD_MASK_SHIFT);   // Disable shift
                    del_weak_mods(MOD_MASK_SHIFT);
                    send_keyboard_report();
                    register_code(KC_LALT);     // Tilde
                    tap_code(KC_P1);
                    tap_code(KC_P2);
                    tap_code(KC_P6);
                    set_mods(mods);  // Restore shift
                } else {
                    register_code(KC_LALT);     // Tilde
                    tap_code(KC_P1);
                    tap_code(KC_P2);
                    tap_code(KC_P6);
                }
            } else {
                unregister_code(KC_LALT);
            }
            return false;
        case ALT_TAB_NAV:
            if (record->event.pressed) {
                // Alt + Tab
                register_code(KC_LALT);
                alt_tab_nav_active = true;
                tap_code16(KC_TAB);
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
        case LT(_BUTTON, KC_W):
            return true;
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
        case LT(_BUTTON, KC_TAB):
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
        case _SYMBOLS:
            clockwise ? tap_code16(C(KC_PGDN)) : tap_code16(C(KC_PGUP)); // Chrome switch tabs
            break;
        case _NUMBERS:
            clockwise ? tap_code16(KC_WH_D) : tap_code16(KC_WH_U);  // Scroll wheel
            break;

    }
    return false;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Default Layer
    * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
    * │ Q │ J │ V │ D │ K │       │ X │ H │ . │ , │ ? │
    * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
    * │ C │ R │ S │ T │ G │       │ M │ N │ E │ I │ A │
    * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
    * │ W │ L │ Y │ P │ B │       │ Z │ F │ O │ U │ - │
    * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
    *           ┌───┐                   ┌───┐
    *           │Del├───┐           ┌───┤Esc│
    *           └───┤Spc├───┐   ┌───┤Bsp├───┘
    *               └───┤Tab│   │Ent├───┘
    *                   └───┘   └───┘
    */
    [_BASE] = LAYOUT_split_3x5_3(
        KC_Q,         KC_L,         KC_Y,                 KC_P, _______,  _______,                                                   KC_F,                     KC_O,         KC_U,         IT_QUES,
        LGUI_T(KC_C), LALT_T(KC_R), LSFT_T(KC_S),         LCTL_T(KC_T), _______, _______,                                            LCTL_T(KC_N),             LSFT_T(KC_E), LALT_T(KC_I), LGUI_T(KC_A),
        LT(_BUTTON, KC_W),         KC_J,         KC_V,                 KC_D, _______, _______,                                                    KC_H,                     IT_DOT,       IT_COMM,      IT_MINS,
                                    LT(_NUMBERS, KC_ESC), LT(_SYMBOLS, KC_SPC), KC_TAB,       LT(_FUNCTION, KC_ENT), LT(_NAVIGATION, KC_BSPC), KC_ESC
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
        _______, KC_HOME, KC_UP,   KC_END,_______, _______,           _______, _______, _______, _______,
        KC_PGUP, KC_LEFT, KC_DOWN, KC_RGHT,_______, _______,          KC_LCTL, KC_LSFT, KC_LALT, KC_LGUI,
        KC_PGDN, _______, IT_LCBR, IT_RCBR,_______, _______,          _______, _______, _______, _______,
                          _______, ALT_TAB_NAV, _______,        _______, _______, _______
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
        _______, _______, _______, _______,_______, _______,         KC_7, KC_8, KC_9, _______,
        KC_LGUI, KC_LALT, KC_LSFT, KC_LCTL, _______,_______,        KC_4, KC_5, KC_6, IT_CIRC,
        _______, _______, _______, _______, _______,_______,        KC_1, KC_2, KC_3, _______,
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
        _______, _______, _______, _______, _______,_______,                    IT_LABK, IT_LCBR, IT_AMPR, IT_DLR,  // TODO
        KC_LGUI, KC_LALT, KC_LSFT, KC_LCTL, _______,_______,                    IT_PERC, IT_LPRN, IT_PLUS, IT_PIPE,
        _______, _______, _______, _______, _______,_______,                    IT_EQL,  IT_LBRC, IT_EXLM, IT_ASTR,
                          _______, _______, _______,   IT_HASH, IT_SLSH, _______
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
        _______, _______,  _______, LSG(KC_S), _______,_______,                      _______, IT_OGRV,         IT_UGRV, _______,
        _______, C(KC_X),  C(KC_V), C(KC_Z), _______,_______,                       _______, TD(TD_EGRV_SFT), IT_IGRV, IT_AGRV,
        _______, A(KC_F4), _______, C(KC_Y), _______,_______,                        _______, _______,         _______, _______,
                           _______, C(KC_C),   _______,     _______, _______, _______
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
        KC_Q, KC_W, KC_F,         KC_P, KC_B,                   KC_J, KC_H,    KC_U,   TD(TD_Y_CLIP), TO(_BASE),
        KC_A, KC_R, KC_S,         KC_T, KC_G,                   KC_M, KC_N,    KC_E,   KC_I,          KC_O,
        KC_Z, KC_X, KC_C,         KC_D, KC_V,                   KC_K, KC_L,    IT_DOT, IT_COMM,       TD(TD_MPLY_MNXT_MPRV),
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
        _______, KC_F7, KC_F8,   KC_F9, _______,_______,             _______, _______, _______, _______,
        KC_VOLU, KC_F4, KC_F5,   KC_F6, _______,_______,             _______, _______, _______, _______,
        KC_VOLD, KC_F1, KC_F2,   KC_F3, _______,_______,             _______, _______, _______, _______,
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
//     [_???] = LAYOUT_split_3x5_3_split_3x5_3(
//         _______, _______, _______, _______, _______,         _______, _______, _______, _______, _______,
//         _______, _______, _______, _______, _______,         _______, _______, _______, _______, _______,
//         _______, _______, _______, _______, _______,         _______, _______, _______, _______, _______,
//                           _______, _______, _______,         _______, _______, _______
//     ),
