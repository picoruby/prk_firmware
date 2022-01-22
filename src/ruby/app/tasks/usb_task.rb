2000.times do
  tud_task
  cdc_task
  sleep_ms 2
end
print "\e[2J\e[1;1H" # clear all & home
puts "Welcome to PRK Firmware!\n\nTUD task started.\n"
autoreload_tick = 0

$mutex = false

while true
  tud_task
  cdc_task
  if autoreload_ready?
    if autoreload_tick == 0
      puts "Autoreload is ready."
      puts "Suspending keymap."
      suspend_keymap
      $rgb.turn_off if $rgb
      autoreload_tick = 500
    elsif autoreload_tick == 1
      puts "Trying to reload keymap."
      reload_keymap
      500.times do
        tud_task
        cdc_task
        sleep_ms 2
      end
      resume_keymap
    end
    autoreload_tick -= 1
    sleep_ms 2
  end
end
