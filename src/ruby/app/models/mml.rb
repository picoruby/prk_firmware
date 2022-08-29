# MML stands for "Music Macro Language"

class MML

  def self.number_str(str, i)
    str_number = ""
    i += 1
    while 47 < (c = (str[i]&.ord&.to_i).to_i) && c < 58
      str_number << str[i].to_s
      i += 1
    end
    return [str_number, i - 1]
  end

  def self.count_punto(str, i)
    punti = 0
    i += 1
    while str[i] == "."
      punti += 1
      i += 1
    end
    return [punti, i - 1]
  end

  def self.coef(punti)
    result = 1.0
    punti.times do |i|
      result += 0.5 ** (i + 1)
    end
    result
  end

  DURATION_BASE = (1000 * 60 * 4).to_f

  def self.compile(str)
    notes = Array.new
    i = 0
    str.each_char do |c|
      ord = c.ord
      if 64 < ord && ord < 91
        str[i] = (ord + 32).chr
      end
      i += 1
    end
    i = -1
    size = str.length
    octave = 4
    tempo = 120
    common_duration = (DURATION_BASE / tempo / 4).to_i
    while true do
      i += 1
      break if i == size
      octave_fix = 0
      note = case str[i]
      when "r"; -1
      when "a"; octave_fix = 1; 0
      when "b"; octave_fix = 1; 2
      when "c"; 3
      when "d"; 5
      when "e"; 7
      when "f"; 8
      when "g"; 10
      else
        case str[i]
        when "t"
          tempo_str, i = number_str(str, i)
          tempo = tempo_str.to_i if 0 < tempo_str.size
          common_duration = (DURATION_BASE / tempo / 4).to_i
        when "o"
          i += 1
          octave = str[i].to_i
        when ">"
          octave -= 1
        when "<"
          octave += 1
        when "l"
          fraction_str, i = number_str(str, i)
          fraction = 0 < fraction_str.size ? fraction_str.to_i : nil
          if fraction
            punti, i = count_punto(str, i)
            common_duration = (DURATION_BASE / tempo / fraction * coef(punti)).to_i
          end
        end
        -2 # Neither a note nor a rest
      end
      pitch = if note == -2
        nil
      elsif note == -1
        0
      else
        case str[i + 1]
        when "-"
          if note == 0
            note = 11
            octave_fix = -1
          else
            note -= 1
          end
          i += 1
        when "+"
          note += 1
          i += 1
        end
        # @type var note: Integer
        (6.875 * (2<<(octave + octave_fix)) * 2 ** (note / 12.0)).to_i
      end
        fraction_str, i = number_str(str, i)
        punti, i = count_punto(str, i)
        duration = if 0 < fraction_str.size
        (DURATION_BASE / tempo / fraction_str.to_i * coef(punti)).to_i
      else
        common_duration
      end
      next unless pitch
      if block_given?
        yield(pitch, duration)
      else
        notes << [pitch, duration]
      end
    end
    return notes
  end
end
