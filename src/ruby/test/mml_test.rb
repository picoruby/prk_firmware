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
  def q_case
    assert_equal [[440,250],[0,250],[440,500]], compile("Q4aQ8a")
    assert_equal [[440,250],[0,250],[440,500]], compile_without_block("Q4aQ8a")
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
end
