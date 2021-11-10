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
    when :RGB_SAI, :RGB_SAD
      message = 0b10000000 # 4 << 5
    when :RGB_VAI, :RGB_VAD
      message = 0b10100000 # 5 << 5
    when :RGB_SPI, :RGB_SPD
      message = 0b11000000 # 6 << 5
      @delay = if key == :RGB_SPI
        10 < @delay  ? @delay - 10 : @delay
      else
        @delay < 300 ? @delay + 10 : @delay
      end
      puts "delay: #{@delay}"
      message |= (@delay / 10)
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
    when 6 # SPI, SPD
      @delay = (message & 0b00011111) * 10
    end
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
