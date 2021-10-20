while !$mutex
  relinquish
end

hue = 0
value = 0

puts "Waiting rgb task ..."

while true
  unless $rgb
    sleep 1
  else
    case $rgb.status
    when :null
      $rgb.status = :initializing
      puts "Starting rgb task ..."
    when :initializing
      case $rgb.effect
      when :swirl
        step = 360.0 / $rgb.pixel_size
        $rgb.pixel_size.times do |i|
          $rgb.set_pixel_at(i, hsv2rgb(i * step, 100, 12.5))
        end
      when :rainbow_mood
        hue = 0
      when :rainbow
        puts "[WARN] :rainbow is deprecated. Use :swirl instead"
      when :breatheng
        puts "[WARN] :breathing is deprecated. Use :rainbow_mood instead"
      end
      $rgb.status = :initialized
    when :initialized
      if $rgb.key?
        case $rgb.action
        when :thunder
          $rgb.thunder
        end
      end
      case $rgb.effect
      when :swirl
        $rgb.rotate
      when :rainbow_mood
        $rgb.fill(hsv2rgb(hue, 100, 12.5))
        hue >= 360 ? hue = 0 : hue += 10
      when :ruby
        $rgb.fill(hsv2rgb(0, 100, value))
        value >= 13 ? value = 0 : value += 1
      end
      $rgb.show
    end
  end
end
