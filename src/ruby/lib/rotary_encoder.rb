class RotaryEncoder
  def initialize(pin_a, pin_b)
    init_encoder(pin_a, pin_b)
  end

  def clockwise(&block)
    @proc_cw = block
  end

  def counterclockwise(&block)
    @proc_ccw = block
  end

  def consume_rotation
    rotation = encoder_value
    if rotation != 0
      if rotation > 0 && @proc_cw
        @proc_cw.call
        sleep_ms 30
      elsif @proc_ccw
        @proc_ccw.call
        sleep_ms 70
      end
      reset_encoder
    end
  end
end
