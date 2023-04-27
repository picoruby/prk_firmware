require "textbringer"

print "static const uint16_t tcode_table[1600] = {\n" + Textbringer::TCodeInputMethod::KANJI_TABLE.map { |row|
  "  " + row.each_codepoint.map { |c|
    "0x%04X" % c
  }.join(", ")
}.join(",\n") + "\n};\n"
