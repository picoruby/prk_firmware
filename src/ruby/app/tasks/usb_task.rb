4000.times do
  tud_task
  cdc_task
  sleep_ms 1
end
print "\e[2J\e[1;1H" # clear all & home
puts "Welcome to PRK Firmware!\n\nTUD task started.\n"
autoreload_tick = 0

while true
  tud_task
  cdc_task
  if $encoders && !$encoders.empty?
    $encoders.each { |encoder| encoder.read }
  end
  if autoreload_ready?
    $mutex = false
    if autoreload_tick == 0
      puts "Autoreload is ready."
      puts "Suspending keymap."
      suspend_keymap
      $rgb.effect = :off if $rgb
      $encoders = Array.new
      autoreload_tick = 1000
    elsif autoreload_tick == 1
      puts "Trying to reload keymap."
      reload_keymap
      3000.times do
        tud_task
        cdc_task
        sleep_ms 1
      end
      resume_keymap
    end
    autoreload_tick -= 1
    sleep_ms 2
  end
end
