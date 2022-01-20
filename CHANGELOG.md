# Change log

## 0.9.11 on 2022/01/21
### Duplex matrix 😎
- Now we encourage you to make a duplex matrix keyboard with Seeed XIAO RP2040  
  Check details of the API here: [https://github.com/picoruby/prk_firmware/wiki/Keyscan-matrix#duplex-and-round-robin-matrix](https://github.com/picoruby/prk_firmware/wiki/Keyscan-matrix#duplex-and-round-robin-matrix)

## 0.9.10 on 2022/01/06
### Keyboard#define_composite_key

Now you can define a composite key that reports multiple keycodes at once.

Let's say there is a five-keys pad. You can make the most useful programming tool like this:

```ruby
kbd.add_layer :default, %i(KC_SPACE CUT COPY PASTE KC_ENTER)
kbd.define_composite_key :CUT,   %i(KC_LCTL KC_X)
kbd.define_composite_key :COPY,  %i(KC_LCTL KC_C)
kbd.define_composite_key :PASTE, %i(KC_LCTL KC_V)
```

You can also write the equivalent keymap in this way:

```ruby
kbd.add_layer :default, [ :KC_SPACE, [:KC_LCTL, :KC_X], [:KC_LCTL, :KC_C], [:KC_LCTL, :KC_V], :KC_ENTER ]
```

If you prefer to make an array of symbols with `%i` syntax, the former should seem elegant.

If you don't mind typing more colons, commas, and nested brackets, the latter would be intuitive.

## 0.9.9 on 2021/12/15
### Supports direct scan👏

- Some keyboards support **direct scan**. e.g. https://www.sho-k.co.uk/tech/1246.html  
  Now you can configure it as follow:

  ```ruby
  kbd.set_scan_mode = :direct
  kbd.init_pins(
    [],
    [ 8, 27, 28, 29, 9, 26 ]
  )
  ```
  or

  ```ruby
  kbd.init_direct_pins(
    [ 8, 27, 28, 29, 9, 26 ]
  )
  ```

## 0.9.8 on 2021/12/10
### RotaryEncoder enhancement
- Multiple encoders can be configured on a unit🎛
- `:RGB_xxx` keycodes work with encoders🌈
- The accuracy of rotation made a big improvement😎
### Bug fix
- Some tiny bugs are fixed

## 0.9.7 on 2021/12/02
### Bug fix
- `keymap.rb` with CRLF line terminators now works

## 0.9.6 on 2021/11/30
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

## 0.9.5 on 2021/10/27
### Breaking Change 🌈
- Amend names of RGB.effect
  - from `:rainbow` to `:swirl`
  - from `:breathing` to `rainbow_mood`
### Improvements
- Some keycodes for Japanese keyboard added. See [pull/31](https://github.com/picoruby/prk_firmware/pull/31)
- Improved startup stability. The startup time is increased, but be patient!

## 0.9.4 on 2021/09/24
### Improvement
- Now you can use "Right side flipped split" keyboard (eg. [Zinc](https://www.sho-k.co.uk/tech/735.html)). See [pull/22](https://github.com/picoruby/prk_firmware/pull/22)

## 0.9.3 on 2021/09/17
### Improvement
- Abbreviated keynames things like `KC_ENT` for `KC_ENTER` can be used in `keymap.rb`. See [bc23e52](https://github.com/picoruby/prk_firmware/commit/bc23e52f51c2899ce5309643f0ab89606a9b469d)

## 0.9.2 on 2021/09/15
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

## 0.9.1 on 2021/09/12
### Small improvement
- Sparkfun Pro Micro RP2040 will reboot to BOOTSEL mode if you double-press RESET button without detaching USB cable!

  ![](doc/images/RP2040_boards.jpg)

## 0.9.0 on 2021/09/10 (First release🎊)
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
