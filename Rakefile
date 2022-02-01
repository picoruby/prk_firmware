require "fileutils"

task :default => :all

desc "build PRK Firmware in build directory"
task :all => %i(check_setup test_all skip_test)

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
    dir = "lib/picoruby/build/repos/host/mruby-mrubyc/repos/mrubyc/src/hal_user_reserved"
    sh "ls #{dir} > /dev/null 2>&1", verbose: false
    sh "ls #{dir}/hal.c > /dev/null 2>&1", verbose: false
    sh "ls #{dir}/hal.h > /dev/null 2>&1", verbose: false
  rescue
    puts "You need to do `rake setup`!"
    exit 1
  end
end

desc "you have to run this task once before build"
task :setup do
  FileUtils.cd "src/ruby" do
    sh "bundle install"
  end
  FileUtils.cd "lib/picoruby" do
    sh "rake all"
  end
  FileUtils.cd "lib/picoruby/build/repos/host/mruby-mrubyc/repos/mrubyc/src/hal_user_reserved" do
    FileUtils.ln_sf "../../../../../../../../../hal/hal.c", "hal.c"
    FileUtils.ln_sf "../../../../../../../../../hal/hal.h", "hal.h"
  end
end

desc "clean built"
task :clean do
  FileUtils.rm_r Dir.glob("build/*")
end

desc "clean everything (Then you'll need to do `rake setup`"
task :deep_clean => :clean do
  FileUtils.cd "lib/picoruby" do
    sh "rake deep_clean"
  end
end
