// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "keymap_italian.h"
#include "sendstring_italian.h"
#include "print.h"
#include "features/custom_shift_keys.h"

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
    SMTD_KEYCODES_BEGIN = SAFE_RANGE,

    CKC_C,
    CKC_R,
    CKC_S,
    CKC_T,

    CKC_N,
    CKC_E,
    CKC_I,
    CKC_A,

    CKC_W,

    CKC_DEL,
    CKC_SPC,

    CKC_ENT,
    CKC_BSP,

    // Game layer
    CKC_1,
    CKC_2,

    SMTD_KEYCODES_END,

    CLIP,
    BACKTICK,
    TILDE,
    ALT_TAB_NAV,
};

#include "sm_td.h"

void on_smtd_action(uint16_t keycode, smtd_action action, uint8_t tap_count) {
    switch (keycode) {
        SMTD_MT(CKC_C, KC_C, KC_LEFT_GUI)
        SMTD_MT(CKC_R, KC_R, KC_LEFT_ALT)
        SMTD_MT(CKC_S, KC_S, KC_LSFT)
        SMTD_MT(CKC_T, KC_T, KC_LEFT_CTRL)

        SMTD_MT(CKC_A, KC_A, KC_LEFT_GUI)
        SMTD_MT(CKC_I, KC_I, KC_LEFT_ALT)
        SMTD_MT(CKC_E, KC_E, KC_LSFT)
        SMTD_MT(CKC_N, KC_N, KC_LEFT_CTRL)

        SMTD_LT(CKC_W, KC_W, _BUTTON)

        SMTD_LT(CKC_DEL, KC_DEL, _NUMBERS)
        SMTD_LT(CKC_SPC, KC_SPC, _SYMBOLS)

        SMTD_LT(CKC_ENT, KC_ENTER, _FUNCTION)
        SMTD_LT(CKC_BSP, KC_BSPC, _NAVIGATION)

        // Game layer
        SMTD_MT(CKC_1, KC_1, KC_LSFT)
        SMTD_MT(CKC_2, KC_2, KC_LEFT_ALT)
    }
}

bool alt_tab_nav_active = false;

// Tap dance keycodes
enum td_keycodes {
    TD_MPLY_MNXT_MPRV,  // Tap dance for media play, next and previous
    TD_Y_CLIP,          // Tap dance for y and clip with nvidia shadowplay
    TD_EGRV_SFT,
    TD_EMAIL,
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

// Email macro
void email_on_press(tap_dance_state_t *state, void *user_data){
    // Empty function to follow the function signature, the important part is the release function
}
void email_on_release(tap_dance_state_t *state, void *user_data){
    switch (((state->count - 1) % 3) + 1) {
        case 1:
            if (state->count > 3) {
                // Delete the previous email
                for (int i = 0; i < 22; i++) {
                    register_code(KC_BSPC);
                }
            }
            SEND_STRING("marco.salvi@ingv.it");
            break;
        case 2:
            // Delete the previous email
            for (int i = 0; i < 19; i++) {
                register_code(KC_BSPC);
            }
            SEND_STRING("mykratos00@gmail.com");
            break;
        case 3:
            // Delete the previous email
            for (int i = 0; i < 20; i++) {
                register_code(KC_BSPC);
            }
            SEND_STRING("1marco.salvi@gmail.com");
            break;
    }
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
    [TD_EGRV_SFT] = ACTION_TAP_DANCE_FN(egrv_sft_finished),
    [TD_EMAIL] = ACTION_TAP_DANCE_FN_ADVANCED_WITH_RELEASE(email_on_press, email_on_release, email_on_press, email_on_press),
};

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TD(TD_EMAIL):
            return 2000;
        default:
            return TAPPING_TERM;
    }
}

// Combos
const uint16_t PROGMEM game_layer_combo[] = {KC_V, KC_D, COMBO_END};
const uint16_t PROGMEM base_layer_combo[] = {KC_X, KC_C, COMBO_END};
const uint16_t PROGMEM caps_word_combo[] = {KC_H, IT_DOT, COMBO_END};
const uint16_t PROGMEM escape_combo_game[] = {KC_A, KC_S, KC_D, COMBO_END};
const uint16_t PROGMEM escape_combo_base[] = {CKC_R, CKC_S, CKC_T, COMBO_END};
const uint16_t PROGMEM caps_lock_combo[] = {KC_D, KC_H, COMBO_END};
const uint16_t PROGMEM vim_combo[] = {CKC_N, CKC_E, CKC_I, COMBO_END};

combo_t key_combos[] = {
    COMBO(game_layer_combo, TG(_GAME)),
    COMBO(base_layer_combo, TG(_GAME)),
    COMBO(caps_word_combo, CW_TOGG),
    COMBO(escape_combo_game, KC_ESC),
    COMBO(caps_lock_combo, KC_CAPS),
    COMBO(escape_combo_base, KC_ESC),
    COMBO(vim_combo, C(KC_F9)),
};

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
    // Home row mods fix
    if (!process_smtd(keycode, record)) {
        return false;
    }
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
    // Encoder alt tab
    if (is_alt_tab_active)
        if (timer_elapsed(alt_tab_timer) > ALT_TAB_TIMER) {
            unregister_code(KC_LALT);
            is_alt_tab_active = false;
        }
}

