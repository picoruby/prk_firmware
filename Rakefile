require "fileutils"

task :default => :all

desc "build PRK Firmware in build directory"
task :all => %i(check_setup test_all skip_test)

# Build without tests
task :skip_test do
  FileUtils.cd "build" do
    sh "cmake .. && make"
  end
end

desc "build PRK Firmware inclusive of keymap.rb (without mass storage)"
task :build_with_keymap, ['keyboard_name'] => :test_all do |_t, args|
  unless args.keyboard_name
    raise "Argument `keyboard_name` missing.\nUsage: rake build_with_keymap[prk_meishi2]"
  end
  dir = "keyboards/#{args.keyboard_name}/build"
  FileUtils.mkdir_p dir
  FileUtils.cd dir do
    sh "cmake -DPRK_NO_MSC=1 ../../.. && make"
  end
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
task :steep_check do
  FileUtils.cd "src/ruby" do
    sh "rake steep"
  end
end

desc "run unit test for ruby program"
task :mrubyc_test do
  FileUtils.cd "src/ruby" do
    sh "rake test"
  end
end

desc "check whether you need to setup"
task :check_setup do
  begin
    FileUtils.cd "src/ruby" do
      sh "bundle exec steep -h > /dev/null 2>&1", verbose: false
      sh "bundle exec mrubyc-test -h > /dev/null 2>&1", verbose: false
    end
    dir = "lib/picoruby/mrbgems/mruby-mrubyc/repos/mrubyc/src/hal_user_reserved"
    sh "ls #{dir} > /dev/null 2>&1", verbose: false
    sh "ls #{dir}/hal.c > /dev/null 2>&1", verbose: false
    sh "ls #{dir}/hal.h > /dev/null 2>&1", verbose: false
  rescue => e
    puts "You need to do `rake setup`!"
    puts e.message
    exit 1
  end
end

desc "you have to run this task once before build"
task :setup do
  sh "git submodule update --recursive"
  FileUtils.cd "src/ruby" do
    sh "bundle exec steep -h || bundle install"
  end
  FileUtils.cd "lib/picoruby" do
    sh "rake all"
  end
  FileUtils.cd "lib/picoruby/mrbgems/mruby-mrubyc/repos/mrubyc/src/hal_user_reserved" do
    FileUtils.ln_sf "../../../../../../../hal/hal.c", "hal.c"
    FileUtils.ln_sf "../../../../../../../hal/hal.h", "hal.h"
  end
end

desc "clean built"
task :clean do
  FileUtils.cd "build" do
    sh "rm -rf *"
  end
end

desc "clean everything (Then you'll need to do `rake setup`"
task :deep_clean do
  FileUtils.rm_r Dir.glob("build/*")
  FileUtils.rm_r "lib/picoruby"
end

# Add a new tag then push it
task :release => :test_all do
  regexp = /(set \(PRK_VERSION\s+)([\d\.]+)/
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
  current_version = `cat CMakeLists.txt`.match(regexp)[2]
  unless current_version
    puts "No PRK_VERSION was found in CMakeLists.txt!"
    exit 1
  end
  tags = `git tag`.chomp.split("\n").sort_by{ |v| Gem::Version.new(v) }
  if tags.last != current_version
    puts "Inconsistent between PRK_VERSION and the newest tag!"
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
  buffer = File.read()
  File.write("CMakeLists.txt", buffer.sub(regexp, "\\1#{new_version}"))
  sh "git add CMakeLists.txt"
  sh "git commit -m 'new version: #{new_version}'"
  sh "git tag #{new_version}"
  sh "git push origin #{new_version}"
  Rake::Task['clean'].invoke
  Rake::Task['skip_test'].invoke
end
