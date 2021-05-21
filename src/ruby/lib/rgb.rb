def hsv2rgb(h, s, v)
  s /= 100.0
  v /= 100.0
  c = v * s
  x = c * (1 - ((h / 60.0).modulo(2) - 1).abs)
  m = v - c
  rgb = if h < 60
          [c, x, 0]
        elsif h < 120
          [x, c, 0]
        elsif h < 180
          [0, c, x]
        elsif h < 240
          [0, x, c]
        elsif h < 300
          [x, 0, c]
        else
          [c, 0, x]
        end
  ((rgb[0] + m) * 255).ceil_to_i << 16 |
    ((rgb[1] + m) * 255).ceil_to_i << 8 |
    ((rgb[2] + m) * 255).ceil_to_i
end

class RGB
  def initialize(pin, underglow_size, backlight_size, is_rgbw)
    @fifo = Array.new
    # TODO: @underglow_size, @backlight_size
    @pixel_size = underglow_size + backlight_size
    ws2812_init(pin, @pixel_size, is_rgbw)
    @delay = 100
  end

  attr_reader :pixel_size
  attr_accessor :delay, :effect, :action

  def fill(val)
    ws2812_fill(val)
  end

  def set_pixel_at(i, val)
    ws2812_set_pixel_at(i, val)
  end

  def rotate
    ws2812_rotate
  end

  def save
    ws2812_save
  end

  def restore
    ws2812_restore
  end

  def show
    sleep_ms @delay
    ws2812_show
  end

  def fifo_push(data)
    return if @fifo.size > 2
    @fifo << data
  end

  def thunder
    save
    4.times do |salt|
      ws2812_rand_fill(0x202020, (salt+1) * 2)
      ws2812_show
      sleep_ms 3
    end
    @fifo.shift
    restore
  end

  def key?
    !@fifo.empty?
  end

end

# Suspend itself until being resumed in Keyboard#start_rgb
suspend_task

hue = 0

case $rgb.effect
when :rainbow
  step = 360.0 / $rgb.pixel_size
  $rgb.pixel_size.times do |i|
    $rgb.set_pixel_at(i, hsv2rgb(i * step, 100, 12.5))
  end
when :breathing
  hue = 0
end

while true
  if $rgb.key?
    case $rgb.action
    when :thunder
      $rgb.thunder
    end
  end
  case $rgb.effect
  when :rainbow
    $rgb.rotate
  when :breathing
    $rgb.fill(hsv2rgb(hue, 100, 12.5))
    hue >= 360 ? hue = 0 : hue += 10
  end
  $rgb.show
end

