.SILENT:

MAKEFLAGS += --no-print-directory

QMK_USERSPACE := $(patsubst %/,%,$(dir $(shell realpath "$(lastword $(MAKEFILE_LIST))")))
ifeq ($(QMK_USERSPACE),)
    QMK_USERSPACE := $(shell pwd)
endif

QMK_FIRMWARE_ROOT = $(shell qmk config -ro user.qmk_home | cut -d= -f2 | sed -e 's@^None$$@@g')
ifeq ($(QMK_FIRMWARE_ROOT),)
    # Try using a default location instead of erroring out
    QMK_FIRMWARE_ROOT=${HOME}/qmk_firmware/
    $(info Cannot determine qmk_firmware location from QMK config. Using default: $(QMK_FIRMWARE_ROOT))
    # Check if the default location exists
    ifeq ($(wildcard $(QMK_FIRMWARE_ROOT)),)
        $(error QMK firmware directory not found at $(QMK_FIRMWARE_ROOT))
    endif
endif

# Helper target to sync keyboard files from userspace to QMK firmware
sync_keyboards:
	@echo "Syncing keyboards from userspace to QMK firmware..."
	@# Find directories in keyboards folder and remove them from QMK firmware dir before copying
	@for kb in $(wildcard ${QMK_USERSPACE}/keyboards/*); do \
		kb_name=$$(basename $$kb); \
		if [ -d "${QMK_FIRMWARE_ROOT}/keyboards/$$kb_name" ]; then \
			echo "Removing existing keyboard: $$kb_name"; \
			rm -rf ${QMK_FIRMWARE_ROOT}/keyboards/$$kb_name; \
		fi; \
	done
	@# Copy the contents of the keyboard directory from the userspace to the qmk firmware dir
	@echo "Copying keyboards to QMK firmware directory..."
	cp -r ${QMK_USERSPACE}/keyboards/* ${QMK_FIRMWARE_ROOT}/keyboards/

%: sync_keyboards
	+$(MAKE) -C $(QMK_FIRMWARE_ROOT) $(MAKECMDGOALS) QMK_USERSPACE=$(QMK_USERSPACE)
