$mutex = Mutex.new

$mutex.lock

HI = 1
LO = 0

MODS = {
  KC_LCTL: 0b00000001,
  KC_LSFT: 0b00000010,
  KC_LALT: 0b00000100,
  KC_LGUI: 0b00001000,
  KC_RALT: 0b00010000,
  KC_RSFT: 0b00100000,
  KC_RCTL: 0b01000000,
  KC_RGUI: 0b10000000
}

KEYCODES = {
  KC_A: 0x04,
  KC_B: 0x05,
  KC_C: 0x06,
  KC_D: 0x07,
  KC_E: 0x08,
  KC_F: 0x09,
  KC_G: 0x0a,
  KC_H: 0x0b,
  KC_I: 0x0c,
  KC_J: 0x0d,
  KC_K: 0x0e,
  KC_L: 0x0f,
  KC_M: 0x10,
  KC_N: 0x11,
  KC_O: 0x12,
  KC_P: 0x13,
  KC_Q: 0x14,
  KC_R: 0x15,
  KC_S: 0x16,
  KC_T: 0x17,
  KC_U: 0x18,
  KC_V: 0x19,
  KC_W: 0x1a,
  KC_X: 0x1b,
  KC_Y: 0x1c,
  KC_Z: 0x1d,
  KC_ENTER: 0x28,
  KC_SPACE: 0x2c,
}

class HID
  def initialize(default_keymap)
    @special_keys = Array.new
    @switches = Array.new
    @led = LED.new
    @keymap = {
      default: default_keymap
    }
  end

  def add_keymap(name, map)
    @keymap[name] = map
  end

  def add_special_key(key, param)
    @keymap.each do |layer, map|
      map.each_with_index do |row, row_index|
        row.each_with_index do |key_symbol, col_index|
          if key == key_symbol
            @special_keys << {
              layer:          layer,
              release_key:    param[:release_key],
              hold_keymap:    param[:hold_keymap],
              time_threshold: (param[:time_threshold] || 0),
              switch:         [row_index, col_index],
              prev_state:     :released,
              pushed_at:      0,
              released_at:    0,
            }
            break
          end
        end
      end
    end
  end

  def start
    while true
      now = board_millis
      keycode_pos = 0
      keycodes = "\000\000\000\000\000\000"
      layer = :default

      @switches.clear
      modifier = 0

      # detect physical switches that are pushed
      COLS.each_with_index do |col_pin, col|
        gpio_put(col_pin, LO)
        ROWS.each_with_index do |row_pin, row|
          @switches << [row, col] if gpio_get(row_pin) == LO
        end
        gpio_put(col_pin, HI)
      end

      @special_keys.each do |sk|
        next if sk[:layer] != layer
        if @switches.include?(sk[:switch])
          @led.on
          case sk[:prev_state]
          when :released
            sk[:pushed_at] = now
            sk[:prev_state] = :pushed
            layer = sk[:hold_keymap]
          when :pushed
            layer = sk[:hold_keymap]
          when :pushed_then_released
            if now - sk[:released_at] <= sk[:time_threshold]
              sk[:prev_state] = :pushed_then_released_then_pushed
            end
          when :pushed_then_released_then_pushed
            keycodes[keycode_pos] = KEYCODES[sk[:release_key]].chr
            keycode_pos += 1
            break
          end
        else
          @led.off
          case sk[:prev_state]
          when :pushed
            if now - sk[:pushed_at] <= sk[:time_threshold]
              keycodes[keycode_pos] = KEYCODES[sk[:release_key]].chr
              keycode_pos += 1
              sk[:prev_state] = :pushed_then_released
              sk[:released_at] = now
              break
            else
              sk[:prev_state] = :released
            end
          when :pushed_then_released
            if now - sk[:released_at] > sk[:time_threshold]
              sk[:prev_state] = :released
            end
          when :pushed_then_released_then_pushed
            sk[:prev_state] = :released
          end
        end
      end

      keymap = @keymap[layer]
      @switches.each do |switch|
        key = keymap[switch[0]][switch[1]]
        if KEYCODES[key]
          keycodes[keycode_pos] = KEYCODES[key].chr
          keycode_pos += 1
        elsif MODS[key]
          modifier |= MODS[key]
        end
        break if keycode_pos > 5
      end

      report_hid(modifier, keycodes)
      time = board_millis - now
      sleep_ms(time) if time > 0
    end
  end

end

$mutex.unlock
