AUDIO_SUPPORTED			= no
LTO_ENABLE				= yes
MOUSEKEY_ENABLE			= no
COMBO_ENABLE			= yes
CAPS_WORD_ENABLE		= yes
TAP_DANCE_ENABLE		= yes
BOOTMAGIC_ENABLE		= yes
OS_DETECTION_ENABLE		= yes

BOOTLOADER				= rp2040
SERIAL_DRIVER = vendor

# Ensure QMK’s keymap-introspection build pulls in the full keymap.c
# so that combos, tap-dance actions, and community modules are detected correctly.
INTROSPECTION_KEYMAP_C = keymap.c

# Remove unused features to save firmware space
SPACE_CADET_ENABLE 	= no
GRAVE_ESC_ENABLE 	= no
MAGIC_ENABLE 		= no
MUSIC_ENABLE 		= no

# used to communicate with the host through a dedicated HID interface
RAW_ENABLE = yes
