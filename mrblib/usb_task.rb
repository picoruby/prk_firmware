require "keyboard"

ENV = {}

200.times do
  USB.tud_task
  sleep_ms 2
end

print "\e[2J\e[1;1H" # clear all & home

Keyboard.mount_volume

require "i2c"
i2c = I2C.new({
  unit: :RP2040_I2C0,
  frequency: 100 * 1000,
  sda_pin: 16,
  scl_pin: 17
})
i2c.write(0x0a, 0x91)

while true
  USB.tud_task
  if Keyboard.autoreload_ready?
    Keyboard.restart
  end
  i2c.poll(0x0a)
end
