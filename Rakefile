require "fileutils"

MRUBY_CONFIG = "prk_firmware"

task :default => :all

desc "build production"
task :all => [:check_setup, :libmruby, :test_all, :cmake_production, :build]

desc "build debug (you may need to rake clean before this)"
task :debug => [:check_setup, :libmruby, :test_all, :cmake_debug, :build]

file "lib/picoruby" do
  sh "git submodule update --init --recursive"
end

task :libmruby => "lib/picoruby" do
  FileUtils.cd "lib/picoruby" do
    sh "MRUBY_CONFIG=#{MRUBY_CONFIG} rake"
  end
end

task :cmake_debug do
  sh "cmake -DCMAKE_BUILD_TYPE=Debug -B build"
end

task :cmake_production do
  sh "cmake -B build"
end

desc "build without cmake preparation"
task :build do
  sh "cmake --build build"
end

file "src/ruby/app/models/buffer.rb" do
  FileUtils.cd "src/ruby/app/models" do
    FileUtils.ln_sf "../../../../lib/picoruby/mrbgems/picoruby-terminal/mrblib/buffer.rb", "buffer.rb"
  end
end

file "src/ruby/sig/buffer.rbs" do
  FileUtils.cd "src/ruby/sig" do
    FileUtils.ln_sf "../../../lib/picoruby/mrbgems/picoruby-terminal/sig/buffer.rbs", "buffer.rbs"
  end
end

desc "build PRK Firmware inclusive of keymap.rb (without mass storage)"
task :build_with_keymap, ['keyboard_name'] => :test_all do |_t, args|
  unless args.keyboard_name
    raise "Argument `keyboard_name` missing.\nUsage: rake build_with_keymap[prk_meishi2]"
  end
  dir = "keyboards/#{args.keyboard_name}"
  FileUtils.mkdir_p "#{dir}/build"
  sh "cmake -DPRK_NO_MSC=1 -B #{dir}/build"
  sh "cmake --build #{dir}/build"
end

desc "clean built that includes keymap"
task :clean_with_keymap , ['keyboard_name'] do |_t, args|
  unless args.keyboard_name
    raise "Argument `keyboard_name` missing.\nUsage: rake clean_with_keymap[prk_meishi2]"
  end
  FileUtils.rm_r Dir.glob("keyboards/#{args.keyboard_name}/build/*")
end


desc "run :steep_check and :mrubyc_test"
task :test_all => %i(steep_check mrubyc_test)

desc "run steep check for ruby program"
task :steep_check => ["src/ruby/app/models/buffer.rb", "src/ruby/sig/buffer.rbs"] do
#  FileUtils.cd "src/ruby" do
#    sh "rake steep"
#  end
end

task :setup do
  sh "git submodule update"
  FileUtils.cd "lib/picoruby" do
    sh "rake"
  end
  FileUtils.cd "src/ruby" do
    sh "bundle install"
  end
  FileUtils.cd "src/ruby/test/tmp" do
    FileUtils.ln_sf "../../../../lib/picoruby/mrbgems/picoruby-mrubyc/repos/mrubyc/src/hal_posix", "hal"
  end
end

desc "run unit test for ruby program"
task :mrubyc_test do
#  FileUtils.cd "src/ruby" do
#    sh "rake test"
#  end
end

desc "check whether you need to setup"
task :check_setup do
  count = 0
  begin
    FileUtils.cd "src/ruby" do
      sh "bundle exec steep -h > /dev/null 2>&1", verbose: false
      sh "bundle exec mrubyc-test -h > /dev/null 2>&1", verbose: false
      sh "ls test/tmp/hal/ > /dev/null 2>&1", verbose: false
    end
  rescue => e
    if 0 == count
      count += 1
      puts "You may need `rake setup`, let me try once"
      Rake::Task['setup'].invoke
      retry
    else
      puts e.message
      exit 1
    end
  end
end

desc "clean built"
task :clean do
  FileUtils.cd "lib/picoruby" do
    sh "MRUBY_CONFIG=#{MRUBY_CONFIG} rake clean"
  end
  FileUtils.cd "build" do
    FileUtils.rm_rf Dir.glob("prk_firmware-*.*")
  end
  begin
    sh "cmake --build build --target clean"
  rescue => e
    puts "Ignoring an error: #{e.message}"
  end
end

desc "deep clean built"
task :deep_clean do
  FileUtils.cd "lib/picoruby" do
    sh "MRUBY_CONFIG=#{MRUBY_CONFIG} rake deep_clean"
  end
  FileUtils.cd "build" do
    FileUtils.rm_rf Dir.glob("*")
  end
end

# Add a new tag then push it
task :release => :test_all do
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
  Rake::Task['skip_test'].invoke
end
