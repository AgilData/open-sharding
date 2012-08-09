#!/bin/env ruby

require 'time'

MYOSP_VERSION  = "1.3"
MYOSP_BUILDNUM = "1"

def run_command (cmd)
  puts "Executing: #{cmd}"
  if ! system cmd
    puts "FAILED: #{cmd}"
    return false
  end

  return true
end

def clean
  run_command "find . -name '*.o' -exec rm -f {} \\;"
  run_command "find . -name '*.a' -exec rm -f {} \\;"
  run_command "find . -name '*.d' -exec rm -f {} \\;"
  run_command "find . -name '*.so' -exec rm -f {} \\;"
  run_command "find . -name '*.so.*' -exec rm -f {} \\;"
end

def write_version_header
    tstamp = Time.new.strftime("%Y%m%d-%H%M%S")
    f = File.open("src/mysql/BuildInfo.h", "w")
    f.puts "#define WRAPPER_VERSION \"#{MYOSP_VERSION}.#{MYOSP_BUILDNUM}\""
    f.puts "#define WRAPPER_BUILD_TSTAMP \"#{tstamp}\""
    f.close
end

def do_compile (version)
    puts "Building version #{version}"
    if run_command "make clean; make #{version}"
      run_command "cp libmyosp* libs/"
    else
      puts "BUILD FOR VERSION #{version} FAILED!!!"
      exit
    end
end

def compile
    if !File.exists? "libs"
        run_command "mkdir libs"
    end
    run_command "rm -f libs/libmyosp*"
    do_compile "fifteen"
    do_compile "fifteen_r"
    do_compile "sixteen"
    do_compile "sixteen_r"
end

def create_tar
  platform = `cat /etc/issue`

  run_command "rm -rf _temp"
  run_command "mkdir _temp"
  run_command "cp libs/lib* _temp"
  run_command "cp ../libopensharding/libopensharding.1.0.0 _temp"
  run_command "cp scripts/setup.rb _temp"
  run_command "cp src/myosp.conf _temp"
  run_command "cp src/README.txt _temp"
  run_command "tar cvzf myosp-#{MYOSP_VERSION}.#{MYOSP_BUILDNUM}.tgz -C _temp ."
  run_command "rm -rf _temp"

end

begin
  start = Time.now
  clean
  write_version_header
  compile
  create_tar
  finish = Time.now

  elapsed = (finish-start).to_i
  puts "Compiled in #{elapsed} seconds"
end

