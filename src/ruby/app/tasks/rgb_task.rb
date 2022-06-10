puts "RGB task started."

3.times { sleep 1 }

while true
  unless $rgb
    sleep 10
  else
    $rgb.show
  end
end
