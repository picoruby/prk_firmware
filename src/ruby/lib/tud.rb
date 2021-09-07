print "\033[2J"   # clear all
print "\033[1;1H" # home
puts "Welcome to PRK Firmware"
puts
puts "Starting tud task ..."

while true
  tud_task
  cdc_task
  if $encoders && !$encoders.empty?
    $encoders.each { |encoder| encoder.read }
  end
end
