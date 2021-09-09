print "\033[2J"   # clear all
print "\033[1;1H" # home
puts "Welcome to PRK Firmware"
puts
puts "Starting tud task ..."

autoreload_tick = 0

while true
  tud_task
  cdc_task
  if $encoders && !$encoders.empty?
    $encoders.each { |encoder| encoder.read }
  end
  if autoreload_ready?
    if autoreload_tick == 0
      puts "Autoreload is ready ..."
      autoreload_tick = 500
    elsif autoreload_tick == 1
      puts "Suspending keymap ..."
      suspend_keymap
      sleep_ms 100
      puts "Reloading keymap ..."
      reload_keymap
      puts "Reloaded keymap"
    end
    autoreload_tick -= 1
  end
end
