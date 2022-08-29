class Tone
  def initialize(pin)
    puts "Init Tone"
    tone_init pin
  end

  def set_tones(tones)
    tone_set_tones tones
  end

  def start
    tone_start
  end
end