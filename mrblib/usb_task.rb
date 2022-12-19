require "keyboard"

200.times do
  USB.tud_task
  sleep_ms 2
end
print "\e[2J\e[1;1H" # clear all & home
puts "==============================================="
puts PRK_DESCRIPTION
puts "==============================================="

autoreload_tick = 0

$mutex = false

while true
  USB.tud_task
  if Keyboard.autoreload_ready?
    if autoreload_tick == 0
      puts "\nReloading keymap"
      $rgb.turn_off if $rgb
      Keyboard.suspend_keymap
      USB.hid_task(0, "\000\000\000\000\000\000", 0, 0, 0)
      autoreload_tick = 200
    elsif autoreload_tick == 1
      Keyboard.reload_keymap
      10.times do
        USB.tud_task
        sleep_ms 2
      end
      Keyboard.resume_keymap
    end
    autoreload_tick -= 1
    USB.tud_task
    sleep_ms 2
  end
end
