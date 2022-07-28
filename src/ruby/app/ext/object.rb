class Object

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

end
