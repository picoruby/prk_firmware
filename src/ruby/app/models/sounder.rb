class Sounder
  def initialize(pin)
    puts "Init Tone"
    sounder_init pin
  end

  def set_tones(tones)
    sounder_set_tones tones
  end

  def start
    sounder_start
  end
end
