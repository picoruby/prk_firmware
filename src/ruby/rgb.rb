class RGB
  def initialize(pin, pixel_size, is_rgbw)
    @fifo = Array.new
    ws2812_init(pin, is_rgbw)
    @pixels = Array.new(pixel_size, 0) # => [0, 0, 0, 0, 0,...
    @max_brightness = 20
  end

  def adjust(val, rr, gg, bb)
    r = ((val & 0x00ff00) >> 8) + rr
    g = (val >> 16)             + gg
    b = (val & 0x0000ff)        + bb
    if r > @max_brightness
      r = @max_brightness
    elsif r < 0
      r = 0
    end
    if g > @max_brightness
      g = @max_brightness
    elsif g < 0
      g = 0
    end
    if b > @max_brightness
      b = @max_brightness
    elsif b < 0
      b = 0
    end
    return r<<8 | g<<16 | b
  end

  def adjust_at(i, rr, gg, bb)
    set_at(i, adjust(@pixels[i], rr, gg, bb))
  end

  def adjust_all(rr, gg, bb)
    @pixels.size.times do |i|
      adjust_at(i, rr, gg, bb)
    end
  end

  def fill(val)
    @pixel.size.times do |i|
      set_at(i, val)
    end
  end

  def set_at(i, val)
    @pixels[i] = val
  end

  def show
    sparkle unless @fifo.empty?
    sleep_ms 30
    ws2812_show(@pixels)
  end

  def fifo_push(data)
    return if @fifo.size > 2
    @fifo << data
  end

  def sparkle
    7.times do |t|
      @pixels.size.times do |i|
        if rand & 0xf > t + 2
          set_at(i, 0x202020)
        else
          set_at(i, 0)
        end
      end
      ws2812_show(@pixels)
      sleep_ms 10
    end
    @fifo.shift
  end

end

# Suspend itself until being resumed in Keyboard#start_rgb
suspend_task

$rgb.fill(0x200000)
$rgb.show

while true
  0x20.times do
    $rgb.adjust_all(-1, 1, 0)
    $rgb.show
  end
  0x20.times do
    $rgb.adjust_all(0, -1, 1)
    $rgb.show
  end
  0x20.times do
    $rgb.adjust_all(1, 0, -1)
    $rgb.show
  end
end

