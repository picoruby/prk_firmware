# wait until Keyboard class is ready
while !$mutex
  relinquish
end

LED_PIN = 25

class LED
  def initialize
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
  end

  def on
    gpio_put(LED_PIN, HI)
  end

  def off
    gpio_put(LED_PIN, LO)
  end
end
