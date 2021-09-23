class KeyboardTest < MrubycTestCase
  description "default layer"
  def default_layer
    kbd = Keyboard.new
    assert_equal :default, kbd.layer
  end
end
