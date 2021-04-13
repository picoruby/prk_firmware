$mutex = Mutex.new

$mutex.lock

GPIO_OUT = 1
GPIO_IN  = 0

HI = 1
LO = 0

MOD_KEYCODE = {
  KC_LCTL: 0b00000001,
  KC_LSFT: 0b00000010,
  KC_LALT: 0b00000100,
  KC_LGUI: 0b00001000,
  KC_RCTL: 0b00010000,
  KC_RSFT: 0b00100000,
  KC_RALT: 0b01000000,
  KC_RGUI: 0b10000000
}

# Due to PicoRuby's limitation,
# a big array can't be created at once
KEYCODE = [
  :KC_NO,               # 0x00
  :KC_ROLL_OVER,
  :KC_POST_FAIL,
  :KC_UNDEFINED,
  :KC_A,
  :KC_B,
  :KC_C,
  :KC_D,
  :KC_E,
  :KC_F,
  :KC_G,
  :KC_H,
  :KC_I,
  :KC_J,
  :KC_K,
  :KC_L,
  :KC_M,                # 0x10
  :KC_N,
  :KC_O,
  :KC_P,
  :KC_Q,
  :KC_R,
  :KC_S,
  :KC_T,
  :KC_U,
  :KC_V,
  :KC_W,
  :KC_X,
  :KC_Y,
  :KC_Z,
  :KC_1,
  :KC_2,
  :KC_3,                # 0x20
  :KC_4,
  :KC_5,
  :KC_6,
  :KC_7,
  :KC_8,
  :KC_9,
  :KC_0,
  :KC_ENTER,
  :KC_ESCAPE,
  :KC_BSPACE,
  :KC_TAB,
  :KC_SPACE,
  :KC_MINUS,
  :KC_EQUAL,
  :KC_LBRACKET,
] + [
  :KC_RBRACKET,         # 0x30
  :KC_BSLASH,
  :KC_NONUS_HASH,
  :KC_SCOLON,
  :KC_QUOTE,
  :KC_GRAVE,
  :KC_COMMA,
  :KC_DOT,
  :KC_SLASH,
  :KC_CAPSLOCK,
  :KC_F1,
  :KC_F2,
  :KC_F3,
  :KC_F4,
  :KC_F5,
  :KC_F6,
  :KC_F7,               # 0x40
  :KC_F8,
  :KC_F9,
  :KC_F10,
  :KC_F11,
  :KC_F12,
  :KC_PSCREEN,
  :KC_SCROLLLOCK,
  :KC_PAUSE,
  :KC_INSERT,
  :KC_HOME,
  :KC_PGUP,
  :KC_DELETE,
  :KC_END,
  :KC_PGDOWN,
  :KC_RIGHT,
  :KC_LEFT,             # 0x50
  :KC_DOWN,
  :KC_UP,
  :KC_NUMLOCK,
  :KC_KP_SLASH,
  :KC_KP_ASTERISK,
  :KC_KP_MINUS,
  :KC_KP_PLUS,
  :KC_KP_ENTER,
  :KC_KP_1,
  :KC_KP_2,
  :KC_KP_3,
  :KC_KP_4,
  :KC_KP_5,
  :KC_KP_6,
  :KC_KP_7,
] + [
  :KC_KP_8,             # 0x60
  :KC_KP_9,
  :KC_KP_0,
  :KC_KP_DOT,
  :KC_NONUS_BSLASH,
  :KC_APPLICATION,
  :KC_POWER,
  :KC_KP_EQUAL,
  :KC_F13,
  :KC_F14,
  :KC_F15,
  :KC_F16,
  :KC_F17,
  :KC_F18,
  :KC_F19,
  :KC_F20,
  :KC_F21,              # 0x70
  :KC_F22,
  :KC_F23,
  :KC_F24,
  :KC_EXECUTE,
  :KC_HELP,
  :KC_MENU,
  :KC_SELECT,
  :KC_STOP,
  :KC_AGAIN,
  :KC_UNDO,
  :KC_CUT,
  :KC_COPY,
  :KC_PASTE,
  :KC_FIND,
  :KC__MUTE,
  :KC_,                 # 0x80
  :KC_,
  :KC_,
  :KC_,
  :KC_,
  :KC_,
  :KC_,
  :KC_,
  :KC_,
  :KC_,
  :KC_,
  :KC_,
  :KC_,
  :KC_,
  :KC_,
  :KC_,
] + [
  :KC_LANG1,            # 0x90
  :KC_LANG2,
  :KC_LANG3,
  :KC_LANG4,
  :KC_LANG5,
  :KC_LANG6,
  :KC_LANG7,
  :KC_LANG8,
  :KC_LANG9,
  :KC_ALT_ERASE,
  :KC_SYSREQ,
  :KC_CANCEL,
  :KC_CLEAR,
  :KC_PRIOR,
  :KC_RETURN,
  :KC_SEPARATOR,
  :KC_OUT,              # 0xA0
  :KC_OPER,
  :KC_CLEAR_AGAIN,
  :KC_CRSEL,
  :KC_EXSEL,
]

