unless PRK_NO_MSC
  require "filesystem-fat"
  require "vfs"
end
require "keyboard"

README = "/README.txt"
KEYMAP = "/keymap.rb"

unless PRK_NO_MSC
  fat = FAT.new(:flash, "PRKFirmware")
  VFS.mount(fat, "/")
  unless File.exist?(README)
    File.open(README, "w") do |f|
      f.puts PRK_DESCRIPTION,
        "\nWelcome to PRK Firmware!\n"
        "Usage:",
        "- Drag and drop your `keymap.rb` into this directory",
        "- Then, your keyboard will be automatically rebooted. That's all!\n"
        "Notice:",
        "- Make sure you always have a backup of your `keymap.rb`",
        "  because upgrading prk_firmware-*.uf2 may remove it from flash\n",
        "https://github.com/picoruby/prk_firmware"
    end
  end
  class Keyboard
    def self.reload_keymap
      @keymap_timestamp ||= 0
      if File.exist?(KEYMAP)
        # TODO File.stat has to return timestamp
        if @keymap_timestamp < File.stat(KEYMAP)[:timestamp]
          keymap = ""
          File.open(KEYMAP) do |f|
            f.each_line do |line|
              keymap << line
            end
          end
          Task[:keyboard].reload(keymap)
          10.times do
            USB.tud_task
            sleep_ms 2
          end
          Task[:keyboard].resume
        end
      end
    end
  end
end

200.times do
  USB.tud_task
  sleep_ms 2
end
print "\e[2J\e[1;1H" # clear all & home
puts "==============================================="
puts PRK_DESCRIPTION
puts "==============================================="

autoreload_tick = 0

$mutex = false

while true
  USB.tud_task
  if Keyboard.autoreload_ready?
    if autoreload_tick == 0
      puts "\nReloading keymap"
      $rgb.turn_off if $rgb
      #Keyboard.suspend_keymap
      Task[:keyboad].suspend
      USB.hid_task(0, "\000\000\000\000\000\000", 0, 0, 0)
      autoreload_tick = 200
    elsif autoreload_tick == 1
      Keyboard.reload_keymap
    end
    autoreload_tick -= 1
    USB.tud_task
    sleep_ms 2
  end
end
