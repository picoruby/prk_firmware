class LoadError < StandardError
  # ScriptError is the super class in CRuby
end

class Object

  alias _puts puts
  def puts(*args)
    tud_task
    args.each do |arg|
      _puts arg
      tud_task
    end
    return nil
  end

  alias _print print
  def print(*args)
    tud_task
    args.each do |arg|
      _print arg
      tud_task
    end
    return nil
  end

end
