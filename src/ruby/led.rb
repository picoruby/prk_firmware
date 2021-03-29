LED_PIN = 25

class LED
  def on
    gpio_put(LED_PIN, 1)
  end

  def off
    gpio_put(LED_PIN, 0)
  end
end
