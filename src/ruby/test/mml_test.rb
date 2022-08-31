class MmlTest < MrubycTestCase
  def compile(*measures)
    mml = MML.new
    result = Array.new
    measures.each do |measure|
      mml.compile measure do |p, d|
        result << [p, d]
      end
    end
    result
  end

  def compile_without_block(*measures)
    mml = MML.new
    result = Array.new
    measures.each do |measure|
      result += mml.compile(measure)
    end
    result
  end

  #########################################

  description "Just an A"
  def basic_case
    assert_equal [[440, 500]], compile("a")
  end

  description "A flat"
  def a_flat_case
    assert_equal [[440, 500],[415,500]], compile("AA-")
  end

  description "C flat"
  def c_flat_case
    assert_equal [[261, 500],[246,500]], compile("CC-")
  end

  description "Change tempo"
  def tempo_case
    assert_equal [[440, 250]], compile("T240a")
  end

  description "Rest octave"
  def rest_case
    assert_equal [[440,500],[0,500],[440,500]], compile("ara")
  end

  description "Specify octave"
  def specify_octave_case
    assert_equal [[880,500],[220,500]], compile("O5aO3a")
  end

  description "Decrease octave"
  def decrease_octave_case
    assert_equal [[440,500],[220,500]], compile("a>a")
  end

  description "Increase octave"
  def increase_octave_case
    assert_equal [[440,500],[880,500]], compile("a<a")
  end

  description "Specify length"
  def specify_length_case
    assert_equal [[440,1000]], compile("a2")
  end

  description "Dotted note without length specified"
  def dotted_without_length_note_case
    assert_equal [[440,750],[440,500]], compile("a.a")
  end

  description "Dotted note with length specified"
  def dotted_with_length_note_case
    assert_equal [[440,750]], compile("a4.")
  end

  description "Specify Q value"
  def q_1_case
    assert_equal [[440,250],[0,250],[440,500]], compile("Q4aQ8a")
  end

  description "Specify Q value"
  def q_1_without_block_case
    assert_equal [[440,250],[0,250],[440,500]], compile_without_block("Q4aQ8a")
  end

  description "Rest after Q note"
  def q_2_case
    assert_equal [[440,250],[0,250]], compile("Q4a")
  end

  description "Rest after Q note"
  def q_2_without_block_case
    assert_equal [[440,250],[0,250]], compile_without_block("Q4a")
  end

  description "Q value of a rest"
  def q_rest_case
    assert_equal [[0,500]], compile("Q4r")
  end

  description "Q rest and rest should unite"
  def q_note_following_rest_case
    assert_equal [[440,250],[0,750]], compile("Q4ar")
    assert_equal [[440,250],[0,750]], compile_without_block("Q4ar")
  end

  description "Super Mario A part"
  def mario_a_case
    [
      #"T200 L8 O5 Q4 e e r e r c e r g r4. > g r4.",
      #"T200 L8 Q6 < c r4 > g r4 e r4 a r b r b- a r",
      #"T200 L8 Q6 g6 < e6 g6 a r f g r e r c d > b r4",
      "T200 L8 Q6 r4 < g g- f d+ r e",
      "T200 L8 Q6 r > g+ a < c r > a < c d",
      #"T200 L8 Q6 r4   g g- f d+ r e r < c r c c r4.",
      "T200 L8 Q6 r4 > g g- f d+ r e",
      "T200 L8 Q6 r > g+ a < c r > a < c d",
      #"T200 L8 Q6 r4 e-8 r4 d r4 c r r4 r2"
    ].each do |measure|
      duration = 0
      p compile(measure).each { |note| duration += note[1] }
      assert_equal 1200, duration
    end
  end

  description "Super Mario B part"
  def mario_b_case
    [
      "T200 L8 O3 Q4 d d r d r d d r g r4. > g r4.",
      "T200 L8 Q6 < g r4 e r4 c r4 f r g r g- f r",
      "T200 L8 Q6 e6 < c6 e6 f r d e r c r > a b g r4",
      "T200 L8 Q6 c r4 g r4 < c r > f r4 < c r4 > f r",
      "T200 L8 Q6 c r4 e r4 g < c r < f r f f r >> g r",
      "T200 L8 Q6 c r4 g r4 < c r > f r4 < c r4 > f r",
      "T200 L8 Q6 c r a- r4 b- r4 < c r4 > g g r c r"
    ].each do |measure|
      duration = 0
      compile(measure).each { |note| duration += note[1] }
      assert_equal 2400, duration
    end
  end

  description "Danger Zone"
  def danger_zone_case
    [
      "T120 Q6 L8 r O2 c d f g4 f4 < g a r4 a4. a r4 a a a4 a4 g a r4 a4. a r4 a a < c4 c4 > Q8 g1 Q6 g1 g a r4 a4. a r4 a a a4 a4 g a r4 a4. a r4 a a a4 a4",
      "T120 Q6 L8 r O2 c d f g4 f4 d << d r4 d4. d >> r c d f g4 f4 d << d r4 d4. d >> r c d f g4 f4 c4. > g < c2 d c4 > g < c4 c4 d4. d d2 r c d f g4 f4 d4. d d2 r c d f g4 f4"
    ].each do |measure|
      duration = 0
      compile(measure).each { |note| duration += note[1] }
      assert_equal 22000, duration
    end
  end
end
