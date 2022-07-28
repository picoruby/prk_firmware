200.times do
  tud_task
  cdc_task
  sleep_ms 2
end
print "\e[2J\e[1;1H" # clear all & home
puts "==============================================="
puts Object._prk_description
puts "==============================================="

autoreload_tick = 0

$mutex = false

while true
  tud_task
  cdc_task
  if autoreload_ready?
    if autoreload_tick == 0
      puts "\nReloading keymap"
      $rgb.turn_off if $rgb
      Keyboard.suspend_keymap
      report_hid(0, "\000\000\000\000\000\000")
      autoreload_tick = 200
    elsif autoreload_tick == 1
      Keyboard.reload_keymap
      10.times do
        tud_task
        cdc_task
        sleep_ms 2
      end
      Keyboard.resume_keymap
    end
    autoreload_tick -= 1
    tud_task
    cdc_task
    sleep_ms 2
  end
end
