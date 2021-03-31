$mutex = Mutex.new

$mutex.lock

GPIO_OUT = 1
GPIO_IN  = 0

HI = 1
LO = 0

MOD_KEYCODE = {
  KC_LCTL: 0b00000001,
  KC_LSFT: 0b00000010,
  KC_LALT: 0b00000100,
  KC_LGUI: 0b00001000,
  KC_RALT: 0b00010000,
  KC_RSFT: 0b00100000,
  KC_RCTL: 0b01000000,
  KC_RGUI: 0b10000000
}

KEYCODE = {
  KC_NO:             0x00,
  KC_ROLL_OVER:      0x01,
  KC_POST_FAIL:      0x02,
  KC_UNDEFINED:      0x03,
  KC_A:              0x04,
  KC_B:              0x05,
  KC_C:              0x06,
  KC_D:              0x07,
  KC_E:              0x08,
  KC_F:              0x09,
  KC_G:              0x0a,
  KC_H:              0x0b,
  KC_I:              0x0c,
  KC_J:              0x0d,
  KC_K:              0x0e,
  KC_L:              0x0f,
  KC_M:              0x10,
  KC_N:              0x11,
  KC_O:              0x12,
  KC_P:              0x13,
  KC_Q:              0x14,
  KC_R:              0x15,
  KC_S:              0x16,
  KC_T:              0x17,
  KC_U:              0x18,
  KC_V:              0x19,
  KC_W:              0x1a,
  KC_X:              0x1b,
  KC_Y:              0x1c,
  KC_Z:              0x1d,
  KC_1:              0x1e,
  KC_2:              0x1f,
  KC_3:              0x20,  # 0x20
  KC_4:              0x21,
  KC_5:              0x22,
  KC_6:              0x23,
  KC_7:              0x24,
  KC_8:              0x25,
  KC_9:              0x26,
  KC_0:              0x27,
  KC_ENTER:          0x28,
  KC_ESCAPE:         0x29,
  KC_BSPACE:         0x2a,
  KC_TAB:            0x2b,
  KC_SPACE:          0x2c,
  KC_MINUS:          0x2d,
  KC_EQUAL:          0x2e,
  KC_LBRACKET:       0x2f,
}
k = {
  KC_RBRACKET:       0x30,  # 0x30
  KC_BSLASH:         0x31,
  KC_NONUS_HASH:     0x32,
  KC_SCOLON:         0x33,
  KC_QUOTE:          0x34,
  KC_GRAVE:          0x35,
  KC_COMMA:          0x36,
  KC_DOT:            0x37,
  KC_SLASH:          0x38,
  KC_CAPSLOCK:       0x39,
  KC_F1:             0x3a,
  KC_F2:             0x3b,
  KC_F3:             0x3c,
  KC_F4:             0x3d,
  KC_F5:             0x3e,
  KC_F6:             0x3f,
  KC_F7:             0x40,  # 0x40
  KC_F8:             0x41,
  KC_F9:             0x42,
  KC_F10:            0x43,
  KC_F11:            0x44,
  KC_F12:            0x45,
  KC_PSCREEN:        0x46,
  KC_SCROLLLOCK:     0x47,
  KC_PAUSE:          0x48,
  KC_INSERT:         0x49,
  KC_HOME:           0x4a,
  KC_PGUP:           0x4b,
  KC_DELETE:         0x4c,
  KC_END:            0x4d,
  KC_PGDOWN:         0x4e,
  KC_RIGHT:          0x4f,
  KC_LEFT:           0x50,  # 0x50
  KC_DOWN:           0x51,
  KC_UP:             0x52,
  KC_NUMLOCK:        0x53,
  KC_KP_SLASH:       0x54,
  KC_KP_ASTERISK:    0x55,
  KC_KP_MINUS:       0x56,
  KC_KP_PLUS:        0x57,
  KC_KP_ENTER:       0x58,
  KC_KP_1:           0x59,
  KC_KP_2:           0x5a,
  KC_KP_3:           0x5b,
  KC_KP_4:           0x5c,
  KC_KP_5:           0x5d,
  KC_KP_6:           0x5e,
  KC_KP_7:           0x5f,
}
KEYCODE.merge!(k)
k = {
  KC_KP_8:           0x60,  # 0x60
  KC_KP_9:           0x61,
  KC_KP_0:           0x62,
  KC_KP_DOT:         0x63,
  KC_NONUS_BSLASH:   0x64,
  KC_APPLICATION:    0x65,
  KC_POWER:          0x66,
  KC_KP_EQUAL:       0x67,
  KC_F13:            0x68,
  KC_F14:            0x69,
  KC_F15:            0x6a,
  KC_F16:            0x6b,
  KC_F17:            0x6c,
  KC_F18:            0x6d,
  KC_F19:            0x6e,
  KC_F20:            0x6f,
  KC_F21:            0x70,  # 0x70
  KC_F22:            0x71,
  KC_F23:            0x72,
  KC_F24:            0x73,
  KC_EXECUTE:        0x74,
  KC_HELP:           0x75,
  KC_MENU:           0x76,
  KC_SELECT:         0x77,
  KC_STOP:           0x78,
  KC_AGAIN:          0x79,
  KC_UNDO:           0x7a,
  KC_CUT:            0x7b,
  KC_COPY:           0x7c,
  KC_PASTE:          0x7d,
  KC_FIND:           0x7e,
  KC__MUTE:          0x7f,
}
KEYCODE.merge!(k)
k = {
  KC_LANG1:          0x90,  # 0x90
  KC_LANG2:          0x91,
  KC_LANG3:          0x92,
  KC_LANG4:          0x93,
  KC_LANG5:          0x94,
  KC_LANG6:          0x95,
  KC_LANG7:          0x96,
  KC_LANG8:          0x97,
  KC_LANG9:          0x98,
  KC_ALT_ERASE:      0x99,
  KC_SYSREQ:         0x9a,
  KC_CANCEL:         0x9b,
  KC_CLEAR:          0x9c,
  KC_PRIOR:          0x9d,
  KC_RETURN:         0x9e,
  KC_SEPARATOR:      0x9f,
  KC_OUT:            0xa0,  # 0xA0
  KC_OPER:           0xa1,
  KC_CLEAR_AGAIN:    0xa2,
  KC_CRSEL:          0xa3,
  KC_EXSEL:          0xa4,
}
KEYCODE.merge!(k)

