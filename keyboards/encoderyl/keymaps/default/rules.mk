LTO_ENABLE				= yes
MOUSEKEY_ENABLE			= no
COMBO_ENABLE			= yes
CAPS_WORD_ENABLE		= yes
TAP_DANCE_ENABLE		= yes
BOOTMAGIC_ENABLE		= yes
BOOTLOADER				= caterina
INTROSPECTION_KEYMAP_C  = keymap.c

# Remove unused features to save firmware space
SPACE_CADET_ENABLE 	= no
GRAVE_ESC_ENABLE 	= no
MAGIC_ENABLE 		= no
MUSIC_ENABLE 		= no

# used to communicate with the host through a dedicated HID interface
RAW_ENABLE = yes
