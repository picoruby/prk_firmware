class RotaryEncoder
  def initialize(pin_a, pin_b)
    init_encoder(pin_a, pin_b)
    @pin_a = pin_a
    @pin_b = pin_b
    @rotation = 0
  end

  def read
    @rotation += read_encoder(@pin_a, @pin_b)
  end

  def clockwise(&block)
    @proc_cw = block
  end

  def counterclockwise(&block)
    @proc_ccw = block
  end

  def consume_rotation
    # ignore values of 1 and -1
    if @rotation > 1 && @proc_cw
      @proc_cw.call
      sleep_ms 20
    elsif @rotation < -1 && @proc_ccw
      @proc_ccw.call
      # counterclockwise seems more sensitive
      sleep_ms 40
    end
    @rotation = 0
  end
end
