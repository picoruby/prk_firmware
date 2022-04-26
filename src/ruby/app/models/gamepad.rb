class Gamepad
  def initialize(axes)
    init_adc_table
    axes.each do |axis, pin|
      init_joystick(axis.to_s, pin)
    end
  end
end
