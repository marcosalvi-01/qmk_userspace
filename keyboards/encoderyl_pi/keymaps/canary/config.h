// Copyright 2024 Marco-Salvi (@Marco-Salvi)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

#define MASTER_RIGHT

#define TAPPING_TERM 300
#define TAPPING_TERM_PER_KEY

#define ALT_TAB_TIMER 800

#define ACHORDION_TIMEOUT 800

#define CAPS_WORD_IDLE_TIMEOUT 10000  // 10 seconds.

#define QUICK_TAP_TERM 0    // Disable key repeat on mod tap keys

#define PERMISSIVE_HOLD

// Disable unused features to reduce firmware size
#undef LOCKING_SUPPORT_ENABLE
#undef LOCKING_RESYNC_ENABLE
#define NO_ACTION_ONESHOT

#define ACHORDION_STREAK
