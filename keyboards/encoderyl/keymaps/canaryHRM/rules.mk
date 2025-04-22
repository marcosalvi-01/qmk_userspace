LTO_ENABLE 			 = yes
MOUSEKEY_ENABLE 	 = yes      # Mouse keys
COMBO_ENABLE 		 = yes      # Enable combo keycodes
CAPS_WORD_ENABLE 	 = yes
TAP_DANCE_ENABLE 	 = yes      # Tap dance
BOOTMAGIC_ENABLE 	 = yes      # Bootmagic
DEFERRED_EXEC_ENABLE = yes
BOOTLOADER 			 = caterina

# Remove unused features to save firmware space
SPACE_CADET_ENABLE 	 = no
GRAVE_ESC_ENABLE 	 = no
MAGIC_ENABLE 		 = no
MUSIC_ENABLE 		 = no

# Add custom shift and achordion
SRC += features/custom_shift_keys.c		# (https://getreuer.info/posts/keyboards/custom-shift-keys/index.html)