// Encoder behavior based on the current layer
bool encoder_update_user(uint8_t index, bool clockwise) {
    switch(get_highest_layer(layer_state)) {
        // Alt tab
        case _BUTTON:
        case _GAME:
            // disable the game layer
            layer_off(_GAME);
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
        case _NUMBERS:
            clockwise ? tap_code16(KC_WH_D) : tap_code16(KC_WH_U);  // Scroll wheel
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
        KC_Q,              KC_L,         KC_Y,                 KC_P,                 KC_B,                   KC_Z,                  KC_F,                     KC_O,         KC_U,         IT_QUES,
        CKC_C,      CKC_R, CKC_S,        CKC_T,         KC_G,                   KC_M,                  CKC_N,             CKC_E, CKC_I, CKC_A,
        CKC_W, KC_J,         KC_V,                 KC_D,                 KC_K,                   KC_X,                  KC_H,                     IT_DOT,      IT_COMM,       IT_MINS,
                                         CKC_DEL, CKC_SPC, KC_TAB,                 CKC_ENT, CKC_BSP, KC_ESC
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
        _______, KC_HOME, KC_UP,   KC_END,      _______,        _______, _______, _______, _______, _______,
        KC_PGUP, KC_LEFT, KC_DOWN, KC_RGHT,     _______,        _______, KC_LCTL, KC_LSFT, KC_LALT, KC_LGUI,
        KC_PGDN, _______, _______, _______,     _______,        _______, _______, _______, _______, _______,
                          _______, ALT_TAB_NAV, _______,        _______, _______, _______
    ),
    /* Numbers Layer
    * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
    * │   │   │   │   │   │       │   │ 7 │ 8 │ 9 │   │
    * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
    * │   │   │   │   │   │       │   │ 4 │ 5 │ 6 │ ^ │
    * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
    * │   │   │   │   │   │       │   │ 1 │ 2 │ 3 │ / │
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
        _______, _______, _______, _______, _______,        _______, KC_1, KC_2, KC_3, IT_SLSH,
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
        _______, _______, _______, _______, _______,        BACKTICK, IT_LABK, IT_LCBR, IT_AMPR, IT_PIPE,
        KC_LGUI, KC_LALT, KC_LSFT, KC_LCTL, _______,        IT_DLR,   IT_PERC, IT_LPRN, IT_PLUS, IT_DQUO,
        _______, _______, _______, _______, _______,        IT_AT,    IT_EQL,  IT_LBRC, IT_EXLM, IT_ASTR,
                          _______, _______, _______,        IT_HASH,  IT_SLSH, _______
    ),
    /* Button Layer
    * ┌───┬───┬───┬───┬───┐       ┌───┬───┬───┬───┬───┐
    * │   │   │   │   │AF4│       │   │   │ ù │   │QMK│
    * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
    * │ à │   │   │PSC│   │       │   │   │ è │ ì │ ò │
    * ├───┼───┼───┼───┼───┤       ├───┼───┼───┼───┼───┤
    * │UND│CUT│CPY│PST│RDO│       │RDO│PST│CPY│CUT│UND│
    * └───┴───┴───┴───┴───┘       └───┴───┴───┴───┴───┘
    *           ┌───┐                   ┌───┐
    *           │   ├───┐           ┌───┤   │
    *           └───┤UND├───┐   ┌───┤   ├───┘
    *               └───┤   │   │   ├───┘
    *                   └───┘   └───┘
    */
    [_BUTTON] = LAYOUT_split_3x5_3(
        _______, _______, _______, _______, LALT(KC_F4),        _______, _______,  IT_OGRV, IT_UGRV, QK_BOOTLOADER,
        _______, _______, _______, LSG(KC_S), _______,      _______,  TD(TD_EMAIL), TD(TD_EGRV_SFT), IT_IGRV, IT_AGRV,
        C(KC_Z), C(KC_X), C(KC_C), C(KC_V), C(KC_Y),        C(KC_Y),     C(KC_V),   C(KC_C), C(KC_X), C(KC_Z),
                          _______, C(KC_Z), _______,        _______,     _______,   _______
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
        KC_TAB, KC_Q, KC_W,         KC_E,   KC_R,             KC_T,   KC_Y,    KC_U,   TD(TD_Y_CLIP), TO(_BASE),
        KC_4, KC_A, KC_S,         KC_D,   KC_F,             KC_M,   KC_N,    KC_E,   KC_I,          KC_O,
        KC_3, KC_Z, KC_X,         KC_C,   KC_V,             KC_K,   KC_L,    IT_DOT, IT_COMM,       TD(TD_MPLY_MNXT_MPRV),
                    CKC_1, KC_SPC, CKC_2,     KC_ENT, KC_BSPC, _______
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
