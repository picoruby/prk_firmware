# wait until HID class is ready
while !$mutex
  relinquish()
end

# GPIO assign
COLS = [ 6, 7 ] # col0, col1,... respectively
ROWS = [ 2, 3 ] # row0, row1,... respectively

# initialize HID with default keymap
hid = HID.new [
  %i(KC_A RAISE_ENTER),
  %i(KC_B LOWER_SPACE),
]

hid.add_keymap :raise, [
  %i(KC_C RAISE_ENTER),
  %i(KC_D ADJUST),
]
hid.add_keymap :lower, [
  %i(KC_E RAISE_ENTER),
  %i(KC_F LOWER_SPACE),
]
hid.add_keymap :adjust, [
  %i(KC_Y RAISE_ENTER),
  %i(KC_Z ADJUST),
]

hid.add_special_key :RAISE_ENTER, {
  release_key:    :KC_ENTER,
  hold_keymap:    :raise,
  time_threshold: 200,
}
hid.add_special_key :LOWER_SPACE, {
  release_key:    :KC_SPACE,
  hold_keymap:    :lower,
  time_threshold: 200,
}
hid.add_special_key :ADJUST, {
  hold_keymap:    :adjust
}

hid.start
