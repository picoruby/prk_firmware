class Debouncer
  # asym_eager_none_pr:
  #   - Eager algorithm works on Key-down event
  #   - One timer is going to be made per ROW
  #   - Any algorithm doesn't work on Key-up event
  # sym_eager_pk:
  #   - Eager algorithm works on both Key-down event and Key-up event
  #   - One timer is going to be made per KEY
  #   - This consumes large memory which may cause `Out of Memory`
  #
  # This naming convention comes from QMK Firmware's document:
  # https://github.com/qmk/qmk_firmware/blob/0524a82a88aef8cc2036e7ab1d603b6c78c54fd9/docs/feature_debounce_type.md
  DEBOUNCE_TYPES = %i(
    none
    asym_eager_none_pr
    sym_eager_pk
  )

  DEFAULT_THRESHOLD = 40

  def initialize(type = :asym_eager_none_pr)
    @table = {}
    self.type = type
    self.threshold = DEFAULT_THRESHOLD
    @now = 0
  end

  def type=(val)
    unless DEBOUNCE_TYPES.include?(val)
      puts "Invalid Debouncer#type: #{val}"
      @type = :none
    else
      puts "Debouncer#type: #{val}"
      @type = val
    end
  end

  def threshold=(val)
    if !val.is_a?(Integer) || val < 1
      puts "Invalid Debouncer#threshold: #{val}"
      @threshold = DEFAULT_THRESHOLD
    else
      puts "Debouncer#threshold: #{val}"
      @threshold = val
    end
  end

  def set_time
    @now = board_millis
  end

  def resolve(pin_val, row, col)
    case @type
    when :sym_eager_pk
      resolve_sym_eager_pk(pin_val, row, col)
    when :asym_eager_none_pr
      resolve_asym_eager_none_pr(pin_val, row, col)
    else
      pin_val
    end
  end

  # When pin_val==true, the GPIO pin stays `on`
  # which means the keyswitch is NOT pushed.
  def resolve_sym_eager_pk(pin_val, row, col)
    key = row << 8 | col
    status = @table[key]
    unless status
      @table[key] = { state: pin_val, time: @now }
      return pin_val
    end
    if status[:state] != pin_val
      if @now - status[:time] < @threshold
        !pin_val
      else
        @table[key] = { state: pin_val, time: @now }
        pin_val
      end
    else
      pin_val
    end
  end

  def resolve_asym_eager_none_pr(pin_val, row, _col)
    status = @table[row]
    unless status
      @table[row] = { state: pin_val, time: @now }
      return pin_val
    end
    if pin_val
      # Unpressed
      unless status[:state] || @now - status[:time] < @threshold
        @table[row] = { state: true, time: @now }
      end
      true
    else
      # Pressed
      if status[:state]
        if @now - status[:time] < @threshold
          puts "down"
          true
        else
          @table[row] = { state: false, time: @now }
          false
        end
      else
        false
      end
    end
  end

end
