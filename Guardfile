guard :process, command: %w(bundle exec steep check) do
  watch %r{lib/picoruby/mrbgems/[^/]+/(mrblib|task|sig)/.+\.(rb|rbs)}
  watch %r{(mrblib|sig)/.+\.(rb|rbs)}
end
