# frozen_string_literal: true

require "json"
require "fileutils"

require 'pathname'
require 'steep'
require 'steep/cli'

def hello(event:, context:)
  {
    statusCode: 200,
    body: { message: "Hello" }.to_json
  }
end

=begin
curl -X POST http://localhost:4000/offline/steep \
 -H 'Content-Type: application/json' \
 -d '{"keymap":"Keyboard.new"}'
=end
MAX_KEYMAP_SIZE = 10 # KB
def steep(event:, context:)
  prepare unless @prepared
  body = JSON.parse(event["body"])
  File.open("/tmp/steep/keymap.rb", "w") do |f|
    f.write body["keymap"]
  end
  if MAX_KEYMAP_SIZE * 1024 < FileTest.size("/tmp/steep/keymap.rb")
    {
      statusCode: 200,
      headers: headers,
      body: {
        message: "keymap.rb is too big. It should be less than #{MAX_KEYMAP_SIZE} KB.",
      }.to_json
    }
  else
    result = _steep_check
    {
      statusCode: 200,
      headers: headers,
      body: {
        message: result,
      }.to_json
    }
  end
end

def _steep_check
  begin
    stdout = File.open("/tmp/steep/stdout", "w+")
    argv = [
      "check",
      "--steepfile=/tmp/steep/Steepfile",
    ]
    if ENV['STAGE'] == "prod"
      argv << "--steep-command=#{ENV['GEM_PATH']}/bin/steep"
    end
    Steep::CLI.new(argv: argv, stdout: stdout, stderr: STDERR, stdin: STDIN).run
  rescue => exn
    STDERR.puts exn.inspect
    exn.backtrace.each do |t|
      STDERR.puts "  #{t}"
    end
    exit 2
  ensure
    stdout.rewind
    result = stdout.read
    stdout.close
    return result
  end
end

def headers
  {
    "Access-Control-Allow-Origin" => "*",
    "Access-Control-Allow-Credentials" => true
  }
end

def prepare
  FileUtils.mkdir_p ["/tmp/steep/sig", "/tmp/steep/app"]
  Dir.glob("steep/sig/*.rbs").each do |f|
    FileUtils.ln_sf "#{Dir.pwd}/#{f}", "/tmp/steep/sig/"
  end
  FileUtils.ln_sf "#{Dir.pwd}/steep/Steepfile", "/tmp/steep/Steepfile"
  @prepared = true
end
