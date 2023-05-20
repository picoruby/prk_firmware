
class Machine
  def self.board_millis
    1000
  end
end

class MIDI
  attr_accessor :play_status, :previous_clock_time, :duration_per_clock
end

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

  description "sustain on"
  def sustain_on_case
    @midi.send_sustain(1)
    assert_equal [[MIDI::CONTROL_CHANGE_EVENT || 0, 0x7f]], @midi.instance_variable_get("@buffer")
  end

  description "sustain off"
  def sustain_off_case
    @midi.send_sustain(0)
    assert_equal [[MIDI::CONTROL_CHANGE_EVENT || 0, 0]], @midi.instance_variable_get("@buffer")
  end

  description "task_when_chord_mode_on_with_major"
  def convert_chord_pattern_with_major_case
    assert_equal [0, 4, 7], @midi.convert_chord_pattern(0)
  end

  description "toggle_chord_pattern_up_and_down"
  def toggle_chord_pattern_case
    # major -> major7th
    @midi.update_event(MIDI::KEYCODE[:MI_NEXTCRD], :press)
    @midi.process_request(MIDI::KEYCODE[:MI_NEXTCRD], {})
    assert_equal :major7th, @midi.chord_pattern

    # major7th -> major
    @midi.update_event(MIDI::KEYCODE[:MI_PREVCRD], :press)
    @midi.process_request(MIDI::KEYCODE[:MI_PREVCRD], {})
    assert_equal :major, @midi.chord_pattern

    # augmented7th -> major
    @midi.chord_pattern = :augmented7th
    @midi.update_event(MIDI::KEYCODE[:MI_NEXTCRD], :press)
    @midi.process_request(MIDI::KEYCODE[:MI_NEXTCRD], {})
    assert_equal :major, @midi.chord_pattern

    # major -> augmented7th
    @midi.update_event(MIDI::KEYCODE[:MI_PREVCRD], :press)
    @midi.process_request(MIDI::KEYCODE[:MI_PREVCRD], {})
    assert_equal :augmented7th, @midi.chord_pattern
  end

  description "midi realtime clock stop"
  def send_midi_realtime_clock_stop_case
    # @play_status :stop
    @midi.play_status = :stop
    @midi.send_srt_clock
    assert_equal [], @midi.instance_variable_get("@buffer")
  end

  description "midi realtime clock start first time"
  def send_midi_realtime_clock_first_time_case
    @midi.play_status = :start
    @midi.previous_clock_time = nil
    @midi.send_srt_clock
    assert_equal 1000, @midi.previous_clock_time
    assert_equal [[MIDI::SRT_TIMING_CLOCK]], @midi.instance_variable_get("@buffer")
  end

  description "midi realtime clock start 2nd time"
  def send_midi_realtime_clock_2nd_time_case
    @midi.play_status = :start
    @midi.previous_clock_time = 500
    @midi.duration_per_clock = 150 
    @midi.send_srt_clock
    # send midi clock 3 times
    assert_equal [
      [MIDI::SRT_TIMING_CLOCK],
      [MIDI::SRT_TIMING_CLOCK],
      [MIDI::SRT_TIMING_CLOCK]
    ], @midi.instance_variable_get("@buffer")
    assert_equal 950, @midi.previous_clock_time
  end
end
