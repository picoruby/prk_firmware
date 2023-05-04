class MidiTest < MrubycTestCase
  def setup
    @midi = MIDI.new
  end

  ##### Tests from here

  description "note_on note_num, with velocity"
  def note_on_with_nil_velocity_case
    @midi.note_on(0, nil)
    assert_equal [[MIDI::NOTE_ON_EVENT || 0, 0, 76]], @midi.instance_variable_get("@buffer")
  end

  description "note_off note_num, with velocity"
  def note_off_with_nil_velocity_case
    @midi.note_off(0, nil)
    assert_equal [[MIDI::NOTE_OFF_EVENT || 0, 0, 76]], @midi.instance_variable_get("@buffer")
  end

  description "task_when_chord_mode_on_with_major"
  def convert_chord_pattern_with_major
    assert_equal [0, 4, 7], @midi.convert_chord_pattern(0)
  end

  def convert_toggle_chord_pattern
    # major -> major7th
    @midi.update_event(MIDI::KEYCODE[:MI_CRDNPTN], :press)
    @midi.process_request(MIDI::KEYCODE[:MI_CRDNPTN], {})
    assert_equal :major7th, @midi.chord_pattern

    # augmented7th -> major
    @midi.chord_pattern = :augmented7th
    @midi.update_event(MIDI::KEYCODE[:MI_CRDNPTN], :press)
    @midi.process_request(MIDI::KEYCODE[:MI_CRDNPTN], {})
    assert_equal :major, @midi.chord_pattern
  end
end
