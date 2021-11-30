# Change log

## 0.9.6 in 2021/11/30
### Big improvements
- RGB feature
  - Keycodes like `:RGB_TOG` (to toggle the LED between *on* and *off*, for example) are added
    - See more details on [wiki/RGB-feature](https://github.com/picoruby/prk_firmware/wiki/RGB-feature)
  - You can set default values of an RGB instance in keymap.rb. eg) `rgb = RGB.new; rgb.speed` to change the speed of blinking
    - See also [wiki/RGB-feature#in-your-keymaprb](https://github.com/picoruby/prk_firmware/wiki/RGB-feature#in-your-keymaprb)
  - On a split type keyboard like CRKBD, LEDs of left and right will be synchronized when you enable "Mutual UART" (see below)
- Mutual UART communication on a split type keyboard on *TRS* (not TRRS) cable
  - This *experimental* feature realizes
    - Synchronized RGB blinking on both halves (anchor half and partner half)
    - Keycodes like `:RGB_TOG` work also on the "partner half"
  - See [wiki/Mutual-UART-communication](https://github.com/picoruby/prk_firmware/wiki/Mutual-UART-communication)

## 0.9.5 in 2021/10/27
### Breaking Change 🌈
- Amend names of RGB.effect
  - from `:rainbow` to `:swirl`
  - from `:breathing` to `rainbow_mood`
### Improvements
- Some keycodes for Japanese keyboard added. See [pull/31](https://github.com/picoruby/prk_firmware/pull/31)
- Improved startup stability. The startup time is increased, but be patient!

## 0.9.4 in 2021/09/24
### Improvement
- Now you can use "Right side flipped split" keyboard (eg. [Zinc](https://www.sho-k.co.uk/tech/735.html)). See [pull/22](https://github.com/picoruby/prk_firmware/pull/22)

## 0.9.3 in 2021/09/17
### Improvement
- Abbreviated keynames things like `KC_ENT` for `KC_ENTER` can be used in `keymap.rb`. See [bc23e52](https://github.com/picoruby/prk_firmware/commit/bc23e52f51c2899ce5309643f0ab89606a9b469d)

## 0.9.2 in 2021/09/15
### Useful feature 🎉
- You can see debug print on a USB serial port that will be helpful if your `keymap.rb` doesn't work well
  - Configuration:
      
      ```
      Baud: 115200
      Data bits: 8
      Parity: None
      Stop bits: 1
      Flow control: None
      ```

  ![](doc/images/serial_port.png)

## 0.9.1 in 2021/09/12
### Small improvement
- Sparkfun Pro Micro RP2040 will reboot to BOOTSEL mode if you double-press RESET button without detaching USB cable!

  ![](doc/images/RP2040_boards.jpg)

## 0.9.0 in 2021/09/10 (First release🎊)
### BIG BIG IMPROVEMENT 🍣
- You no longer need any compiler toolchain!!!
- No longer detaching USB cable every time amending your keymap, too!!!
- See README.md 👀

## 2021/08/17
### Breaking Change 💣
- Code upgraded to correspond to the newest pico-sdk as of today. Please upgrade your pico-sdk

## 2021/08/16
### Changed
- Bump up PicoRuby to "mruby3 version"
- There is no external change though, if your keymap doesn't work, try to add `sleep 1` to the beggining of your keymap.rb

## 2021/06/19
###  New feature added
- Rotary Encoders &#x1F39B;
  - Find an example in [prk_helix_rev3/keymap.rb](https://github.com/picoruby/prk_helix_rev3/blob/master/keymap.rb)

## 2021/06/01
### Breaking Change 💣
- The second argument of `Keyboard#add_layer`

  Before the change: `Array[Array[Symbol]]`
  ```ruby
  kbd.add_layer :default, [
    %i(KC_ESCAPE KC_Q KC_W KC_E KC_R KC_T KC_Y KC_U KC_I KC_O KC_P      KC_MINUS),
    %i(KC_TAB    KC_A KC_S KC_D KC_F KC_G KC_H KC_J KC_K KC_L KC_SCOLON KC_BSPACE),
    ...
  ]
  ```

  After the change: `Array[Symbol]`
  ```ruby
  kbd.add_layer :default, %i(
    KC_ESCAPE KC_Q KC_W KC_E KC_R KC_T KC_Y KC_U KC_I KC_O KC_P      KC_MINUS
    KC_TAB    KC_A KC_S KC_D KC_F KC_G KC_H KC_J KC_K KC_L KC_SCOLON KC_BSPACE
    ...
  )
  ```

## 2021/05/21
### Dependency
- CRuby (MRI) for static type checking by RBS and Steep
### Changed
- Directory of Ruby source is changed from `src/` to `src/ruby/`

## 2021/05/08
###  Added
- RGB class 🌈

## 2021/04/14
- Published as a public beta 🎉