class Keyboard
  def initialize(rows, cols)
    @before_filters = Array.new
    @layer = Hash.new
    @mode_keys = Array.new
    @switches = Array.new
    @led = LED.new
    @rows = rows
    @cols = cols
    @rows.each do |pin|
      gpio_init(pin)
      gpio_set_dir(pin, GPIO_IN);
      gpio_pull_up(pin);
    end
    @cols.each do |pin|
      gpio_init(pin)
      gpio_set_dir(pin, GPIO_OUT);
      gpio_put(pin, HI);
    end
  end

  # Input
  #   name:    default, map: [ [ :KC_A, :KC_B, :KC_LCTL,   :MACRO_1 ],... ]
  # ↓
  # Result
  #   layer: { default:      [ [ -0x04, -0x05, 0b00000001, :MACRO_1 ],... ] }
  def add_layer(name, map)
    map.each_with_index do |row, row_index|
      row.each_with_index do |key, col_index|
        if KEYCODE[key]
          map[row_index][col_index] = KEYCODE[key] * -1
        elsif MOD_KEYCODE[key]
          map[row_index][col_index] = MOD_KEYCODE[key]
        end
      end
    end
    @layer[name] = map
    @first_layer_name ||= name
  end

  # param[0] :release_key
  # param[1] :hold_layer
  # param[2] :release_threshold
  # param[3] :repush_threshold
  def define_mode_key(key_name, param)
    @layer.each do |layer, map|
      map.each_with_index do |row, row_index|
        row.each_with_index do |key_symbol, col_index|
          if key_name == key_symbol
            @mode_keys << {
              layer:             layer,
              release_key:       ( (KEYCODE[param[0]] || 0) * -1),
              hold_layer:        param[1],
              release_threshold: (param[2] || 0),
              repush_threshold:  (param[3] || 0),
              switch:            [row_index, col_index],
              prev_state:        :released,
              pushed_at:         0,
              released_at:       0,
            }
            break
          end
        end
      end
    end
  end

  def keys_include?(key)
    @keycodes.include? KEYCODE[key].chr
  end

  # MOD_KEYCODE = {
  #   KC_LCTL: 0b00000001,
  #   KC_LSFT: 0b00000010,
  #   KC_LALT: 0b00000100,
  #   KC_LGUI: 0b00001000,
  #   KC_RALT: 0b00010000,
  #   KC_RSFT: 0b00100000,
  #   KC_RCTL: 0b01000000,
  #   KC_RGUI: 0b10000000
  # }
  def invert_sft
    if ( (@modifier & 0b00000010) | (@modifier & 0b00100000) ) > 0
      @modifier &= 0b11011101
    else
      @modifier |= 0b00000010
    end
  end

  def before_report(&block)
    @before_filters << block
  end

  def start!
    while true
      now = board_millis
      keycode_pos = 0
      @keycodes = "\000\000\000\000\000\000"
      layer = @first_layer_name

      @switches.clear
      @modifier = 0

      # detect physical switches that are pushed
      @cols.each_with_index do |col_pin, col|
        gpio_put(col_pin, LO)
        @rows.each_with_index do |row_pin, row|
          @switches << [row, col] if gpio_get(row_pin) == LO
        end
        gpio_put(col_pin, HI)
      end

      @mode_keys.each do |mode_key|
        next if mode_key[:layer] != layer
        if @switches.include?(mode_key[:switch])
          @led.on
          case mode_key[:prev_state]
          when :released
            mode_key[:pushed_at] = now
            mode_key[:prev_state] = :pushed
            layer = mode_key[:hold_layer]
          when :pushed
            layer = mode_key[:hold_layer]
          when :pushed_then_released
            if now - mode_key[:released_at] <= mode_key[:repush_threshold]
              mode_key[:prev_state] = :pushed_then_released_then_pushed
            end
          when :pushed_then_released_then_pushed
            @keycodes[keycode_pos] = (mode_key[:release_key] * -1).chr
            keycode_pos += 1
            break
          end
        else
          @led.off
          case mode_key[:prev_state]
          when :pushed
            if now - mode_key[:pushed_at] <= mode_key[:release_threshold]
              @keycodes[keycode_pos] = (mode_key[:release_key] * -1).chr
              keycode_pos += 1
              mode_key[:prev_state] = :pushed_then_released
              mode_key[:released_at] = now
              break
            else
              mode_key[:prev_state] = :released
            end
          when :pushed_then_released
            if now - mode_key[:released_at] > mode_key[:release_threshold]
              mode_key[:prev_state] = :released
            end
          when :pushed_then_released_then_pushed
            mode_key[:prev_state] = :released
          end
        end
      end

      layer = @layer[layer]
      @switches.each do |switch|
        key = layer[switch[0]][switch[1]]
        next unless key.is_a?(Fixnum)
        if key < 0 # Normal keys
          @keycodes[keycode_pos] = (key * -1).chr
          keycode_pos += 1
        else # Modifier keys
          @modifier |= key
        end
        break if keycode_pos > 5
      end

      @before_filters.each do |block|
        block.call
      end

      report_hid(@modifier, @keycodes)
      time = board_millis - now
      sleep_ms(time) if time > 0
    end
  end

end

$mutex.unlock
