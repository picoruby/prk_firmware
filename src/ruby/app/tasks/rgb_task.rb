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
      when :rainbow, :breathing
        hue = 0
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
      when :rainbow
        step = 360 / $rgb.pixel_size
        $rgb.pixel_size.times do |i|
          $rgb.set_pixel_at(i, hsv2rgb((hue + (i * step))%360, 100, 12.5))
        end
        hue >= 360 ? hue = 0 : hue += 5
      when :breathing
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
