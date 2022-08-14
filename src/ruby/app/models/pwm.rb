class PWM
  def initialize(pin, hz, remain_ms = 100)
    puts "Init PWM"
    pwm_init(pin, hz, remain_ms)
  end
end