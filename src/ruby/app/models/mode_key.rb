class ModeKey

  attr_reader :layer
  attr_reader :on_release
  attr_reader :on_hold
  attr_reader :release_threshold
  attr_reader :repush_threshold
  attr_reader :switch
  attr_accessor :prev_state
  attr_accessor :pushed_at
  attr_accessor :released_at

  def initialize(layer, on_release, on_hold, release_threshold, repush_threshold, row_index, col_index)
    @layer = layer
    @on_release = on_release
    @on_hold = on_hold
    @release_threshold = (release_threshold || 0)
    @repush_threshold =  (repush_threshold || 0)
    @switch = [row_index, col_index]
    @prev_state = :released
    @pushed_at = 0
    @released_at = 0
  end
end
