class KeyboardTest < MrubycTestCase
  # TODO: improve mock functionality of mrubyc-test
  def define_mocks
    kbd = Keyboard.new
    mock(kbd).gpio_init(1)
    mock(kbd).gpio_set_dir(2)
  end
  def setup
    @kbd = Keyboard.new
    @kbd.init_pins([1,2], [3,4])
  end

  ##### Tests from here

  description "init @direct_pins"
  def direct_pins_case
    @kbd.init_direct_pins([1,2,3,4])
    assert_equal [1,2,3,4], @kbd.instance_variable_get("@direct_pins")
  end

  description "normal matrix of anchor"
  def init_matrix_case
    expectation = {
      1 => { 3 => [0, 0], 4 => [0, 1] },
      2 => { 3 => [1, 0], 4 => [1, 1] }
    }
    assert_equal expectation, @kbd.instance_variable_get("@matrix")
    assert_equal 2, @kbd.instance_variable_get("@rows_size")
    assert_equal 2, @kbd.instance_variable_get("@cols_size")
  end

  description "normal matrix of partner"
  def init_partner_matrix_case
    kbd = Keyboard.new
    stub(kbd).tud_mounted? { false }
    mock(kbd).uart_partner_init(1)
    kbd.split = true
    kbd.init_pins([1,2], [3,4])
    expectation = {
      1 => { 3 => [0, 3], 4 => [0, 2] },
      2 => { 3 => [1, 3], 4 => [1, 2] }
    }
    assert_equal expectation, kbd.instance_variable_get("@matrix")
  end

  description "duplex matrix of partner"
  def init_duplex_partner_matrix_case
    kbd = Keyboard.new
    stub(kbd).tud_mounted? { false }
    mock(kbd).uart_partner_init(1)
    kbd.split = true
    kbd.init_matrix_pins(
      [
        [ [1, 3], [1, 4], [3, 1], [4, 1] ],
        [ [2, 3], [2, 4], [3, 2], [4, 2] ]
      ]
    )
    expectation = {
      1 => { 3 => [0, 7], 4 => [0, 6] },
      3 => { 1 => [0, 5], 2 => [1, 5] },
      4 => { 1 => [0, 4], 2 => [1, 4] },
      2 => { 3 => [1, 7], 4 => [1, 6] }
    }
    assert_equal expectation, kbd.instance_variable_get("@matrix")
  end

  description "duplex matrix of anchor"
  def init_duplex_matrix_case
    @kbd.init_matrix_pins(
      [
        [ [1, 3], [1, 4], [3, 1], [4, 1] ],
        [ [2, 3], [2, 4], [3, 2], [4, 2] ]
      ]
    )
    expectation = {
      1 => { 3 => [0, 0], 4 => [0, 1] },
      3 => { 1 => [0, 2], 2 => [1, 2] },
      4 => { 1 => [0, 3], 2 => [1, 3] },
      2 => { 3 => [1, 0], 4 => [1, 1] }
    }
    assert_equal expectation, @kbd.instance_variable_get("@matrix")
    assert_equal 2, @kbd.instance_variable_get("@rows_size")
    assert_equal 4, @kbd.instance_variable_get("@cols_size")
  end

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
    assert_equal 3, @kbd.calculate_col_position(3, 4)
  end

  description "should return calculated col_index"
  def flipped_calculate_col_position_case
    @kbd.split = true
    @kbd.split_style = :right_side_flipped_split
    @kbd.add_layer :default, %i(KC_A KC_B KC_C KC_D   KC_E KC_F KC_G KC_H)
    assert_equal 2, @kbd.calculate_col_position(3, 4)
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
end
