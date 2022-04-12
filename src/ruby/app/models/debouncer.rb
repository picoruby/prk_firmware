class Debouncer
  # sym_eager_pk:
  #   - Eager algorithm works on both Key-down event and Key-up event
  #   - One timer is going to be made per KEY
  #   - This consumes large memory which may cause `Out of Memory`
  #
  # This naming convention comes from QMK Firmware's document:
  # https://docs.qmk.fm/#/feature_debounce_type
  DEBOUNCE_TYPES = %i(
    none
    sym_eager_pk
  )

  DEFAULT_THRESHOLD = 40

  def initialize(type = :sym_eager_pk)
    @pk_table = {}
    self.type = type
    self.threshold = DEFAULT_THRESHOLD
    @now = 0
  end

  def type=(val)
    unless DEBOUNCE_TYPES.include?(val)
      puts "Invalid Debouncer type: #{val}"
      @type = :none
    else
      puts "Debouncer type: #{val}"
      @type = val
    end
  end

  def threshold=(val)
    if !val.is_a?(Integer) || val < 1
      puts "Invalid Debouncer threshold: #{val}"
      @threshold = DEFAULT_THRESHOLD
    else
      puts "Debouncer threshold: #{val}"
      @threshold = val
    end
  end

  def set_time
    @now = board_millis
  end

  # When pin_val==true, the GPIO pin stays `on`
  # which means the keyswitch is NOT pushed.
  def resolve(pin_val, row, col)
    return pin_val if @type == :none
    key = row << 8 | col
    status = @pk_table[key]
    unless status
      @pk_table[key] = { pin_val: pin_val, time: @now }
      return pin_val
    end
    if status[:pin_val] != pin_val
      if @now - status[:time] < @threshold
        !pin_val
      else
        @pk_table[key] = { pin_val: pin_val, time: @now }
        pin_val
      end
    else
      pin_val
    end
  end

end
