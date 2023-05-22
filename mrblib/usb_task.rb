if PICORUBY_MSC == "MSC_SD"
  require "spi"
end
require "keyboard"

ENV = {}

200.times do
  USB.tud_task
  sleep_ms 2
end

print "\e[2J\e[1;1H" # clear all & home

Keyboard.mount_volume

keymap_updated_at = -1

while true
  USB.tud_task
  if Keyboard.autoreload_ready?
    if File.exist?("/keymap.rb")
      unixtime = File::Stat.new("/keymap.rb").mtime.to_i
      if unixtime != keymap_updated_at
        Keyboard.restart
        keymap_updated_at = unixtime
      else
        Keyboard.autoreload_off
      end
    end
  end
end
