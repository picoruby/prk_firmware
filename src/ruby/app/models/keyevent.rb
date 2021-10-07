class Keyboard

  # param[0] :on_release
  # param[1] :on_hold
  # param[2] :release_threshold
  # param[3] :repush_threshold
  def define_mode_key(key_name, param)
    on_release = param[0]
    on_hold = param[1]
    release_threshold = param[2]
    repush_threshold = param[3]
    @keymaps.each do |layer, map|
      map.each_with_index do |row, row_index|
        row.each_with_index do |key_symbol, col_index|
          if key_name == key_symbol
            # @type var on_release_action: Integer | Array[Integer] | Proc | nil
            on_release_action = case on_release.class
                                when Symbol
                                  # @type var on_release: Symbol
                                  key = KC_ALIASES[on_release] ? KC_ALIASES[on_release] : on_release
                                  keycode_index = KEYCODE.index(key)
                                  if keycode_index
                                    keycode_index * -1
                                  elsif KEYCODE_SFT[key]
                                    (KEYCODE_SFT[key] + 0x100) * -1
                                  end
                                when Array
                                  # @type var on_release: Array[Symbol]
                                  # @type var ary: Array[Integer]
                                  ary = Array.new
                                  on_release.each do |sym|
                                    keycode_index = KEYCODE.index(sym)
                                    ary << if keycode_index
                                             keycode_index * -1
                                           elsif KEYCODE_SFT[sym]
                                             (KEYCODE_SFT[sym] + 0x100) * -1
                                           else # Should be a modifier
                                             MOD_KEYCODE[sym]
                                           end
                                  end
                                  ary
                                when Proc
                                  # @type var on_release: Proc
                                  on_release
                                end
            on_hold_action = if on_hold.is_a?(Symbol)
                               # @type var on_hold: Symbol
                               MOD_KEYCODE[on_hold] ? MOD_KEYCODE[on_hold] : on_hold
                             else
                               # @type var on_hold: Proc
                               on_hold
                             end
            @mode_keys << {
              layer:             layer,
              on_release:        on_release_action,
              on_hold:           on_hold_action,
              release_threshold: (release_threshold || 0),
              repush_threshold:  (repush_threshold || 0),
              switch:            [row_index, col_index],
              prev_state:        :released,
              pushed_at:         0,
              released_at:       0,
            }
          end
        end
      end
    end
  end

  def action_on_release(mode_key)
    case mode_key.class
    when Integer
      # @type var mode_key: Integer
      if mode_key < -255
        @keycodes << ((mode_key + 0x100) * -1).chr
        @modifier |= 0b00000010
      else
        @keycodes << (mode_key * -1).chr
      end
    when Array
      0 # `steep check` will fail if you remove this line 🤔
      # @type var mode_key: Array[Integer]
      mode_key.each do |key|
        if key < -255
          @keycodes << ((key + 0x100) * -1).chr
          @modifier |= 0b00000010
        elsif key < 0
          @keycodes << (key * -1).chr
        else # Should be a modifier
          @modifier |= key
        end
      end
    when Proc
      # @type var mode_key: Proc
      mode_key.call
    end
  end

  def action_on_hold(mode_key)
    case mode_key.class
    when Integer
      # @type var mode_key: Integer
      @modifier |= mode_key
    when Symbol
      # @type var mode_key: Symbol
      @layer = mode_key
    when Proc
      # @type var mode_key: Proc
      mode_key.call
    end
  end

  def send_key(symbol)
    keycode = KEYCODE.index(symbol)
    if keycode
      modifier = 0
      c = keycode.chr
    else
      keycode = KEYCODE_SFT[symbol]
      if keycode
        modifier = 0b00100000
        c = keycode.chr
      else
        return
      end
    end
    report_hid(modifier, "#{c}\000\000\000\000\000")
    sleep_ms 5
    report_hid(0, "\000\000\000\000\000\000")
  end

end
