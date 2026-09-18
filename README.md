# QMK Userspace

This is a template repository which allows for an external set of QMK keymaps to be defined and compiled. This is useful for users who want to maintain their own keymaps without having to fork the main QMK repository.

## Howto configure your build targets

1. Run the normal `qmk setup` procedure if you haven't already done so -- see [QMK Docs](https://docs.qmk.fm/#/newbs) for details.
1. Fork this repository
1. Clone your fork to your local machine
1. Enable userspace in QMK config using `qmk config user.overlay_dir="$(realpath qmk_userspace)"`
1. Add a new keymap for your board using `qmk new-keymap`
    * This will create a new keymap in the `keyboards` directory, in the same location that would normally be used in the main QMK repository. For example, if you wanted to add a keymap for the Planck, it will be created in `keyboards/planck/keymaps/<your keymap name>`
    * You can also create a new keymap using `qmk new-keymap -kb <your_keyboard> -km <your_keymap>`
    * Alternatively, add your keymap manually by placing it in the location specified above.
    * `layouts/<layout name>/<your keymap name>/keymap.*` is also supported if you prefer the layout system
1. Add your keymap(s) to the build by running `qmk userspace-add -kb <your_keyboard> -km <your_keymap>`
    * This will automatically update your `qmk.json` file
    * Corresponding `qmk userspace-remove -kb <your_keyboard> -km <your_keymap>` will delete it
    * Listing the build targets can be done with `qmk userspace-list`
1. Commit your changes

## Howto build with GitHub

1. In the GitHub Actions tab, enable workflows
1. Push your changes above to your forked GitHub repository
1. Look at the GitHub Actions for a new actions run
1. Wait for the actions run to complete
1. Inspect the Releases tab on your repository for the latest firmware build

## Howto build locally

1. Run the normal `qmk setup` procedure if you haven't already done so -- see [QMK Docs](https://docs.qmk.fm/#/newbs) for details.
1. Fork this repository
1. Clone your fork to your local machine
1. `cd` into this repository's clone directory
1. Set global userspace path: `qmk config user.overlay_dir="$(realpath .)"` -- you MUST be located in the cloned userspace location for this to work correctly
    * This will be automatically detected if you've `cd`ed into your userspace repository, but the above makes your userspace available regardless of your shell location.
1. Compile normally: `qmk compile -kb your_keyboard -km your_keymap` or `make your_keyboard:your_keymap`

Alternatively, if you configured your build targets above, you can use `qmk userspace-compile` to build all of your userspace targets at once.

## Flashing the RP2040 keyboard

For the `encoderyl_pi` RP2040 keyboard, build from the userspace, then flash from the QMK firmware checkout:

```sh
cd ~/qmk_userspace
make encoderyl_pi:default
cd ~/qmk_firmware
make encoderyl_pi:default:flash
```

The `:flash` target waits for an RP2040 UF2 bootloader drive. Enter bootloader with `QK_BOOTLOADER` (or by holding BOOTSEL while reconnecting the keyboard). QMK's UF2 helper only detects mounted drives, so it can wait indefinitely if Linux detects the keyboard but does not mount it.

If the drive is not mounted, use another terminal:

```sh
lsblk -o NAME,LABEL,FSTYPE,MOUNTPOINTS
udisksctl mount -b /dev/sda1
```

Use the device labelled `RPI-RP2`; its device name may not be `/dev/sda1` on every system. The flash target should detect the newly mounted drive and copy the firmware automatically. Alternatively, copy the generated UF2 manually:

```sh
mountpoint="$(findmnt -n -o TARGET /dev/sda1)"
cp ~/qmk_userspace/encoderyl_pi_default.uf2 "$mountpoint/NEW.UF2"
sync
```

The keyboard reboots after the UF2 file is accepted. RP2040 uses USB mass-storage UF2 flashing, not `dfu-util`.

