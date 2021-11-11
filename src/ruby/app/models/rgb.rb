class RGB
  MAX_VALUE = 15.5

 def initialize(pin, underglow_size, backlight_size, is_rgbw)
    puts "Initializing RGB ..."
    @fifo = Array.new
    # TODO: @underglow_size, @backlight_size
    @pixel_size = underglow_size + backlight_size
    ws2812_init(pin, @pixel_size, is_rgbw)
    @delay = 100
    @hue = 100
    @value = 0.0
    @max_value = 13.0 # default
  end

  attr_reader :effect, :delay, :pixel_size
  attr_accessor :action

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

  def effect=(name)
    @effect = name
    reset_pixel
  end

  def reset_pixel
    case @effect
    when :off
      off = hsv2rgb(0, 0, 0)
      @pixel_size.times do |i|
        ws2812_set_pixel_at(i, off)
      end
    when :swirl
      step = 360.0 / @pixel_size
      @pixel_size.times do |i|
        ws2812_set_pixel_at(i, hsv2rgb(i * step, 100, @max_value))
      end
    when :rainbow_mood
      @hue = 0
    when :rainbow
      puts "[WARN] :rainbow is deprecated. Use :swirl instead"
    when :breatheng
      puts "[WARN] :breathing is deprecated. Use :rainbow_mood instead"
    end
  end

  def show
    unless @fifo.empty?
      case @action
      when :thunder
        thunder
      end
    end
    case @effect
    when :swirl
      ws2812_rotate
    when :rainbow_mood
      ws2812_fill(hsv2rgb(@hue, 100, @max_value))
      @hue >= 360 ? @hue = 0 : @hue += 10
    when :ruby
      ws2812_fill(hsv2rgb(0, 100, @value))
      @value >= @max_value ? @value = 0.0 : @value += 0.5
    end
    ws2812_show
  end

  def invoke_anchor(key)
    print "RGB: invoked #{key} / "
    message = 0
    case key
    when nil
      return 0 # do nothing
    when :RGB_TOG
      message = 0b00100000 # 1 << 5
    when :RGB_MODE_FORWARD, :RGB_MOD, :RGB_MODE_REVERSE, :RGB_RMOD
      message = 0b01000000 # 2 << 5
    when :RGB_HUI, :RGB_HUD
      message = 0b01100000 # 3 << 5
      puts
    when :RGB_SAI, :RGB_SAD
      message = 0b10000000 # 4 << 5
      puts
    when :RGB_VAI, :RGB_VAD
      message = 0b10100000 # 5 << 5
      if key == :RGB_VAI
        @max_value += 0.5 if @max_value < MAX_VALUE
      else
        @max_value -= 0.5 if 0 < @max_value
      end
      puts "max_value: #{@max_value}"
      message |= (@max_value * 2).to_i # max 31
      reset_pixel
    when :RGB_SPI, :RGB_SPD
      message = 0b11000000 # 6 << 5
      if key == :RGB_SPI
        @delay = @delay - 10 if 10 < @delay
      else
        @delay = @delay + 10 if @delay < 300
      end
      puts "delay: #{@delay}"
      message |= (@delay / 10)
    else
      puts "unknown method"
    end
    sleep 0.2 # preventing continuous invoke
    return message
  end
  #            ...      method
  # message: 0b11111111
  #               ^^^^^ value
  def invoke_partner(message)
    case message >> 5
    when 1
    when 2
    when 3
    when 4
    when 5
      @max_value = (message & 0b00011111) / 2.0
      reset_pixel
    when 6 # SPI, SPD
      @delay = (message & 0b00011111) * 10
    end
  end

  def fifo_push(data)
    return if @fifo.size > 2
    @fifo << data
  end

  def thunder
    unless @srand
      # generate seed with board_millis
      srand(board_millis)
      @srand = true
    end
    ws2812_save
    4.times do |salt|
      ws2812_rand_fill(0x202020, (salt+1) * 2)
      ws2812_show
      sleep_ms 3
    end
    @fifo.shift
    ws2812_restore
  end

end
