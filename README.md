## PRK Firmware (public beta)

PRK Firmware is a framework for DIY-keyboards written and configured in [PicoRuby](https://github.com/picoruby/picoruby) which is an alternative mruby implementation targeting on one-chip microcontroller.

### Features

- A "keymap" can be configured in Ruby which is a concise, readable and happy language
- RP2040 is the target platform microcontroller
- You can write your own "action" which will be invoked when you tap or hold a key ([example]()(TBD))

### Boards

The following boards have been confirmed to work as of now:

- Raspberry Pi Pico
- Sparkfun Pro Micro RP2040 (DEV-17177)

![](doc/images/RP2040_boards.jpg)

_(left: Raspberry Pi Pico / right: Sparkfun Pro Micro RP2040)_

### Roadmap

- [x] Unsplit keyboard
- [ ] Split keyboard
  - [x] Symmetrical type. eg) Crkbd
  - [ ] Asymmetrical type. eg) ???
  - [x] UART communication between left and right
  - [ ] I2C communication between left and right
- [ ] Macros
- [ ] Media keys
- [x] *new!* RGBLED. An example on [picoruby/prk_crkbd](https://github.com/picoruby/prk_crkbd/blob/main/keymap.rb#L61-L76)
- [ ] OLED display
- [ ] Rotary encoder

### Getting started

- Setup Raspberry Pi Pico C/C++ SDK

  - Follow the instructions on [https://github.com/raspberrypi/pico-sdk#quick-start-your-own-project](https://github.com/raspberrypi/pico-sdk#quick-start-your-own-project)
    - Make sure you have `PICO_SDK_PATH` environment variable

  - Be knowledgeable how to install a UF2 file into Raspi Pico on [https://www.raspberrypi.org/documentation/rp2040/getting-started/#getting-started-with-c](https://www.raspberrypi.org/documentation/rp2040/getting-started/#getting-started-with-c)

  - [https://learn.sparkfun.com/tutorials/pro-micro-rp2040-hookup-guide](https://learn.sparkfun.com/tutorials/pro-micro-rp2040-hookup-guide) will also help you if you use Sparkfun Pro Micro RP2040

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

- Build with `cmake` and `make`

    ```
    cd prk_meishi2/build
    cmake ../../..
    make
    ```

    Now you should have `prk_firmware.uf2` file in `prk_firmware/keyboards/prk_meishi2/build/` directory.

### Contributing

Fork, clone, patch and send a pull request.

#### For those who are willing to contribute to PRK or write your own keymaps:

- It's possible that your Ruby code can't be compiled as you wish
  - Remember that "Ruby" in PRK is neither CRuby nor even mruby
  - [PicoRuby](https://github.com/picoruby/picoruby) doesn't support some Ruby syntax and may have bugs. It would be great if you send a patch, too!
- Unlike QMK Firmware, prk_firmware repository doesn't include individual keymaps

### Keymaps for example

- Raspberrypi Pi Pico
  - PiPi Gherkin (coming soon)

- Sparkfun Pro Micro RP2040 (DEV-17717)
  - [meishi2](https://github.com/picoruby/prk_meishi2)
  - [Crkbd](https://github.com/picoruby/prk_crkbd)
  - [Claw44](https://github.com/picoruby/prk_claw44)
  - [Helix rev3](https://github.com/picoruby/prk_helix_rev3)

#### To keyboard creators:

Feel free to reach out to [me](https://twitter.com/hasumikin) if you want me to link your keymap on this README.

### FAQ

- Q: Can I use Sparkfun Pro Micro RP2040 as a drop-in replacement instead of a Pro Micro without having to modify the CRKBD PCB?
  
  A: Yes you can! However, note that exising LEDs on your CRKBD may not blink ~~RGBLED feature is still not implemented on PRK. And don't expect your existing CRKBS's LEDs will blink even if the feature is ready~~ because the logic voltage of RP2040 is 3.3V while 5V on "normal Pro Micro". It depends on the specificaion of LED.
  
  In terms of 3.3V, you should be careful of the same thing which is warned on Proton-C: https://qmk.fm/proton-c/
  
  > Some of the PCBs compatible with Pro Micro have VCC (3.3V) and RAW (5V) pins connected (shorted) on the pcb. Using the Proton C will short 5V power from USB and regulated 3.3V which is connected directly to the MCU. Shorting those pins may damage the MCU on the Proton C.
  > 
  > So far, it appears that this is only an issue on the Gherkin PCBs, but other PCBs may be affected in this way.
  > 
  > In this case, you may want to not hook up the RAW pin at all.

- Q: PRK Firmware no longer works after pulling the newest master branch, why?
  
  A: It likely happens when submodules like PicoRuby were also updated, try `git submodule update`. Besides, PRK may get a breaking change as it is still a beta version. Take a good look at [CHANGELOG.md](CHANGELOG.md)


### License

Copyright © 2021 HASUMI Hitoshi. See MIT-LICENSE for further details.

