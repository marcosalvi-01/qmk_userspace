// Copyright 2024 Marco-Salvi (@Marco-Salvi)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define MASTER_RIGHT

// mod-tap configs
#define TAPPING_TERM 200
#define TAPPING_TERM_PER_KEY
#define PERMISSIVE_HOLD
#define COMBO_TERM_PER_COMBO

// qmk's achordion
#define CHORDAL_HOLD

#define CAPS_WORD_IDLE_TIMEOUT 10000

// Disable key repeat on mod tap keys
#define QUICK_TAP_TERM 0

// for raw hid
#define RAW_USAGE_PAGE 0xFF60
#define RAW_USAGE_ID 0x61

// Disable unused features to reduce firmware size
#undef LOCKING_SUPPORT_ENABLE
#undef LOCKING_RESYNC_ENABLE
#define NO_ACTION_ONESHOT

#define OS_DETECTION_KEYBOARD_RESET
#define OS_DETECTION_SINGLE_REPORT
