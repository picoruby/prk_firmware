# Suspend itself until being resumed in Keyboard#start_rgb
suspend_task

hue = 0
value = 0

case $rgb.effect
when :rainbow
  step = 360.0 / $rgb.pixel_size
  $rgb.pixel_size.times do |i|
    $rgb.set_pixel_at(i, hsv2rgb(i * step, 100, 12.5))
  end
when :breathing
  hue = 0
end

while true
  if $rgb.key?
    case $rgb.action
    when :thunder
      $rgb.thunder
    end
  end
  case $rgb.effect
  when :rainbow
    $rgb.rotate
  when :breathing
    $rgb.fill(hsv2rgb(hue, 100, 12.5))
    hue >= 360 ? hue = 0 : hue += 10
  when :ruby
    $rgb.fill(hsv2rgb(0, 100, value))
    value >= 13 ? value = 0 : value += 1
  end
  $rgb.show
end
