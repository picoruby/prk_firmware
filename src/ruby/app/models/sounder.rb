if RUBY_ENGINE == 'mruby/c'
  require "mml"
end

class Sounder
  PRESET_SONGS = {
    beepo:    "T140 a32 < a32",
    pobee:    "T140 O5 a32 > a32",
    beepbeep: "T250 O2 c8 r8 c4",
    oneup:    "T320 L8 O5 e g < e c d g",
  }

  def initialize(pin)
    puts "Init Sounder"
    sounder_init pin
    clear_song
    @playing = false
  end

  attr_accessor :playing

  def play(*measures)
    set_song(*measures)
    replay
  end

  def set_song(*measures)
    clear_song
    measures.each do |measure|
      MML.compile measure do |f, d|
        add_note f, d
      end
    end
  end

  def replay
    return if @playing
    @playing = true
    sounder_replay
  end
end
