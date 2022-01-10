class KeyboardTest < MrubycTestCase
  # TODO: improve mock functionality of mrubyc-test
  def define_mocks
    kbd = Keyboard.new
    mock(kbd).gpio_init(1)
    mock(kbd).gpio_set_dir(2)
    mock(kbd).gpio_put(2)
    mock(kbd).gpio_pull_up(1)
  end
  def setup
    @kbd = Keyboard.new
    @kbd.init_pins([1,2], [3,4])
  end

  ##### Tests from here

  description "default layer"
  def default_layer
    assert_equal :default, @kbd.layer
  end

  description ":KC_ENT should be equal to :KC_ENTER(0x28)"
  def kc_aliases_1
    assert_equal (0x28)*(-1), @kbd.find_keycode_index(:KC_ENT)
  end

  description "split_style should be :standard_split or :right_side_flipped_split"
  def split_stype_case
    assert_equal :standard_split, @kbd.split_style
    @kbd.split_style = :right_side_flipped_split
    assert_equal :right_side_flipped_split, @kbd.split_style
    @kbd.split_style = :this_is_invalid
    assert_equal :standard_split, @kbd.split_style
  end

  description "should return col_index as it is"
  def standard_calculate_col_position_case
    @kbd.split = true
    @kbd.split_style = :standard_split
    @kbd.add_layer :default, %i(KC_A KC_B KC_C KC_D   KC_E KC_F KC_G KC_H)
    assert_equal 3, @kbd.calculate_col_position(3)
  end

  description "should return calculated col_index"
  def flipped_calculate_col_position_case
    @kbd.split = true
    @kbd.split_style = :right_side_flipped_split
    @kbd.add_layer :default, %i(KC_A KC_B KC_C KC_D   KC_E KC_F KC_G KC_H)
    assert_equal 2, @kbd.calculate_col_position(3)
  end

  description "raise"
  def raise_layer_case
    @kbd.add_layer :default, %i(KC_A)
    @kbd.add_layer :raise,   %i(KC_B)
    @kbd.add_layer :lower,   %i(KC_C)
    @kbd.raise_layer
    assert_equal :raise, @kbd.instance_variable_get("@locked_layer")
    @kbd.raise_layer
    assert_equal :lower, @kbd.instance_variable_get("@locked_layer")
  end

  description "lower"
  def lower_layer
    @kbd.add_layer :default, %i(KC_A)
    @kbd.add_layer :raise,   %i(KC_B)
    @kbd.add_layer :lower,   %i(KC_C)
    @kbd.lower_layer
    assert_equal :lower, @kbd.instance_variable_get("@locked_layer")
    @kbd.lower_layer
    assert_equal :raise, @kbd.instance_variable_get("@locked_layer")
  end

  description "@ruby_mode should switch"
  def ruby_mode
    assert_not_equal true, @kbd.instance_variable_get("@ruby_mode")
    @kbd.ruby
    assert_equal true, @kbd.instance_variable_get("@ruby_mode")
  end

  description "VIA Response"
  def via_get_layer_count
    response = @kbd.raw_hid_receive([17,0,0,0])
    assert_equal 5, response[1]
  end
end
