class Joystick
  # https://raspberrypi.github.io/pico-sdk-doxygen/group__hardware__adc.html
  # > input channel. 0...3 are GPIOs 26...29 respectively
  VALID_PINS = [26, 27, 28, 29]

  def initialize(axes)
    reset_axes
    axes.each do |axis, pin|
      unless VALID_PINS.include?(pin)
        puts "Invaid joystick pin: #{pin}"
      else
        init_axis_offset(axis.to_s, pin - VALID_PINS[0])
      end
    end
  end
end
