class RGB
 def initialize(pin, underglow_size, backlight_size, is_rgbw)
    puts "Initializing RGB ..."
    @fifo = Array.new
    # TODO: @underglow_size, @backlight_size
    @pixel_size = underglow_size + backlight_size
    ws2812_init(pin, @pixel_size, is_rgbw)
    @delay = 100
    @status = :null
  end

  attr_reader :pixel_size
  attr_accessor :delay, :effect, :action, :status

  def invoke(key)
    print "RGB: invoked #{key} / "
    case key
    when nil
      # do nothing
    when :RGB_TOG
    when :RGB_MODE_FORWARD, :RGB_MOD
    when :RGB_MODE_REVERSE, :RGB_RMOD
    when :RGB_HUI
    when :RGB_HUD
    when :RGB_SAI
    when :RGB_SAD
    when :RGB_VAI
    when :RGB_VAD
    when :RGB_SPI
      @delay -= 10 if 20 < @delay
      puts "delay: #{@delay}"
    when :RGB_SPD
      @delay += 10 if @delay < 200
      puts "delay: #{@delay}"
    end
    sleep 0.5 # preventing continuous invoke
  end

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
