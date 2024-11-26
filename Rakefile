require "fileutils"

PICO_SDK_TAG = "2.1.0"

def mruby_config
  case ENV['BOARD']&.downcase
  when 'pico2'
    'prk_firmware-cortex-m33'
  else
    'prk_firmware-cortex-m0plus'
  end
end

def cmake_flags
  flags = []
  case ENV['BOARD']&.downcase
  when 'pico2'
    flags << "PICO2=yes"
  end
  flags.join(" ")
end

def def_board
  case ENV['BOARD']&.downcase
  when 'pico2'
    '-DPICO_PLATFORM=rp2350 -DPICO_BOARD=pico2'
  else
    '-DPICO_PLATFORM=rp2040 -DPICO_BOARD=pico'
  end
end

def build_dir
  dir = case ENV['BOARD']&.downcase
  when 'pico2'
    "#{ENV['WITH_KEYMAP_DIR']}build_pico2"
  else
    "#{ENV['WITH_KEYMAP_DIR']}build_pico"
  end
  FileUtils.mkdir_p dir
  dir
end

task :default do
  puts "Specify a task:"
  puts "  rake pico       # build for RP2040"
  puts "  rake pico2      # build for RP2350"
end

task :setup do
  sh "bundle install"
  sh "git submodule update --init --recursive"
  FileUtils.cd "lib/picoruby" do
    sh "bundle install"
  end
end

task :all => [:libmruby, :cmake, :build]

desc "build debug for RP2040 (you may need to rake clean before this)"
task :pico_debug do
  ENV['PICORUBY_DEBUG'] = '1'
  ENV['BOARD'] = 'pico'
  ENV['CMAKE_BUILD_TYPE'] = 'Debug'
  Rake::Task[:all].invoke
end

desc "build production for RP2040"
task :pico do
  ENV['BOARD'] = 'pico'
  ENV['CMAKE_BUILD_TYPE'] = 'Release'
  Rake::Task[:all].invoke
end

desc "build debug for RP2350 (you may need to rake clean before this)"
task :pico2_debug do
  ENV['PICORUBY_DEBUG'] = '1'
  ENV['BOARD'] = 'pico2'
  ENV['CMAKE_BUILD_TYPE'] = 'Debug'
  Rake::Task[:all].invoke
end

desc "build production for RP2350"
task :pico2 do
  ENV['BOARD'] = 'pico2'
  ENV['CMAKE_BUILD_TYPE'] = 'Release'
  Rake::Task[:all].invoke
end

def with_keymap(board)
  ENV['BOARD'] = board
  ENV['PICORUBY_NO_MSC'] = '1'
  ENV['WITH_KEYMAP_DIR'] = "keyboards/#{args.keyboard_name}/"
  Rake::Task[:all].invoke
end

desc "build PRK Firmware inclusive of keymap.rb (without mass storage) for RP2040"
task :build_with_keymap_pico, ['keyboard_name'] do |_t, args|
  unless args.keyboard_name
    raise "Argument `keyboard_name` missing.\nUsage: rake build_with_keymap[prk_meishi2]"
  end
  with_keymap('pico')
end

desc "build PRK Firmware inclusive of keymap.rb (without mass storage) for RP2350 (Pico2)"
task :build_with_keymap_pico2, ['keyboard_name'] do |_t, args|
  unless args.keyboard_name
    raise "Argument `keyboard_name` missing.\nUsage: rake build_with_keymap[prk_meishi2]"
  end
  with_keymap('pico2')
end

desc "build production with SQLite3 and SD card for RP2350 (Pico2)"
task :sqlite3 do
  ENV['PICORUBY_SQLITE3'] = '1'
  ENV['PICORUBY_SD_CARD'] = '1'
  ENV['PICORUBY_MSC_SD'] = '1'
  ENV['BOARD'] = 'pico2'
  Rake::Task[:all].invoke
end

file "lib/picoruby" do
  sh "git submodule update --init --recursive"
end

task :libmruby => "lib/picoruby" do
  FileUtils.cd "lib/picoruby" do
    sh "MRUBY_CONFIG=default rake test"
    sh "MRUBY_CONFIG=#{mruby_config} rake"
  end
end

task :cmake do
  sh "#{cmake_flags} cmake #{def_board} -DCMAKE_BUILD_TYPE=#{ENV['CMAKE_BUILD_TYPE']} -B #{build_dir}"
end

task :check_pico_sdk => :check_pico_sdk_path do
  FileUtils.cd ENV["PICO_SDK_PATH"] do
    unless `git describe --tags`.strip.end_with?(PICO_SDK_TAG)
      raise <<~MSG
        pico-sdk #{PICO_SDK_TAG} is not checked out!\n
        Tips for dealing with:\n
        cd $PICO_SDK_PATH && git pull && git checkout #{PICO_SDK_TAG} && git submodule update --recursive\n
      MSG
    end
  end
end

task :check_pico_sdk_path do
  unless ENV["PICO_SDK_PATH"]
    raise <<~MSG
      Environment variable `PICO_SDK_PATH` does not exist!
    MSG
  end
end

desc "build without cmake preparation"
task :build => :check_pico_sdk do
  sh "cmake --build #{build_dir}"
end

desc "clean built"
task :clean do
  FileUtils.cd "lib/picoruby" do
    sh "rake clean"
  end
  %(build_pico build_pico2).each do |dir|
    FileUtils.cd dir do
      FileUtils.rm_rf Dir.glob("prk_firmware-*.*")
    end
    begin
      sh "cmake --build #{dir} --target clean"
    rescue => e
      puts "Ignoring an error: #{e.message}"
    end
  end
end

desc "deep clean built"
task :deep_clean do
  FileUtils.cd "lib/picoruby" do
    sh "rake deep_clean"
  end
  %(build_pico build_pico2).each do |dir|
    FileUtils.cd dir do
      FileUtils.rm_rf Dir.glob("*")
    end
  end
end

desc "create symlinks for develop-convenience"
task :symlinks do
  Dir.glob("lib/picoruby/mrbgems/picoruby-prk-*").each do |src|
    FileUtils.ln_sf File.join("..", src), "symlinks/#{File.basename(src)}"
  end
end

# Add a new tag then push it
task :release do
  git_status = `git status`
  branch = git_status.split("\n")[0].match(/\AOn branch (.+)\z/)[1]
  if branch != "master"
    puts "You should run `rake release` on master branch!"
    puts "'#{branch}' branch is checked out now."
    exit 1
  end
  unless git_status.include?("up to date")
    puts "Your master branch is not up to date!"
    puts "Might have to git pull or push."
    exit 1
  end
  unless git_status.include?("nothing to commit")
    puts "You still have things to commit!"
    exit 1
  end
  puts "Existing tags:"
  puts tags
  puts
  new_version = Gem::Version.new("#{current_version}.1").bump
  line = ''
  loop do
    loop do
      print "Do you want to add a new tag '#{new_version}'? [y/n/another tag]: "
      line = STDIN.gets.chomp
      break unless line.size.zero?
    end
    if %w(n N).include?(line)
      puts "abort"
      exit 1
    end
    break if %w(y Y).include?(line)
    new_version = line
  end
  sh "git tag #{new_version}"
  sh "git push origin #{new_version}"
  Rake::Task['clean'].invoke
  Rake::Task['all'].invoke
end
