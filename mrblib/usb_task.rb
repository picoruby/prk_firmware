require "machine"
require "watchdog"
Watchdog.disable
require 'env'

ENV["HOME"] = "/"
ENV["PWD"] = "/"

STDOUT = IO
STDIN = IO
STDIN.echo = false

if PICORUBY_MSC == "MSC_SD"
  require "spi"
end
require "keyboard"

200.times do
  USB.tud_task
  sleep_ms 2
end

print "\e[2J\e[1;1H" # clear all & home

Keyboard.mount_volume

while true
  USB.tud_task
  if Keyboard.autoreload_ready? && File.exist?("/keymap.rb")
    break unless Keyboard.restart
  end
end

puts "Restart microcontroller when you want to reload keymap.rb"
while true
  USB.tud_task
end
