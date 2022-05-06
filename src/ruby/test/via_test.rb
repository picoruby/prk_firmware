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

  description "via_keycode_into_keysymbol(0x121E) =>:RSFT_1"
  def translate_to_RSFT_1
    assert_equal(:RSFT_1, @via.via_keycode_into_keysymbol(0x121E))
  end

  description "expand :RSFT_1"
  def expand_shifted_key
    assert_equal(%i[KC_RSFT KC_1], @via.expand_composite_key(:RSFT_1) )
  end
end