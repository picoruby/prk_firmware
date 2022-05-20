class ViaTest < MrubycTestCase
  def setup
    @via = VIA.new
    @kbd = Keyboard.new
    @via.kbd = @kbd
  end

  description ":KC_RCTL => 0xE4"
  def translate_RCTL
    kc = @kbd.find_keycode_index(:KC_RCTL)
    assert_equal(0x00E4, @via.prk_keycode_into_via_keycode(kc))
  end

  description "translate to :KC_RPRN"
  def translate_to_shifted_key
    assert_equal(:KC_RPRN, @via.via_keycode_into_keysymbol(0x1227))
  end

  description "expand :RCTL_1"
  def expand_key_with_modifier
    assert_equal(%i[KC_RCTL KC_1], @via.expand_composite_key(:RCTL_1) )
  end
end