class VIA
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

  VIA_FILENAME = "VIA_MAP RB "

  def initialize
    puts "Initializing VIA."
    @layer_count = 5
    @mode_keys = Hash.new
    @keymap_saved = true
    @rows_size = 1
    @cols_size = 1
    @keymaps = Array.new
    @composite_keys = Array.new
  end

  attr_accessor :layer_count, :kbd, :cols_size, :rows_size

  def load_mode_keys
    @mode_keys.each do |key_name, param|
      @kbd.define_mode_key(key_name, param)
    end

    @composite_keys.each do |key_name|
      @kbd.define_composite_key(key_name, expand_composite_key(key_name))
    end
  end

  def define_mode_key(key_name, param)
    @mode_keys[key_name] = param
  end

  def expand_composite_key(name)
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
        prk_keycode = @kbd.find_keycode_index( key_str.intern )
        
        if prk_keycode.class == Integer
          # @type var prk_keycode: Integer
          via_keycode = prk_keycode_into_via_keycode(prk_keycode)
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
  def via_keycode_into_prk_keycode(keycode)
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
  def prk_keycode_into_via_keycode(key)
    case key.class
    when Integer
      # @type var key: Integer
      if key>0
        if key < 0x100
          ret = 0
          8.times do |i|
            unless (key&(1<<i)) == 0
              ret = 0xE0 + i
            end
          end
          return ret
        else
          return 0
        end
      elsif key<-255
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


  def via_keycode_into_keysymbol(keycode)
    if (keycode>>8)==0
      if keycode<0xE0
        return @kbd.keycode_to_keysym(keycode & 0x00FF)
      elsif 0x00C0 <= keycode && keycode <= 0x00DF
        c = keycode - 0x00C0
        cs = c.to_s
        s = "VIA_FUNC" + cs
        return s.intern
      else
        i = keycode - 0xE0
        return :KC_NO if i>8
        bits = 1<<i
        return ( "KC_"+get_modifier_name(bits) ).intern
      end
    else
      case (keycode>>12) & 0x0F
      when 0x0
        modifiers = (keycode>>8) & 0x0F
        key = keycode & 0x00FF
        keysymbol = (
          get_modifier_name(modifiers) + "_" + 
          via_keycode_into_keysymbol(key).to_s.split("_")[1] ).intern
        return keysymbol
      when 0x1
        modifiers = ( (keycode>>8) & 0x0F )<<4
        key = keycode & 0x00FF
        keysymbol = (
          get_modifier_name(modifiers) + "_" + 
          via_keycode_into_keysymbol(key).to_s.split("_")[1] ).intern
        return keysymbol
      end
    end

    return :KC_NO
  end

  def convert_to_uint8_array(str)
    data = []
    str.each_char do |c|
      data << c.ord
    end
    return data
  end
  
  def init_keymap
    @layer_count.times do |layer|
      @keymaps[layer] = []
      @rows_size.times do |row|
        @keymaps[layer][row] = Array.new(@cols_size)
      end
    end
  end

  def sync_keymap(init=false)
    @layer_count.times do |i|
      layer_name = init ? nil : via_get_layer_name(i)
      layer = @kbd.get_layer(layer_name, i)
      
      if layer
        via_map = []
        layer.each_with_index do |rows, row|
          via_map[row] = []
          rows.each_with_index do |cell, col|
            via_map[row][col] = cell ? prk_keycode_into_via_keycode(cell) : 0
          end
        end
        @keymaps[i] = via_map
      end
    end
  end

  def save_keymap
    save_on_keyboard
    save_on_flash
  end

  def save_on_keyboard
    @layer_count.times do |layer|
      layer_name = via_get_layer_name(layer)
      @kbd.delete_mode_keys(layer_name)
      
      map = Array.new(@rows_size)
      @rows_size.times do |row|
        map[row] = Array.new(@cols_size)
        @cols_size.times do |col|
          keyname = via_keycode_into_prk_keycode(@keymaps[layer][row][col] || 0)
          
          case keyname.class
          when Array
            # composite key
            # @type var keyname: [Integer, Integer]
            keysymbol = (
              get_modifier_name(keyname[0]) + "_" + 
              via_keycode_into_keysymbol(keyname[1]).to_s.split("_")[1] ).intern
            @composite_keys << keysymbol unless @composite_keys.include?(keysymbol)
            map[row][col] = keysymbol
          else
            # @type var keyname: ( Symbol | Integer )
            map[row][col] = keyname
          end
        end
      end
      
      @kbd.set_layer( layer_name, map )
    end
    load_mode_keys
  end

  def save_on_flash
    data = "keymap = [ \n%i[ "

    @layer_count.times do |i|
      @rows_size.times do |j|
        @cols_size.times do |k|
          key = via_keycode_into_keysymbol(@keymaps[i][j][k] || 0)
          data << key.to_s + " "
        end

        data << " \n    "
      end

      data << "], \n\n%i[ "
    end

    data << "KC_NO ] \n]\n"
    
    write_file_internal(VIA_FILENAME, data)
  end
  
  def start!
    init_keymap

    fileinfo = find_file(VIA_FILENAME)
    
    if fileinfo
      puts "via_map.rb found"
      script = ""
      ary = read_file(fileinfo[0], fileinfo[1])
      ary.each do |i|
        script << i.chr;
      end

      ret = eval_val(script)
    
      if ret.class == Array
        puts "loading VIA map"
        # @type var ret: Array[Array[Symbol]]
        ret.each_with_index do |map,i|
          layer_name = via_get_layer_name i
          @kbd.add_layer layer_name, map
          map.each do |kc|
            next if kc.to_s.start_with?("KC")
            next if kc.to_s.start_with?("VIA_FUNC")
            # composite keys
            @composite_keys << kc
          end
        end
        
        sync_keymap(false)
      else
        sync_keymap(true)
      end
    else
      sync_keymap(true)
      save_on_keyboard
    end
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
        # not implemented
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
      when :ID_VIA_MACRO_GET_COUNT
        data[1] = 0
      when :ID_VIA_MACRO_GET_BUFFER_SIZE
        size   = 0x0 #dynamic_keymap_macro_get_buffer_size()
        data[1] = size >> 8
        data[2] = size & 0xFF
      when :ID_VIA_GET_LAYER_COUNT
        data[1] = @layer_count
      when :ID_VIA_GET_BUFFER
        data = dynamic_keymap_get_buffer(data)
        
        unless @keymap_saved
          save_keymap
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
    layer_num = ( offset/2/(@cols_size*@rows_size) ).to_i
    key_index = ( offset/2 - layer_num * (@cols_size*@rows_size) ).to_i
    
    (size/2).times do |i|
      keycode = data[i*2+4] << 8 | data[i*2+5]
      if key_index==(@cols_size*@rows_size)
        layer_num += 1
        key_index = 0
      end
      row = (key_index / @rows_size).to_i
      col = (key_index % @rows_size).to_i
      
      dynamic_keymap_set_keycode(layer_num, row, col, keycode);

      key_index += 1
    end
  end

  def dynamic_keymap_get_buffer(data)
    offset = (data[1]<<8) | data[2]
    size   = data[3]
    layer_num = (offset/2/(@cols_size*@rows_size)).to_i
    key_index = (offset/2 - layer_num * (@cols_size*@rows_size)).to_i
    
    (size/2).times do |i|
      if key_index==(@cols_size*@rows_size)
        layer_num += 1
        key_index = 0
      end
      
      row = key_index / @cols_size
      col = key_index % @cols_size

      keycode = dynamic_keymap_get_keycode(layer_num, row, col)
      
      # @type var keycode : Integer
      data[i*2+4] = (keycode >> 8) & 0xFF
      data[i*2+5] = keycode & 0xFF
      
      key_index += 1
    end
    
    return data
  end

  def dynamic_keymap_get_keycode(layer, row, col)
    return @keymaps[layer][row][col] || 0
  end

  def dynamic_keymap_set_keycode(layer, row, col, keycode)
    @keymaps[layer][row][col] = keycode
    @keymap_saved = false
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
      puts "Error: Compile failed"
      return nil
    end
  end

  def task
    if raw_hid_report_received?
      data = raw_hid_receive( get_last_received_raw_hid_report )
      # sleep is needed to be received
      sleep_ms 1
      report_raw_hid( data )
    end
  end
end
