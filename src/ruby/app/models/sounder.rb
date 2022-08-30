if RUBY_ENGINE == 'mruby/c'
  require "mml"
end

class Sounder
  SONGS = {
    beepo:    "T140 a32 < a32",
    pobeep:   "T140 O5 a32 > a32",
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

  def add_song(name, *measures)
    SONGS[name] = measures.join
  end

  def play(*measures)
    compile(*measures)
    replay
  end

  def compile(*measures)
    clear_song
    if (name = measures[0]) && name.is_a?(Symbol)
      MML.compile SONGS[name].to_s do |f, d|
        add_note f, d
      end
    else
      measures.each do |measure|
        MML.compile measure.to_s do |f, d|
          add_note f, d
        end
      end
    end
  end

  def replay
    return if @playing
    @playing = true
    sounder_replay
  end
end
