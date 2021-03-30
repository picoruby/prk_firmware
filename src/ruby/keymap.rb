# wait until Keyboard class is ready
while !$mutex
  relinquish
end

# initialize a Keyboard with GPIO assign
kbd = Keyboard.new(
  [ 2, 3 ], # row0, row1,... respectively
  [ 6, 7 ]  # col0, col1,... respectively
)

# default layer should be added at first
kbd.add_layer :default, [
  %i(KC_A RAISE_ENTER),
  %i(KC_B LOWER_SPACE),
]
kbd.add_layer :raise, [
  %i(KC_C RAISE_ENTER),
  %i(KC_D ADJUST),
]
kbd.add_layer :lower, [
  %i(KC_E RAISE_ENTER),
  %i(KC_F LOWER_SPACE),
]
kbd.add_layer :adjust, [
  %i(KC_Y RAISE_ENTER),
  %i(KC_Z ADJUST),
]

#                   Your key name   Keycode when clicked   Layer when held   Time threshold(ms)
kbd.define_mode_key :RAISE_ENTER, [ :KC_ENTER,             :raise,           200 ]
kbd.define_mode_key :LOWER_SPACE, [ :KC_SPACE,             :lower,           200 ]
kbd.define_mode_key :ADJUST,      [ nil,                   :adjust,          nil ]

kbd.start
