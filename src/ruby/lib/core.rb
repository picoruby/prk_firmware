class Float
  def modulo(right)
    left = self.to_f
    while left > right
      left -= right
    end
    left
  end
  def ceil_to_i
    n = self.to_i
    (self > n) ? (n + 1) : n
  end
end

class Object
  def hsv2rgb(h, s, v)
    s /= 100.0
    v /= 100.0
    c = v * s
    x = c * (1 - ((h / 60.0).modulo(2) - 1).abs)
    m = v - c
    rgb = if h < 60
            [c, x, 0]
          elsif h < 120
            [x, c, 0]
          elsif h < 180
            [0, c, x]
          elsif h < 240
            [0, x, c]
          elsif h < 300
            [x, 0, c]
          else
            [c, 0, x]
          end
    ((rgb[0] + m) * 255).ceil_to_i << 16 |
      ((rgb[1] + m) * 255).ceil_to_i << 8 |
      ((rgb[2] + m) * 255).ceil_to_i
  end
end
