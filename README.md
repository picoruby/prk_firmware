## PRK firmware (public beta)

PRK is a keyboard firmware written and configured in [PicoRuby](https://github.com/picoruby/picoruby) which is an alternative mruby implementation targeting on one-chip microcontroller.

### Features

- A "keymap" can be configured in Ruby which is a concise, readable and happy language
- RP2040 is the target platform microcontroller
- You can write your own "action" which will be invoked when you tap or hold a key ([example]()(TBD))

### Boards

- Raspberry Pi Pico
- Sparkfun Pro Micro RP2040 (DEV-17177)

![](doc/images/RP2040_boards.jpg)

_(left: Raspberry Pi Pico / right: DEV-17717)_

### Roadmap

- [x] Unsplit keyboard
- [ ] Split keyboard
  - [x] Symmetrical type. eg) Crkbd
  - [ ] Asymmetrical type. eg) ???
  - [x] UART communication between left and right
  - [ ] I2C communication between left and right
- [ ] LED
- [ ] OLED display
- [ ] Macros
- [ ] Media keys

### Getting started

- Setup Raspberry Pi Pico C/C++ SDK

  - Follow the instructions on [https://github.com/raspberrypi/pico-sdk#quick-start-your-own-project](https://github.com/raspberrypi/pico-sdk#quick-start-your-own-project)

  - Be knowledgeable how to install a UF2 file into Raspi Pico on [https://www.raspberrypi.org/documentation/rp2040/getting-started/#getting-started-with-c](https://www.raspberrypi.org/documentation/rp2040/getting-started/#getting-started-with-c)

- Clone the `prk_firmware` (this repository) wherever you like

    ```
    git clone --recursive https://github.com/picoruby/prk_firmware.git # Don't forget --recursive
    cd prk_firmware
    ```

- Clone a keymap repository, for example, "meishi2" which is a 2x2 matrix card-shaped keyboard in `keyboards` directory

    ```
    cd keyboards
    git clone https://github.com/picoruby/prk_meishi2.git
    ```

- Build with `cmake`

    ```
    cd prk_meishi2
    cmake ../..
    make
    ```

    Now you should have `prk_firmware.uf2` file in `prk_firmware/keyboards/prk_meishi2/` directory.

### Contributing

Fork, clone, patch and send a pull request.

- It's possible that your Ruby code can't be compiled as you wish
  - Remember that "Ruby" in PRK is neither CRuby nor even mruby
  - [PicoRuby](https://github.com/picoruby/picoruby) doesn't support some Ruby syntax and may have bugs. It would be great if you send a patch, too!
- Unlike QMK Firmware, prk_firmware repository doesn't include individual keymaps

### License

Copyright © 2021 HASUMI Hitoshi. See MIT-LICENSE for further details.
