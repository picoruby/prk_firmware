class RGB
  def initialize(pwm_pin)
    @fifo = Array.new
    ws2812_init(pwm_pin)
  end

  def fifo_push_blocking(data)
    return if @fifo.size > 3
    @fifo << data
  end

  def flash
    if @fifo.empty?
      ws2812_off
    else
      rand % 2 == 0 ? snake : sparkle
      @fifo.shift
    end
  end

  def sparkle
    75.times do
      if rand % 16 > 0
        ws2812_put_pixel(32, 32, 32);
      else
        ws2812_put_pixel(0);
      end
    end
  end

  def snake
    75.times do |i|
      x = i % 32
      if x < 5
        ws2812_put_pixel(32, 0, 0)
      elsif x >= 8 && x < 12
        ws2812_put_pixel(0, 32, 0)
      elsif x >= 15 && x < 20
        ws2812_put_pixel(0, 0, 32)
      else
        ws2812_put_pixel(0, 0, 0)
      end
    end
  end
end

# Suspend itself until being resumed in Keyboard#start_rgb
suspend_task

while true
  $rgb.flash
  sleep_ms 50
end
