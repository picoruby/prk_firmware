class MmlTest < MrubycTestCase
  def setup
    @data = Array.new
  end

  #########################################

  description "Just an A"
  def basic_case
    duration = MML.new.compile("a") { |p, d| @data << [p, d] }
    assert_equal [[440, 500]], @data
    assert_equal 500, duration
  end

  description "A flat"
  def a_flat_case
    duration = MML.new.compile("AA-") { |p, d| @data << [p, d] }
    puts @data
    assert_equal [[440, 500],[415.3046975799451, 500]], @data
    assert_equal 1000, duration
  end

  description "C flat"
  def c_flat_case
    duration = MML.new.compile("CC-") { |p, d| @data << [p, d] }
    assert_equal [[261.6255653005986, 500], [246.94165062806206, 500]], @data
    assert_equal 1000, duration
  end

  description "Change tempo"
  def tempo_case
    duration = MML.new.compile("T240a") { |p, d| @data << [p, d] }
    assert_equal [[440, 250]], @data
    assert_equal 250, duration
  end

  description "Rest octave"
  def rest_case
    duration = MML.new.compile("ara") { |p, d| @data << [p, d] }
    assert_equal [[440,500],[0,500],[440,500]], @data
    assert_equal 1500, duration
  end

  description "Specify octave"
  def specify_octave_case
    duration = MML.new.compile("O5aO3a") { |p, d| @data << [p, d] }
    assert_equal [[880,500],[220,500]], @data
    assert_equal 1000, duration
  end

  description "Decrease octave"
  def decrease_octave_case
    duration = MML.new.compile("a>a") { |p, d| @data << [p, d] }
    assert_equal [[440,500],[220,500]], @data
    assert_equal 1000, duration
  end

  description "Increase octave"
  def increase_octave_case
    duration = MML.new.compile("a<a") { |p, d| @data << [p, d] }
    assert_equal [[440,500],[880,500]], @data
    assert_equal 1000, duration
  end

  description "Specify length"
  def specify_length_case
    duration = MML.new.compile("a2") { |p, d| @data << [p, d] }
    assert_equal [[440,1000]], @data
    assert_equal 1000, duration
  end

  description "Dotted note without length specified"
  def dotted_without_length_note_case
    duration = MML.new.compile("a.a") { |p, d| @data << [p, d] }
    assert_equal [[440,750],[440,500]], @data
    assert_equal 1250, duration
  end

  description "Dotted note with length specified"
  def dotted_with_length_note_case
    duration = MML.new.compile("a4.") { |p, d| @data << [p, d] }
    assert_equal [[440,750]], @data
    assert_equal 750, duration
  end

  description "Specify Q value"
  def q_1_case
    duration = MML.new.compile("Q4aQ8a") { |p, d| @data << [p, d] }
    assert_equal [[440,250],[0,250],[440,500]], @data
    assert_equal 1000, duration
  end

  description "Rest after Q note"
  def q_2_case
    duration = MML.new.compile("Q4a") { |p, d| @data << [p, d] }
    assert_equal [[440,250],[0,250]], @data
    assert_equal 500, duration
  end

  description "Q value of a rest"
  def q_rest_case
    duration = MML.new.compile("Q4r") { |p, d| @data << [p, d] }
    assert_equal [[0,500]], @data
    assert_equal 500, duration
  end

  description "Q rest and rest should unite"
  def q_note_following_rest_case
    duration = MML.new.compile("Q4ar") { |p, d| @data << [p, d] }
    assert_equal [[440,250],[0,750]], @data
    assert_equal 1000, duration
  end

  description "Super Mario A part"
  def mario_a_case
    [
      "T200 L8 O5 Q4 e e r e r c e r g r4. > g r4.",
      "T200 L8 Q6 < c r4 > g r4 e r4 a r b r b- a r",
      "T200 L8 Q6 g6 < e6 g6 a r f g r e r c d > b r4",
      "T200 L8 Q6 r4 l< g g- f d+ r e r > g+ a < c r > a < c d",
      "T200 L8 Q6 r4   g g- f d+ r e r < c r c c r4.",
      "T200 L8 Q6 r4 > g g- f d+ r e r > g+ a < c r > a < c d",
      "T200 L8 Q6 r4 e-8 r4 d r4 c r r4 r2"
    ].each do |measure|
      duration = MML.new.compile(measure) { |p, d| @data << [p, d] }
      assert_equal 2400, duration
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
      duration = MML.new.compile(measure) { |p, d| @data << [p, d] }
      assert_equal 2400, duration
    end
  end

  description "Danger Zone"
  def danger_zone_case
    [
      "T120 Q6 L8 r O2 c d f g4 f4 < g a r4 a4. a r4 a a a4 a4 g a r4 a4. a r4 a a < c4 c4 > Q8 g1 Q6 g1 g a r4 a4. a r4 a a a4 a4 g a r4 a4. a r4 a a a4 a4",
      "T120 Q6 L8 r O2 c d f g4 f4 d << d r4 d4. d >> r c d f g4 f4 d << d r4 d4. d >> r c d f g4 f4 c4. > g < c2 d c4 > g < c4 c4 d4. d d2 r c d f g4 f4 d4. d d2 r c d f g4 f4"
    ].each do |measure|
      duration = MML.new.compile(measure) { |p, d| @data << [p, d] }
      assert_equal 22000, duration
    end
  end
end
