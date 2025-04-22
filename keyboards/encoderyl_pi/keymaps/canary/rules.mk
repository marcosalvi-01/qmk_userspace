AUDIO_SUPPORTED = no        # Audio is not supported

SERIAL_DRIVER = vendor
LTO_ENABLE 			= yes
MOUSEKEY_ENABLE 	= yes       # Mouse keys
COMBO_ENABLE 		= yes      # Enable combo keycodes
CAPS_WORD_ENABLE 	= yes
TAP_DANCE_ENABLE 	= yes      # Tap dance

# Remove unused features to save firmware space
SPACE_CADET_ENABLE 	= no
GRAVE_ESC_ENABLE 	= no
MAGIC_ENABLE 		= no
MUSIC_ENABLE 		= no

# used to communicate with the host through a dedicated HID interface
RAW_ENABLE = yes

# Add custom shift and achordion
SRC += features/custom_shift_keys.c		# (https://getreuer.info/posts/keyboards/custom-shift-keys/index.html)
SRC += features/achordion.c 			# (https://getreuer.info/posts/keyboards/achordion/index.html)

