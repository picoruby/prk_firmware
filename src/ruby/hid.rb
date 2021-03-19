COLS = [
  6, #col0
  7
]
ROWS = [
  2, #row0
  3
]

HI  = 1
LOW = 0

KEYCODES = {
  KC_A: "\x04",
  KC_B: "\x05",
  KC_C: "\x06",
  KC_D: "\x07",
}

DEFAULT = [
  %i(KC_A RAISE),
  %i(KC_C KC_RSFT),
]

RAISE = [
  %i(KC_B RAISE),
  %i(KC_D KC_RSFT),
]

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

# use to avoid send multiple consecutive zero report for keyboard
has_key = false

raise_tapped_at = nil

keymap = DEFAULT

while true
  now = board_millis

  switches = Array.new
  COLS.each_with_index do |col_pin, col|
    gpio_put(col_pin, LOW)
    ROWS.each_with_index do |row_pin, row|
      switches << keymap[row][col] if gpio_get(row_pin) == LOW
    end
    gpio_put(col_pin, HI)
  end

  if switches.include?(:RAISE)
    if raise_tapped_at #&& ()
      keymap = RAISE
    else
      raise_tapped_at = now
      keymap = DEFAULT
    end
  else
    if now - raise_tapped_at < 200
      report_hid(0, "\050\000\000\000\000\000")
    end
    raise_tapped_at = nil
    keymap = DEFAULT
  end

  keycodes = "\000\000\000\000\000\000"
  pos = 0
  switches.each do |keysymbol|
    if KEYCODES[keysymbol]
      keycodes[pos] = KEYCODES[keysymbol]
      pos += 1
    end
    break if pos > 5
  end

  modifier = 0
  MODS.keys.each do |mod_key|
    modifier |= MODS[mod_key] if switches.include?(mod_key)
  end

#  if (keycodes[0] != "\000" || modifier > 0)
    report_hid(modifier, keycodes)
#    has_key = true
#  elsif has_key
#    report_hid(modifier, 0)
#    has_key = false
#  end

  sleep_ms(8)
end
