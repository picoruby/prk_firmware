class Keyboard
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
    :KC_MUTE,
    :KC_VOLUP,                 # 0x80
    :KC_VOLDOWN,
    :KC_,
    :KC_,
    :KC_,
    :KC_,
    :KC_,
    :KC_INT1,
    :KC_INT2,
    :KC_INT3,
    :KC_INT4,
    :KC_INT5,
    :KC_INT6,
    :KC_INT7,
    :KC_INT8,
    :KC_INT9,
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
    KC_UNDS:           0x2d,
    KC_PLUS:           0x2e,
    KC_LCBR:           0x2f,
    KC_RCBR:           0x30,
    KC_PIPE:           0x31,
  #  KC_TILD:           0x32,
    KC_COLON:          0x33,
    KC_DQUO:           0x34,
    KC_TILD:           0x35,
    KC_LABK:           0x36,
    KC_RABK:           0x37,
    KC_QUES:           0x38,
  }

  KEYCODE_RGB = {
    RGB_TOG:          0x101,
    RGB_MODE_FORWARD: 0x102,
    RGB_MOD:          0x103,
    RGB_MODE_REVERSE: 0x104,
    RGB_RMOD:         0x105,
    RGB_HUI:          0x106,
    RGB_HUD:          0x107,
    RGB_SAI:          0x108,
    RGB_SAD:          0x109,
    RGB_VAI:          0x10a,
    RGB_VAD:          0x10b,
    RGB_SPI:          0x10c,
    RGB_SPD:          0x10d
  }

  letter = [
    nil,nil,nil,nil,
    'a', # 0x04
    'b',
    'c',
    'd',
    'e',
    'f',
    'g',
    'h',
    'i',
    'j',
    'k',
    'l',
    'm', # 0x10
    'n',
    'o',
    'p',
    'q',
    'r',
    's',
    't',
    'u',
    'v',
    'w',
    'x',
    'y',
    'z',
    '1',
    '2',
    '3', # 0x20
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',
    :ENTER,
    :ESCAPE,
    :BSPACE,
    :TAB,
    ' ',
    '-',
    '=',
    '[',
    ']', # 0x30
    "\\",
    nil, # ???
    ';',
    "'",
    '`',
    ',',
    '.',
    '/'
  ]
  letter[74] = :HOME
  letter += [
    :PGUP,
    :DELETE,
    :END,
    :PGDOWN,
    :RIGHT,
    :LEFT,   # 0x50
    :DOWN,
    :UP    # 82
  ]
  LETTER = letter + [
    'A',
    'B',
    'C',
    'D',
    'E',
    'F',
    'G',
    'H',
    'I',
    'J',
    'K',
    'L',
    'M', # 0x10
    'N',
    'O',
    'P',
    'Q',
    'R',
    'S',
    'T',
    'U',
    'V',
    'W',
    'X',
    'Y',
    'Z',
    '!',
    '@',
    '#',
    '$',
    '%',
    '^',
    '&',
    '*',
    '(',
    ')',
    '_',
    '+',
    '{',
    '}',
    '|',
    nil, # KC_TILD
    ':',
    '"',
    '~',
    '<',
    '>',
    '?'
  ]
  KC_ALIASES = {
    KC_ENT: :KC_ENTER,
    KC_ESC: :KC_ESCAPE,
    KC_BSPC: :KC_BSPACE,
    KC_SPC: :KC_SPACE,
    KC_MINS: :KC_MINUS,
    KC_EQL: :KC_EQUAL,
    KC_LBRC: :KC_LBRACKET,
    KC_RBRC: :KC_RBRACKET,
    KC_BSLS: :KC_BSLASH,
    # KC_NUHS: :KC_NONUS_HASH,
    # KC_SCLN: :KC_SCOLON,
    KC_QUOT: :KC_QUOTE,
    KC_GRV: :KC_GRAVE,
    KC_ZKHK: :KC_GRAVE,
    KC_COMM: :KC_COMMA,
    KC_SLSH: :KC_SLASH,
    # KC_NUBS: :KC_NONUS_BSLASH,
    # KC_CLCK: :KC_CAPSLOCK,
    KC_CAPS: :KC_CAPSLOCK,
    # KC_SLCK: :KC_SCROLLLOCK,
    # KC_BRMD: :KC_SCROLLLOCK,
    # KC_NLCK: :KC_NUMLOCK,
    # KC_LCTRL: :KC_LCTL,
    # KC_LSHIFT: :KC_LSFT,
    # KC_LOPT: :KC_LALT,
    # KC_LCMD: :KC_LGUI,
    # KC_LWIN: :KC_LGUI,
    # KC_RCTRL: :KC_RCTL,
    # KC_RSHIFT: :KC_RSFT,
    # KC_ROPT: :KC_RALT,
    # KC_ALGR: :KC_RALT,
    # KC_RCMD: :KC_RGUI,
    # KC_RWIN: :KC_RGUI,
    KC_KANA: :KC_INT2,
    KC_HENK: :KC_INT4,
    KC_MHEN: :KC_INT5,
    KC_HAEN: :KC_LANG1,
    KC_HANJ: :KC_LANG2,
    # KC_PSCR: :KC_PSCREEN,
    # KC_PAUS: :KC_PAUSE,
    # KC_BRK: :KC_PAUSE,
    # KC_BRMU: :KC_PAUSE,
    KC_INS: :KC_INSERT,
    KC_DEL: :KC_DELETE,
    KC_PGDN: :KC_PGDOWN,
    KC_RGHT: :KC_RIGHT,
    # KC_APP: :KC_APPLICATION,
    # KC_EXEC: :KC_EXECUTE,
    # KC_SLCT: :KC_SELECT,
    # KC_AGIN: :KC_AGAIN,
    # KC_PSTE: :KC_PASTE,
    # KC_ERAS: :KC_ALT_ERASE,
    # KC_CLR: :KC_CLEAR,
    # KC_PSLS: :KC_KP_SLASH,
    # KC_PAST: :KC_KP_ASTERISK,
    # KC_PMNS: :KC_KP_MINUS,
    # KC_PPLS: :KC_KP_PLUS,
    # KC_PENT: :KC_KP_ENTER,
    # KC_P1: :KC_KP_1,
    # KC_P2: :KC_KP_2,
    # KC_P3: :KC_KP_3,
    # KC_P4: :KC_KP_4,
    # KC_P5: :KC_KP_5,
    # KC_P6: :KC_KP_6,
    # KC_P7: :KC_KP_7,
    # KC_P8: :KC_KP_8,
    # KC_P9: :KC_KP_9,
    # KC_P0: :KC_KP_0,
    # KC_PDOT: :KC_KP_DOT,
    # KC_PEQL: :KC_KP_EQUAL,
    XXXXXXX: :KC_NO,
  }
  letter = nil
