[![Ruby CI](https://github.com/picoruby/prk_firmware/actions/workflows/ruby.yml/badge.svg)](https://github.com/picoruby/prk_firmware/actions/workflows/ruby.yml)

## PRK Firmware

PRK Firmware is a framework for DIY-keyboards written and configured in [PicoRuby](https://github.com/picoruby/picoruby) which is an alternative mruby implementation targeting on one-chip microcontrollers.

### Features

- A "keymap" can be configured in Ruby which is a concise, readable and happy language
- RP2040 is the target platform microcontroller
- You can write your own "action" which will be invoked when you tap or hold a key.

### Documents

See Wiki pages: [https://github.com/picoruby/prk_firmware/wiki](https://github.com/picoruby/prk_firmware/wiki)

### Presentations

The author gave a talk about PRK Firmware in RubyConf 2021.

See the video on [YouTube](https://www.youtube.com/watch?v=SLSwn41iJX4&t=12s).

Japanese talk is available at [RubyKaigi Takeout 2021](https://rubykaigi.org/2021-takeout/presentations/hasumikin.html).

### Roadmap

- [x] Non-split-type keyboard
- [ ] Split-type keyboard
  - [x] Symmetrical type. eg) Crkbd
  - [ ] Asymmetrical type. eg) ???
  - [x] UART communication between left and right
  - [ ] I2C communication between left and right
- [x] Macros
- [ ] Media keys
- [x] RGBLED. An example on [picoruby/prk_crkbd](https://github.com/picoruby/prk_crkbd/blob/main/keymap.rb)
- [ ] OLED display
- [x] Rotary encoders. An example on [prk_helix_rev3/keymap.rb](https://github.com/picoruby/prk_helix_rev3/blob/master/keymap.rb)
- [ ] Mouse / Trackball
- [x] Debugging over a serial port

### Contributing

Fork, patch and send a pull request.

#### For those who are willing to contribute to PRK or write your own keymaps:

- It's possible that your Ruby code can't be compiled as you wish
  - Remember that "Ruby" in PRK is neither CRuby nor even mruby
  - [PicoRuby](https://github.com/picoruby/picoruby) doesn't support some Ruby syntax and may have bugs. It would be great if you send a patch, too!
- Unlike QMK Firmware, prk_firmware repository doesn't include individual keymaps

### License

Copyright © 2021-2022 HASUMI Hitoshi. See MIT-LICENSE for further details.
