target :lib do
  library "io-console"

  signature "sig"
  Dir.glob("lib/picoruby/mrbgems/**/sig/*.rbs").each { |rb| signature rb }

  check "mrblib"
  Dir.glob([
    "lib/picoruby/mrbgems/**/mrblib/*.rb",
    "lib/picoruby/mrbgems/**/task/*.rb"
  ]).each { |rb| check rb }

  ignore "lib/picoruby/mrbgems/picoruby-mrubyc/repos/mrubyc/mrblib/*.rb"
  ignore "lib/picoruby/mrbgems/picoruby-vfs/mrblib/file.rb"
  ignore "lib/picoruby/mrbgems/picoruby-vfs/mrblib/dir.rb"
  ignore "lib/picoruby/mrbgems/picoruby-shell/mrblib/0_out_of_steep.rb"
  ignore "mrblib/object-ext.rb"
end