end

#
# Keyboard class have to be defined twice to avoid "too big operand" error of mrbc
# It seems `b` operand of OP_LOADSYM will be more than 0xFFFF if the class is united
#

class Keyboard

  STANDARD_SPLIT = :standard_split
  RIGHT_SIDE_FLIPPED_SPLIT = :right_side_flipped_split
  VIA_FILENAME = "VIA_MAP RB "

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
    @composite_keys = Array.new
    @mode_keys = Array.new
    @mode_keys_via = Hash.new
    @enable_via = false
    @via_layer_count = 5
    @keymap_saved = true
    @switches = Array.new
    @layer_names = Array.new
    @layer = :default
    @split = false
    @split_style = STANDARD_SPLIT
    @anchor = true
    @anchor_left = true # so-called "master left"
    @uart_pin = 1
    $rgb = nil
    @encoders = Array.new
    @partner_encoders = Array.new
    @macro_keycodes = Array.new
    @buffer = Buffer.new("picoirb")
    @scan_mode = :matrix
  end

  attr_accessor :split, :uart_pin
  attr_reader :layer, :split_style

  # TODO: OLED, SDCard
  def append(feature)
    case feature.class
    when RGB
      # @type var feature: RGB
      $rgb = feature
      $rgb.anchor = @anchor
    when RotaryEncoder
      # @type var feature: RotaryEncoder
      if @split
        feature.create_keycodes(@partner_encoders.size)
        if @anchor_left
          if @anchor == feature.left? # XNOR
            feature.init_pins
            @encoders << feature
          end
        else
          if @anchor != feature.left? #XOR
            feature.init_pins
            @encoders << feature
          end
        end
        if @anchor && (@anchor_left != feature.left?)
          @partner_encoders << feature
        end
      else
        feature.init_pins
        @encoders << feature
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

  def set_scan_mode(mode)
    case mode
    when :matrix, :direct
      @scan_mode = mode
    else
      puts 'Scan mode only support :matrix and :direct. (default: :matrix)'
    end
  end

  def init_pins(rows, cols)
    puts "Initializing GPIO ..."
    if @split
      print "Configured as a split-type"
      @anchor = tud_mounted?
      if @anchor
        uart_anchor_init(@uart_pin)
        puts " Anchor"
      else
        uart_partner_init(@uart_pin)
        puts " Partner"
      end
    end
    sleep_ms 500
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

  def init_direct_pins(pins)
    set_scan_mode :direct
    init_pins([], pins)
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
      case key.class
      when Symbol
        # @type var map: Array[Symbol]
        # @type var key: Symbol
        new_map[row_index][col_position] = find_keycode_index(key)
      when Array
        # @type var map: Array[Array[Symbol]]
        # @type var key: Array[Symbol]
        composite_key = key.join("_").to_sym
        new_map[row_index][col_position] = composite_key
        @composite_keys << {
          layer: name,
          keycodes: convert_composite_keys(key),
          switch: [row_index, col_position]
        }
      end
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
    elsif KEYCODE_RGB[key]
      KEYCODE_RGB[key]
    else
      key
    end
  end

  def convert_composite_keys(keys)
    keys.map do |sym|
      keycode_index = KEYCODE.index(sym)
      if keycode_index
        keycode_index * -1
      elsif KEYCODE_SFT[sym]
        (KEYCODE_SFT[sym] + 0x100) * -1
      else # Should be a modifier
        MOD_KEYCODE[sym]
      end
    end
  end

  def define_composite_key(key_name, keys)
    @keymaps.each do |layer, map|
      map.each_with_index do |row, row_index|
        row.each_with_index do |key_symbol, col_index|
          if key_name == key_symbol
            @composite_keys << {
              layer: layer,
              keycodes: convert_composite_keys(keys),
              switch: [row_index, col_index]
            }
          end
        end
      end
    end
  end

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
              convert_composite_keys(on_release)
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

  # for Encoders
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
        keycode = KEYCODE_RGB[symbol]
        if keycode && $rgb
          $rgb.invoke_anchor(symbol)
        end
        return
      end
    end
    report_hid(modifier, "#{c}\000\000\000\000\000")
    sleep_ms 1
    report_hid(0, "\000\000\000\000\000\000")
    sleep_ms 1
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
    if @split && @anchor
      sleep_ms 100
      while true
        data = uart_anchor(0)
        break if data == 0xFFFFFF
      end
    end

    rgb_message = 0
    while true
      cycle_time = 20
      now = board_millis
      @keycodes.clear

      @switches.clear
      @modifier = 0

      @switches = @scan_mode == :matrix ? scan_matrix! : scan_direct!

      # TODO: more features
      $rgb.fifo_push(true) if $rgb && !@switches.empty?

      if @anchor
        # Receive max 3 switches from partner
        if @split
          sleep_ms 3
          if rgb_message > 0
            data24 = uart_anchor(rgb_message)
            rgb_message = 0
          elsif $rgb && $rgb.ping?
            data24 = uart_anchor(0b11100000) # adjusts RGB time
          else
            data24 = uart_anchor(0)
          end
          [data24 & 0xFF, (data24 >> 8) & 0xFF, data24 >> 16].each do |data|
            if data == 0xFF
              # do nothing
            elsif data > 246
              @partner_encoders.each { |encoder| encoder.call_proc_if(data) }
            else
              switch = [data >> 5, data & 0b00011111]
              # To avoid chattering
              @switches << switch unless @switches.include?(switch)
            end
          end
        end

        desired_layer = @layer
        @composite_keys.each do |composite_key|
          next if composite_key[:layer] != @layer
          if @switches.include?(composite_key[:switch])
            action_on_release(composite_key[:keycodes])
          end
        end
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
          elsif keycode > 0x100
            rgb_message = $rgb.invoke_anchor KEYCODE_RGB.key(keycode)
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
          cycle_time = 40 # To avoid accidental skip
        end

        (6 - @keycodes.size).times do
          @keycodes << "\000"
        end

        @before_filters.each do |block|
          block.call
        end

        @encoders.each do |encoder|
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
        # Partner
        @encoders.each do |encoder|
          data = encoder.consume_rotation_partner
          uart_partner_push8(data) if data && data > 0
        end
        @switches.each do |switch|
          # 0b11111111
          #   ^^^      row number (0 to 7)
          #      ^^^^^ col number (0 to 31)
          uart_partner_push8((switch[0] << 5) + switch[1])
        end
        rgb_message = uart_partner
        $rgb.invoke_partner rgb_message if $rgb
      end
      
      via_task if @enable_via 

      time = cycle_time - (board_millis - now)
      sleep_ms(time) if time > 0
    end

  end

  def scan_matrix!
    switches = []
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
          switches << [row, col_data]
        end
        # @type break: nil
        break if switches.size >= @cols.size
      end
      gpio_put(row_pin, HI)
    end
    return switches
  end

  def scan_direct!
    switches = []
    @cols.each_with_index do |col_pin, col|
      if gpio_get(col_pin) == LO
        switches << [0, col]
      end
    end
    return switches
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

  def eval_val(script)
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
        return sandbox_result
      end
    else
      return nil
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
      end
    else
      @buffer.refresh_screen
      @ruby_mode = true
      @ruby_mode_stop = false
      if $rgb
        @prev_rgb_effect = $rgb.effect
        $rgb.effect = :ruby
      end
    end
  end
