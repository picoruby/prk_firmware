class GPIO
  IN         = 0b000001
  OUT        = 0b000010
  HIGH_Z     = 0b000100
  PULL_UP    = 0b001000
  PULL_DOWN  = 0b010000
  OPEN_DRAIN = 0b100000
  def self._init(pin)
  end
  def self.set_dir_at(pin,flag)
  end
  def self.pull_up_at(pin)
  end
  def self.open_drain_at(pin)
  end
end

