while true
  tud_task
  $encoders.each { |encoder| encoder.read } unless $encoders.empty?
end