end

class Keyboard
  VIA_IDs = %i[
    ID_NONE
    ID_GET_PROTOCOL_VERISON
    ID_GET_KEYBOARD_VALUE
    ID_SET_KEYBOARD_VALUE
    ID_VIA_GET_KEYCODE
    ID_VIA_SET_KEYCODE
    ID_VIA_RESET
    ID_LIGHTING_SET_VALUE
    ID_LIGHTING_GET_VALUE
    ID_LIGHTING_SAVE
    ID_EEPROM_RESET
    ID_BOOTLOADER_JUMP
    ID_VIA_MACRO_GET_COUNT
    ID_VIA_MACRO_GET_BUFFER_SIZE
    ID_VIA_MACRO_GET_BUFFER
    ID_VIA_MACRO_SET_BUFFER
    ID_VIA_MACRO_RESET
    ID_VIA_GET_LAYER_COUNT
    ID_VIA_GET_BUFFER
    ID_VIA_SET_BUFFER
  ]
  ID_UNHANDLED = 0xFF
  VIA_PROTOCOL_VERSION = 0x0009
  KEYBOARD_VALUES = %i[
    ID_NONE
    ID_UPTIME
    ID_LAYOUT_OPTIONS
    ID_SWITCH_MATRIX_STATE
  ]

  def load_mode_keys_via
    @mode_keys_via.each do |key_name, param|
      define_mode_key(key_name, param)
    end
  end

  def define_mode_key_via(key_name, param)
    @mode_keys_via[key_name] = param
  end

  def get_composite_key(name)
    names = "LCTL LSFT LALT LGUI RCTL RSFT RALT RGUI".split
    
    keynames = []
    ary = name.to_s.split("_")
    ary.each do |n|
      keynames << ("KC_"+n).intern
    end

    return keynames
  end

  def get_modifier_name(bits)
    names = "LCTL LSFT LALT LGUI RCTL RSFT RALT RGUI".split
    ary = []
    8.times do |i|
      unless ( bits & (1<<i) )==0
        ary << names[i]
      end
    end
    return ary.join("_")
  end

  def get_via_composite_keycode(keyname)
    names = "LCTL LSFT LALT LGUI RCTL RSFT RALT RGUI".split
    modifier = 0
    via_keycode = 0
    keyname.to_s.split("_").each do |n|
      i = names.index n
      if i
        modifier |= 1<<i
      else
        key_str = "KC_"+n
        prk_keycode = find_keycode_index( key_str.intern )
        
        if prk_keycode.class == Integer
          # @type var prk_keycode: Integer
          via_keycode = get_keycode(prk_keycode)
        else
          via_keycode = 0
        end
      end
    end
    
    if 0x0F & modifier == 0
      via_keycode |= (modifier<<4) & 0x0F00
      via_keycode |= 0x1000
    else
      via_keycode |= modifier<<8
    end

    return via_keycode
  end

  # VIA_Keycode -> :VIA_FUNCn | PRK_Keycode
  def translate_keycode(keycode)
    if (keycode>>8)==0
      if 0x00C0 <= keycode && keycode <= 0x00DF
        c = keycode - 0x00C0
        cs = c.to_s
        s = "VIA_FUNC" + cs
        return s.intern
      end
      return -(keycode & 0x00FF)
    else
      case (keycode>>12) & 0x0F
      when 0x0
        modifiers = (keycode>>8) & 0x0F
        prk_keycode = -(keycode & 0x00FF)
        return [ modifiers, prk_keycode ]
      when 0x1
        modifiers = ( (keycode>>8) & 0x0F )<<4
        prk_keycode = -(keycode & 0x00FF)
        return [ modifiers, prk_keycode ]
      end
    end

    return 0x0000
  end

  # PRK_KeySymbol | PRK_Keycode -> VIA_Keycode
  def get_keycode(key)
    case key.class
    when Integer
      # @type var key: Integer
      if key<-255
        key = key * (-1) - 0x100
        return (0x12<<8) | key
      else
        return -key
      end
    when Symbol
      # @type var key: Symbol
      if key==:KC_NO
        return 0
      else
        key_str = key.to_s
        if key_str.start_with?("VIA_FUNC")
          return 0x00C0 + key_str.split("C")[1].to_i
        elsif ! key_str.start_with?("KC_")
          return get_via_composite_keycode(key)
        else
          return 0
        end
      end
    else
      return 0
    end
  end

  # PRK_KeySymbol | PRK_Keycode -> keymap.rb Symbol 
  def find_keyname(key)
    case key.class
    when Integer
      # @type var key: Integer
      if key<-255
        key = key * (-1)
        KEYCODE_RGB.each do |k,v|
          return k if v == key
        end

        key -= 0x100
        KEYCODE_SFT.each do |k,v|
          return k if v == key
        end
        return :KC_NO
      else
        return KEYCODE[-key] || :KC_NO
      end
    when Symbol
      # @type var key: Symbol
      return key
    else
      return :KC_NO
    end
  end

  def convert_to_uint8_array(str)
    data = []
    str.each_char do |c|
      data << c.ord
    end
    return data
  end

  def save_keymap_via
    keymap_strs = []
    
    @via_layer_count.times do |i|
      layer_name = via_get_layer_name i
      map = @keymaps[layer_name]
      next unless map

      keymap = []
      map.each do |row|
        keycodes_row = []
        
        row.each do |key|
          keycodes_row << find_keyname(key).to_s
        end

        (@entire_cols_size - row.size).times do |i|
          keycodes_row << "KC_NO"
        end
        keymap << keycodes_row.join(" ")
      end
      keymap_strs << keymap.join(" \n    ")
    end

    data = "keymap = [ \n%i[ "
    data << keymap_strs.join("], \n\n%i[ ")
    data << " ] \n]\n"
    binary = convert_to_uint8_array(data)
    
    write_file_internal(VIA_FILENAME, binary);

    load_mode_keys_via
  end
  
  def via_enable(layer_count=7)
    @via_layer_count = layer_count
    fileinfo = find_file(VIA_FILENAME)
    
    if fileinfo
      script = ""
      ary = read_file(fileinfo[0], fileinfo[1])
      ary.each do |i|
        script << i.chr;
      end

      ret = eval_val(script)
      
      if ret.class == Array
        # @type var ret: Array[Array[Symbol]]
        keynames_not_kc = []
        
        ret.each_with_index do |map,i|
          layer_name = via_get_layer_name i
          add_layer layer_name, map
          map.each do |kc|
            next if kc.to_s.start_with?("KC")
            next if kc.to_s.start_with?("VIA_FUNC")
            keynames_not_kc << kc
          end
        end

        keynames_not_kc.each do |kc|
          # composite keys
          define_composite_key kc, get_composite_key(kc)
        end
      end
    else
      i = 0
      @keymaps.each do |name, map|
        next if name.to_s.start_with?("VIA_LAYER")
        via_layer_name = via_get_layer_name i
        @keymaps[via_layer_name] = map
        i += 1
      end
      save_keymap_via
    end

    @enable_via = true
    load_mode_keys_via
  end

  def raw_hid_receive_kb(data)
    ary = Array.new(data.size)
    ary[0] = ID_UNHANDLED
    return ary
  end

  def raw_hid_receive(data)
      command_id   = data[0]
      command_name = VIA_IDs[command_id]

      case command_name
      when :ID_GET_PROTOCOL_VERSION
          data[1] = VIA_PROTOCOL_VERSION >> 8
          data[2] = VIA_PROTOCOL_VERSION & 0xFF
      when :ID_GET_KEYBOARD_VALUE
          case KEYBOARD_VALUES[data[1]]
          when :ID_UPTIME
              value  = board_millis >> 10 # seconds
              data[2] = (value >> 24) & 0xFF
              data[3] = (value >> 16) & 0xFF
              data[4] = (value >> 8) & 0xFF
              data[5] = value & 0xFF
          when :ID_LAYOUT_OPTIONS
              value  = 0x12345678 #via_get_layout_options
              data[2] = (value >> 24) & 0xFF
              data[3] = (value >> 16) & 0xFF
              data[4] = (value >> 8) & 0xFF
              data[5] = value & 0xFF
          when :ID_SWITCH_MATRIX_STATE
              if (@entire_cols_size / 8 + 1) * @rows.size <= 28
                  i = 2;
                  @rows.size.times do |row|
                      value = 0 #matrix_get_row(row)
                      if @entire_cols_size > 24
                        data[i] = (value >> 24) & 0xFF
                        i += 1
                      end
                      if @entire_cols_size > 16
                        data[i] = (value >> 16) & 0xFF
                        i += 1
                      end
                      if @entire_cols_size > 8
                        data[i] = (value >> 8) & 0xFF
                        i += 1
                      end
                      data[i] = value & 0xFF
                      i += 1
                  end
              end
          else
              data = raw_hid_receive_kb(data)
          end
      
      when :ID_SET_KEYBOARD_VALUE
          case KEYBOARD_VALUES[data[1]]
          when :ID_LAYOUT_OPTIONS
              value = (data[2] << 24) | (data[3] << 16)  | (data[4] << 8) | data[5]
              #via_set_layout_options(value);
          else
              data = raw_hid_receive_kb(data)
          end
      when :ID_VIA_GET_KEYCODE
          keycode = dynamic_keymap_get_keycode(data[1], data[2], data[3])
          data[4]  = keycode >> 8
          data[5]  = keycode & 0xFF
      when :ID_VIA_SET_KEYCODE
          dynamic_keymap_set_keycode(data[1], data[2], data[3], (data[4] << 8) | data[5])
          @keymap_saved = false
      when :ID_VIA_MACRO_GET_COUNT
          data[1] = 0
          return data
      when :ID_VIA_MACRO_GET_BUFFER_SIZE
          size   = 0x0 #dynamic_keymap_macro_get_buffer_size()
          data[1] = size >> 8
          data[2] = size & 0xFF
      when :ID_VIA_GET_LAYER_COUNT
        data[1] = @via_layer_count
        return data
      when :ID_VIA_GET_BUFFER
          data = dynamic_keymap_get_buffer(data)
          
          unless @keymap_saved
              save_keymap_via
              @keymap_saved = true
          end
      when :ID_VIA_SET_BUFFER
          dynamic_keymap_set_buffer(data)
          
          keymap_saved = false;
      else

      end

      return data
  end

  def via_get_layer_name(i)
    return :default if i==0
    
    s = "VIA_LAYER"+i.to_s
    return s.intern
  end

  def dynamic_keymap_set_buffer(data)
    # @type var data: Array[Integer]
    offset = (data[1]<<8) | data[2]
    size   = data[3]
    map_size = @entire_cols_size * @rows.size
    layer_num = offset/2/map_size
    key_index = offset/2 - layer_num * map_size
    
    layer_name = via_get_layer_name layer_num
    
    (data.size/2).times do |i|
      next if i<2 #header
      keycode = data[i*2] << 8 | data[i*2+1]
      if key_index==map_size
        layer_num += 1
        layer_name = via_get_layer_name layer_num
      end
      row = (key_index / @rows.size).to_i
      col = (key_index % @rows.size).to_i
    
      keyname = translate_keycode(keycode)

      unless @keymaps[layer_name]
        @layer_names << layer_name
        @keymaps[layer_name] = Array.new(@rows.size)
        @rows.size.times { |i| @keymaps[layer_name][i] = [] }
      end
      
      @composite_keys.delete_if { |item| item[:layer]==layer_name && item[:switch]==[row, col] }
      @mode_keys.delete_if { |item| item[:layer]==layer_name && item[:switch]==[row, col] }
    
      case keyname.class
      when Array
        # composite key
        # @type var keyname: [ Integer, Integer ]
        @composite_keys << {
          layer: layer_name,
          keycodes: keyname,
          switch: [ row, col ]
        }
      else
        # @type var keyname: ( Symbol | Integer )
        @keymaps[layer_name][row][col] = keyname
      end
      
      key_index += 1
    end
  end

  def dynamic_keymap_get_buffer(data)
    offset = (data[1]<<8) | data[2]
    size   = data[3]
    map_size = @entire_cols_size * @rows.size
    layer_num = (offset/2/map_size).to_i
    key_index = (offset/2 - layer_num * map_size).to_i
    
    layer_name = via_get_layer_name layer_num
    
    (data.size/2).times do |i|
      next if i<2 #header
      
      if key_index==map_size
        layer_num += 1
        layer_name = via_get_layer_name layer_num
        key_index = 0
      end
      keyname = if @keymaps[layer_name]
          @keymaps[layer_name][key_index/@entire_cols_size][key_index % @entire_cols_size]
        else
          :KC_NO
        end
      keycode = get_keycode(keyname)
      
      # @type var keycode : Integer
      data[i*2] = (keycode >> 8) & 0xFF
      data[i*2+1] = keycode & 0xFF
      
      key_index += 1
    end
    
    return data
  end

  def dynamic_keymap_get_keycode(layer, row, col)
    layer_name = via_get_layer_name layer
    keyname = if @keymaps[layer_name]
        @keymaps[layer_name][row][col]
      else
        :KC_NO
      end
    return get_keycode(keyname)
  end

  def dynamic_keymap_set_keycode(layer, row, col, keycode)
    layer_name = via_get_layer_name layer
    keyname = translate_keycode(keycode)
    
    unless @keymaps[layer_name]
      @keymaps[layer_name] = Array.new(@rows.size)
      @layer_names << layer_name
      @rows.size.times { |i| @keymaps[layer_name][i] = [] }
    end

    @composite_keys.delete_if { |item| item[:layer]==layer_name && item[:switch]==[row, col] }
    @mode_keys.delete_if { |item| item[:layer]==layer_name && item[:switch]==[row, col] }

    case keyname.class
    when Array
      # composite key
      # @type var keyname: [Integer, Integer]
      @composite_keys << {
        layer: layer_name,
        keycodes: keyname,
        switch: [ row, col ]
      }
      @keymaps[layer_name][row][col] = (
          get_modifier_name(keyname[0]) + "_" + 
          find_keyname(keyname[1]).to_s.split("_")[1] ).intern
    else
      # @type var keyname: ( Symbol | Integer )
      @keymaps[layer_name][row][col] = keyname
    end
  end
  
  def via_task
    if raw_hid_report_received?
      data = raw_hid_receive( get_last_received_raw_hid_report )
      # sleep is needed to be received
      sleep_ms 1
      report_raw_hid( data )
    end
  end
end

$mutex = true