# Keycodes with SHIFT modifier
KEYCODE_SFT = {
  KC_EXLM:           0x1e,
  KC_AT:             0x1f,
  KC_HASH:           0x20,
  KC_DLR:            0x21,
  KC_PERC:           0x22,
  KC_CIRC:           0x23,
  KC_AMPR:           0x24,
  KC_ASTER:          0x25,
  KC_LPRN:           0x26,
  KC_RPRN:           0x27,
  KC_PLUS:           0x2e,
  KC_LCBR:           0x2f,
  KC_RCBR:           0x30,
  KC_DQUO:           0x34,
  KC_LABK:           0x36,
  KC_RABK:           0x37,
}

class Keyboard
  def initialize
    @before_filters = Array.new
    @layers = Hash.new
    @mode_keys = Array.new
    @switches = Array.new
    @layer_names = Array.new
    @split = false
    @anchor = true
    @anchor_left = true # so-called "master left"
    @uart_pin = 0
    @rgb_pin = 25
  end

  attr_accessor :split, :uart_pin

  # val should be treated as `:left` if it's anything other than `:right`
  def set_anchor(val)
    @anchor_left = false if val == :right
  end

  def init_pins(rows, cols)
    if @split
      sleep 3 # Wait until USB ready
      @anchor = tud_mounted?
      report_hid(0, "\000\000\000\000\000\000")
      if @anchor
        rgb_init(@rgb_pin)
        resume_rgb
        uart_rx_init(@uart_pin)
      else
        gpio_init(@rgb_pin); # LED on board
        gpio_set_dir(@rgb_pin, GPIO_OUT);
        gpio_put(@rgb_pin, HI)
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
  end

  # Input
  #   name:    default, map: [ [ :KC_A, :KC_B, :KC_LCTL,   :MACRO_1 ],... ]
  # ↓
  # Result
  #   layer: { default:      [ [ -0x04, -0x05, 0b00000001, :MACRO_1 ],... ] }
  def add_layer(name, map)
    map.each_with_index do |row, row_index|
      row.each_with_index do |key, col_index|
        if KEYCODE.include?(key)
          map[row_index][col_index] = KEYCODE.index(key) * -1
        elsif KEYCODE_SFT[key]
          map[row_index][col_index] = (KEYCODE_SFT[key] + 0x100) * -1
        elsif MOD_KEYCODE[key]
          map[row_index][col_index] = MOD_KEYCODE[key]
        end
      end
    end
    @layers[name] = map
    @locked_layer_name ||= name
    @layer_names << name
  end

  # param[0] :on_release
  # param[1] :on_hold
  # param[2] :release_threshold
  # param[3] :repush_threshold
  def define_mode_key(key_name, param)
    @layers.each do |layer_name, map|
      map.each_with_index do |row, row_index|
        row.each_with_index do |key_symbol, col_index|
          if key_name == key_symbol
            on_release = case param[0].class
                         when Symbol
                           if KEYCODE.include?(param[0])
                             KEYCODE.index(param[0]) * -1
                           elsif KEYCODE_SFT[param[0]]
                             (KEYCODE_SFT[param[0]] + 0x100) * -1
                           end
                         when Array # Should be an Array of Symbol
                           ary = Array.new
                           param[0].each do |sym|
                             ary << if KEYCODE.include?(sym)
                               KEYCODE.index(sym) * -1
                             elsif KEYCODE_SFT[sym]
                               (KEYCODE_SFT[sym] + 0x100) * -1
                             else # Should be a modifier
                               MOD_KEYCODE[sym]
                             end
                           end
                           ary
                         else # Should be a Proc or a NilClass
                           param[0]
                         end
            on_hold = MOD_KEYCODE[param[1]] ? MOD_KEYCODE[param[1]] : param[1]
            @mode_keys << {
              layer_name:        layer_name,
              on_release:        on_release,
              on_hold:           on_hold,
              release_threshold: (param[2] || 0),
              repush_threshold:  (param[3] || 0),
              switch:            [row_index, col_index],
              prev_state:        :released,
              pushed_at:         0,
              released_at:       0,
            }
            break
          end
        end
      end
    end
  end

  def keys_include?(key)
    @keycodes.include? KEYCODE.index(key).chr
  end

  # MOD_KEYCODE = {
  #   KC_LCTL: 0b00000001,
  #   KC_LSFT: 0b00000010,
  #   KC_LALT: 0b00000100,
  #   KC_LGUI: 0b00001000,
  #   KC_RCTL: 0b00010000,
  #   KC_RSFT: 0b00100000,
  #   KC_RALT: 0b01000000,
  #   KC_RGUI: 0b10000000
  # }
  def invert_sft
    if ( (@modifier & 0b00000010) | (@modifier & 0b00100000) ) > 0
      @modifier &= 0b11011101
    else
      @modifier |= 0b00000010
    end
  end

  def before_report(&block)
    @before_filters << block
  end

  def action_on_release(mode_key)
    case mode_key.class
    when Fixnum # should be a normal key
      if mode_key < -255
        @keycodes << ((mode_key + 0x100) * -1).chr
        @modifier |= 0b00000010
      else
        @keycodes << (mode_key * -1).chr
      end
    when Array # Should be an array of Fixnum
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
      mode_key.call
    end
  end

  def action_on_hold(mode_key)
    case mode_key.class
    when Fixnum # should be a modifier key
      @modifier |= mode_key
    when Proc
      mode_key.call
    end
  end

  def start!
    @keycodes = Array.new
    while true
      now = board_millis
      @keycodes.clear
      @layer_name = @locked_layer_name

      @switches.clear
      @modifier = 0

      if @split && @anchor
        sleep_ms(5)
        # receive data from split partner
        while true
          data = uart_getc
          break if data.nil?
          switch = [data >> 4, data & 0b00001111]
          # To avoid chattering
          @switches << switch unless @switches.include?(switch)
        end
      end

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
                           (col - 3) * -1 + 8
                         end
                       else # right side is the anchor
                         unless @anchor
                           # left
                           col
                         else
                           # right
                           (col - 3) * -1 + 8
                         end
                       end
            @switches << [row, col_data]
          end
          break if @switches.size > 5
        end
        gpio_put(row_pin, HI)
      end

      if @anchor
        @mode_keys.each do |mode_key|
          next if mode_key[:layer_name] != @layer_name
          if @switches.include?(mode_key[:switch])
            case mode_key[:prev_state]
            when :released
              mode_key[:pushed_at] = now
              mode_key[:prev_state] = :pushed
              action_on_hold(mode_key[:on_hold])
            when :pushed
              action_on_hold(mode_key[:on_hold])
            when :pushed_then_released
              if now - mode_key[:released_at] <= mode_key[:repush_threshold]
                mode_key[:prev_state] = :pushed_then_released_then_pushed
              end
            when :pushed_then_released_then_pushed
              action_on_release(mode_key[:on_release])
              break
            end
          else
            case mode_key[:prev_state]
            when :pushed
              if now - mode_key[:pushed_at] <= mode_key[:release_threshold]
                action_on_release(mode_key[:on_release])
                mode_key[:prev_state] = :pushed_then_released
                mode_key[:released_at] = now
                break
              else
                mode_key[:prev_state] = :released
              end
            when :pushed_then_released
              if now - mode_key[:released_at] > mode_key[:release_threshold]
                mode_key[:prev_state] = :released
              end
            when :pushed_then_released_then_pushed
              mode_key[:prev_state] = :released
            end
          end
        end

        layer = @layers[@layer_name]
        @switches.each do |switch|
          keycode = layer[switch[0]][switch[1]]
          next unless keycode.is_a?(Fixnum)
          if keycode < -255 # Key with SHIFT
            @keycodes << ((keycode + 0x100) * -1).chr
            @modifier |= 0b00100000
          elsif keycode < 0 # Normal keys
            @keycodes << (keycode * -1).chr
          else # Modifier keys
            @modifier |= keycode
          end
        end

        (6 - @keycodes.size).times do
          @keycodes << "\000"
        end

        @before_filters.each do |block|
          block.call
        end

        report_hid(@modifier, @keycodes.join)
      else
        @switches.each do |switch|
          # 0b11111111
          #   ^^^^     row number (0 to 15)
          #       ^^^^ col number (0 to 15)
          uart_putc_raw((switch[0] << 4) + switch[1])
        end
      end

      time = 10 - (board_millis - now)
      sleep_ms(time) if time > 0
    end
  end

  #
  # Actions can be used in keymap.rb
  #

  # Raises layer and keeps it
  def raise_layer
    current_index = @layer_names.index(@locked_layer_name)
    if current_index < @layer_names.size - 1
      @locked_layer_name = @layer_names[current_index + 1]
    else
      @locked_layer_name = @layer_names.first
    end
  end

  # Lowers layer and keeps it
  def lower_layer
    current_index = @layer_names.index(@locked_layer_name)
    if current_index == 0
      @locked_layer_name = @layer_names.last
    else
      @locked_layer_name = @layer_names[current_index - 1]
    end
  end

  # Holds specified layer while pressed
  def hold_layer(layer_name)
    @layer_name = layer_name
  end

  # Switch to specified layer
  def switch_layer(layer_name)
    @locked_layer_name = layer_name
  end

end

$mutex.unlock
