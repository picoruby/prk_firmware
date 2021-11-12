while !$mutex
  relinquish
end


puts "Starting rgb task ..."

while true
  unless $rgb
    sleep 1
  else
    $rgb.show
  end
end
