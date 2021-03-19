LED = 25

while true
  gpio_put(LED, 1)
  sleep(1)
  gpio_put(LED, 0)
  sleep(1)
end
