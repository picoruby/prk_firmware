class Keyboard

  STANDARD_SPLIT = :standard_split
  RIGHT_SIDE_FLIPPED_SPLIT = :right_side_flipped_split

  def initialize
    puts "Initializing Keyboard ..."
    # mruby/c VM doesn't work with a CONSTANT to make another CONSTANT
    # steep doesn't allow dynamic assignment of CONSTANT
    @SHIFT_LETTER_THRESHOLD_A    = LETTER.index('A').to_i
    @SHIFT_LETTER_OFFSET_A       = @SHIFT_LETTER_THRESHOLD_A - KEYCODE.index(:KC_A).to_i
    @SHIFT_LETTER_THRESHOLD_UNDS = LETTER.index('_').to_i
    @SHIFT_LETTER_OFFSET_UNDS    = @SHIFT_LETTER_THRESHOLD_UNDS - KEYCODE_SFT[:KC_UNDS]
    @before_filters = Array.new
    @keymaps = Hash.new
    @mode_keys = Array.new
    @switches = Array.new
    @layer_names = Array.new
    @layer = :default
    @split = false
    @split_style = STANDARD_SPLIT
    @anchor = true
    @anchor_left = true # so-called "master left"
    @uart_pin = 1
    $rgb = nil
    $encoders = Array.new
    @partner_encoders = Array.new
    @macro_keycodes = Array.new
    @buffer = Buffer.new("picoirb")
  end

  attr_accessor :split, :uart_pin
  attr_reader :layer, :split_style

  # TODO: OLED, SDCard
  def append(feature)
    case feature.class
    when RGB
      # @type var feature: RGB
      $rgb = feature
    when RotaryEncoder
      # @type var feature: RotaryEncoder
      if @split
        feature.create_keycodes(@partner_encoders.size)
        if @anchor_left
          if @anchor == feature.left? # XNOR
            feature.init_pins
            $encoders << feature
          end
        else
          if @anchor != feature.left? #XOR
            feature.init_pins
            $encoders << feature
          end
        end
        if @anchor && (@anchor_left != feature.left?)
          @partner_encoders << feature
        end
      else
        feature.init_pins
        $encoders << feature
      end
    end
  end

  # val should be treated as `:left` if it's anything other than `:right`
  def set_anchor(val)
    @anchor_left = false if val == :right
  end

  def split_style=(style)
    case style
    when STANDARD_SPLIT, RIGHT_SIDE_FLIPPED_SPLIT
      @split_style = style
    else
      # NOTE: fall back
      @split_style = STANDARD_SPLIT
    end
  end

  def init_pins(rows, cols)
    if @split
      sleep 2 # Wait until USB ready
      @anchor = tud_mounted?
      if @anchor
        uart_rx_init(@uart_pin)
      else
        uart_tx_init(@uart_pin)
      end
    end
    @rows = rows
    @cols = cols
    @rows.each do |pin|
      gpio_init(pin)
      gpio_set_dir(pin, GPIO_OUT);
      gpio_put(pin, HI);
    end
    @cols.each do |pin|
      gpio_init(pin)
      gpio_set_dir(pin, GPIO_IN);
      gpio_pull_up(pin);
    end
    # for split type
    @offset_a = (@cols.size / 2.0).ceil_to_i
    @offset_b = @cols.size * 2 - @offset_a - 1
  end

  # Input
  #   name:    default, map: [ [ :KC_A, :KC_B, :KC_LCTL,   :MACRO_1 ],... ]
  # ↓
  # Result
  #   layer: { default:      [ [ -0x04, -0x05, 0b00000001, :MACRO_1 ],... ] }
  def add_layer(name, map)
    new_map = Array.new(@rows.size)
    row_index = 0
    col_index = 0
    @entire_cols_size = @split ? @cols.size * 2 : @cols.size
    map.each do |key|
      new_map[row_index] = Array.new(@cols.size) if col_index == 0
      col_position = calculate_col_position(col_index)
      new_map[row_index][col_position] = find_keycode_index(key)
      if col_index == @entire_cols_size - 1
        col_index = 0
        row_index += 1
      else
        col_index += 1
      end
    end
    @keymaps[name] = new_map
    @layer_names << name
  end

  def calculate_col_position(col_index)
    return col_index unless @split

    case @split_style
    when STANDARD_SPLIT
      col_index
    when RIGHT_SIDE_FLIPPED_SPLIT
      if col_index < @cols.size
        col_index
      else
        @entire_cols_size - (col_index - @cols.size) - 1
      end
    end
  end

  def find_keycode_index(key)
    key = KC_ALIASES[key] ? KC_ALIASES[key] : key
    keycode_index = KEYCODE.index(key)

    if keycode_index
      keycode_index * -1
    elsif KEYCODE_SFT[key]
      (KEYCODE_SFT[key] + 0x100) * -1
    elsif MOD_KEYCODE[key]
      MOD_KEYCODE[key]
    else
      key
    end
  end

  # MOD_KEYCODE = {
  #        KC_LCTL: 0b00000001,
  #        KC_LSFT: 0b00000010,
  #        KC_LALT: 0b00000100,
  #        KC_LGUI: 0b00001000,
  #        KC_RCTL: 0b00010000,
  #        KC_RSFT: 0b00100000,
  #        KC_RALT: 0b01000000,
  #        KC_RGUI: 0b10000000
  # }
  def invert_sft
    if (@modifier & 0b00100010) > 0
       @modifier &= 0b11011101
    else
       @modifier |= 0b00000010
    end
  end

  def before_report(&block)
    @before_filters << block
  end

  def keys_include?(key)
    keycode = KEYCODE.index(key)
    !keycode.nil? && @keycodes.include?(keycode.chr)
  end

  # **************************************************************
  #  For those who are willing to contribute to PRK Firmware:
  #
  #   The author has intentionally made this method big and
  #   redundant for resilience against a change of spec.
  #   Please refrain from "refactoring" for a while.
  # **************************************************************
  def start!
    puts "Starting keyboard task ..."

    @keycodes = Array.new
    # To avoid unintentional report on startup
    # which happens only on Sparkfun Pro Micro RP2040
    if @split
      sleep_ms 100
      while true
        data = uart_getc
        break if data.nil?
      end
    end
    default_sleep = 10
    while true
      now = board_millis
      @keycodes.clear

      @switches.clear
      @modifier = 0

      # detect physical switches that are pushed
      @rows.each_with_index do |row_pin, row|
        gpio_put(row_pin, LO)
        @cols.each_with_index do |col_pin, col|
          if gpio_get(col_pin) == LO
            col_data = if @anchor_left
                         if @anchor
                           # left
                           col
                         else
                           # right
                           (col - @offset_a) * -1 + @offset_b
                         end
                       else # right side is the anchor
                         unless @anchor
                           # left
                           col
                         else
                           # right
                           (col - @offset_a) * -1 + @offset_b
                         end
                       end
            @switches << [row, col_data]
          end
          # @type break: nil
          break if @switches.size >= @cols.size
        end
        gpio_put(row_pin, HI)
      end

      # TODO: more features
      $rgb.fifo_push(true) if $rgb && !@switches.empty?

      # Receive switches from partner
      if @split && @anchor
        sleep_ms 5
        while true
          data = uart_getc
          break unless data
          # @type var data: Integer
          if data > 246
            @partner_encoders.each { |encoder| encoder.call_proc_if(data) }
          else
            switch = [data >> 5, data & 0b00011111]
            # To avoid chattering
            @switches << switch unless @switches.include?(switch)
          end
        end
      end

      if @anchor
        desired_layer = @layer
        @mode_keys.each do |mode_key|
          next if mode_key[:layer] != @layer
          if @switches.include?(mode_key[:switch])
            case mode_key[:prev_state]
            when :released
              mode_key[:pushed_at] = now
              mode_key[:prev_state] = :pushed
              on_hold = mode_key[:on_hold]
              if on_hold.is_a?(Symbol) &&
                  @layer_names.index(desired_layer).to_i < @layer_names.index(on_hold).to_i
                desired_layer = on_hold
              end
            when :pushed
              if !mode_key[:on_hold].is_a?(Symbol) && (now - mode_key[:pushed_at] > mode_key[:release_threshold])
                action_on_hold(mode_key[:on_hold])
              end
            when :pushed_then_released
              if now - mode_key[:released_at] <= mode_key[:repush_threshold]
                mode_key[:prev_state] = :pushed_then_released_then_pushed
              end
            when :pushed_then_released_then_pushed
              action_on_release(mode_key[:on_release])
            end
          else
            case mode_key[:prev_state]
            when :pushed
              if now - mode_key[:pushed_at] <= mode_key[:release_threshold]
                action_on_release(mode_key[:on_release])
                mode_key[:prev_state] = :pushed_then_released
              else
                mode_key[:prev_state] = :released
              end
              mode_key[:released_at] = now
              @layer = @prev_layer || :default
              @prev_layer = nil
            when :pushed_then_released
              if now - mode_key[:released_at] > mode_key[:release_threshold]
                mode_key[:prev_state] = :released
              end
            when :pushed_then_released_then_pushed
              mode_key[:prev_state] = :released
            end
          end
        end

        if @layer != desired_layer
          @prev_layer ||= @layer
          action_on_hold(desired_layer)
        end

        keymap = @keymaps[@locked_layer || @layer]
        @switches.each do |switch|
          keycode = keymap[switch[0]][switch[1]]
          next unless keycode.is_a?(Integer)
          if keycode < -255 # Key with SHIFT
            @keycodes << ((keycode + 0x100) * -1).chr
            @modifier |= 0b00100000
          elsif keycode < 0 # Normal keys
            @keycodes << (keycode * -1).chr
          else # Modifier keys
            @modifier |= keycode
          end
        end

        # Macro
        macro_keycode = @macro_keycodes.shift
        if macro_keycode
          if macro_keycode < 0
            @modifier |= 0b00100000
            @keycodes << (macro_keycode * -1).chr
          else
            @keycodes << macro_keycode.chr
          end
          default_sleep = 40 # To avoid accidental skip
        else
          default_sleep = 10
        end

        (6 - @keycodes.size).times do
          @keycodes << "\000"
        end

        @before_filters.each do |block|
          block.call
        end

        $encoders.each do |encoder|
          encoder.consume_rotation_anchor
        end

        if @ruby_mode
          code = @keycodes[0].ord
          c = nil
          if @ruby_mode_stop
            @ruby_mode_stop = false if code == 0
          elsif code > 0
            if @modifier & 0b00100010 > 1 # with SHIFT
              if code <= KEYCODE_SFT[:KC_RPRN].to_i
                c = LETTER[code + @SHIFT_LETTER_OFFSET_A]
              elsif code <= KEYCODE_SFT[:KC_QUES].to_i
                c = LETTER[code + @SHIFT_LETTER_OFFSET_UNDS]
              end
            elsif code < @SHIFT_LETTER_THRESHOLD_A
              c = LETTER[code]
            end
            @ruby_mode_stop = true
          end
          if c
            @buffer.put(c)
            @buffer.refresh_screen
          end
        end
        report_hid(@modifier, @keycodes.join)

        if @switches.empty? && @locked_layer.nil?
          @layer = :default
        elsif @locked_layer
          # @type ivar @locked_layer: Symbol
          @layer = @locked_layer
        end
      else
        $encoders.each do |encoder|
          data = encoder.consume_rotation_partner
          uart_putc_raw(data) if data && data > 0
        end
        @switches.each do |switch|
          # 0b11111111
          #   ^^^      row number (0 to 7)
          #      ^^^^^ col number (0 to 31)
          uart_putc_raw((switch[0] << 5) + switch[1])
        end
      end

      time = default_sleep - (board_millis - now)
      sleep_ms(time) if time > 0
    end

  end

  #
  # Actions can be used in keymap.rb
  #

  # Raises layer and keeps it
  def raise_layer
    current_index = @layer_names.index(@locked_layer || @layer)
    return if current_index.nil?
    if current_index < @layer_names.size - 1
      # @type var current_index: Integer
      @locked_layer = @layer_names[current_index + 1]
    else
      @locked_layer = @layer_names.first
    end
  end

  # Lowers layer and keeps it
  def lower_layer
    current_index = @layer_names.index(@locked_layer || @layer)
    return if current_index.nil?
    if current_index == 0
      @locked_layer = @layer_names.last
    else
      # @type var current_index: Integer
      @locked_layer = @layer_names[current_index - 1]
    end
  end

  # Switch to specified layer
  def lock_layer(layer)
    @locked_layer = layer
  end

  def unlock_layer
    @locked_layer = nil
  end

  def macro(text, opts = [:ENTER])
    print text.to_s
    prev_c = ""
    text.to_s.each_char do |c|
      index = LETTER.index(c)
      next unless index
      @macro_keycodes << 0
      if index >= @SHIFT_LETTER_THRESHOLD_UNDS
        @macro_keycodes << (index - @SHIFT_LETTER_OFFSET_UNDS) * -1
      elsif index >= @SHIFT_LETTER_THRESHOLD_A
        @macro_keycodes << (index - @SHIFT_LETTER_OFFSET_A) * -1
      else
        @macro_keycodes << index
      end
    end
    opts.each do |opt|
      @macro_keycodes << 0
      @macro_keycodes << LETTER.index(opt)
      puts if opt == :ENTER
    end
  end

  def eval(script)
    if sandbox_picorbc(script)
      if sandbox_resume
        n = 0
        while sandbox_state != 0 do # 0: TASKSTATE_DORMANT == finished(?)
          sleep_ms 50
          n += 50
          if n > 10000
            puts "Error: Timeout (sandbox_state: #{sandbox_state})"
            break;
          end
        end
        macro("=> #{sandbox_result.inspect}")
      end
    else
      macro("Error: Compile failed")
    end
  end

  def ruby
    if @ruby_mode
      @macro_keycodes << LETTER.index(:ENTER)
      @buffer.adjust_screen
      eval @buffer.dump
      @buffer.clear
      @ruby_mode = false
      if $rgb
        $rgb.effect = @prev_rgb_effect || :rainbow
        $rgb.restore
      end
    else
      @buffer.refresh_screen
      @ruby_mode = true
      @ruby_mode_stop = false
      if $rgb
        @prev_rgb_effect = $rgb.effect
        $rgb.save
        $rgb.effect = :ruby
      end
    end
  end

end

$mutex = true
