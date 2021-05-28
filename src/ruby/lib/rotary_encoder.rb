class RotaryEncoder
  def initialize(pin_a, pin_b)
    init_rotary_encoder(pin_a, pin_b)
  end

  def clockwise(&block)
    @proc_cw = block
  end

  def counterclockwise(&block)
    @proc_ccw = block
  end

  def consume_rotation
    rotation = consume_rotation_rotary_encoder
    if rotation > 0 && @proc_cw
      @proc_cw.call
    elsif rotation < 0 && @proc_ccw
      @proc_ccw.call
    end
  end
end
