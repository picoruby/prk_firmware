require "keyboard"

ENV = {}

200.times do
  USB.tud_task
  sleep_ms 2
end
print "\e[2J\e[1;1H" # clear all & home

Keyboard.mount_volume

puts "==============================================="
puts PRK_DESCRIPTION
puts "PRK_NO_MSC: #{PRK_NO_MSC}"
puts "==============================================="

first_time = true

while true
  USB.tud_task
  if Keyboard.autoreload_ready?
    Keyboard.restart
  end
end
