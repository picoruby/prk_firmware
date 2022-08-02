class LoadError < StandardError
  # ScriptError is the super class in CRuby
end

class Object

  def require(feature)
    $LOADED_FEATURES ||= Array.new
    if required?(feature)
      false
    elsif _require(feature)
      $LOADED_FEATURES << feature
      true
    else
      raise LoadError, "cannot load such gem -- #{feature}"
    end
  end

  def required?(feature)
    $LOADED_FEATURES&.include?(feature)
  end

  alias _puts puts
  def puts(*args)
    tud_task
    cdc_task
    args.each do |arg|
      _puts arg
      tud_task
      cdc_task
    end
    return nil
  end

  alias _print print
  def print(*args)
    tud_task
    cdc_task
    args.each do |arg|
      _print arg
      tud_task
      cdc_task
    end
    return nil
  end

end
