#!/bin/env ruby

require 'time'

MYOSP_VERSION   = "1.3"
MYOSP_BUILDNUM  = "1"
MYOSP_TIMESTAMP = Time.new.strftime("%Y%m%d-%H%M%S")

def run_command (cmd)
  puts "Executing: #{cmd}"
  if ! system cmd
    raise "FAILED: #{cmd}"
  end
end

def clean
  run_command "find . -name 'libmyosp*.so*' -exec rm -f {} \\;"
  run_command "find . -name 'libmyosp*.tgz' -exec rm -f {} \\;"
  run_command "find . -name 'myosp*.tgz' -exec rm -f {} \\;"
  run_command "find . -name '*.o' -exec rm -f {} \\;"
  run_command "find . -name '*.a' -exec rm -f {} \\;"
  run_command "find . -name '*.d' -exec rm -f {} \\;"
  run_command "find . -name '*.so' -exec rm -f {} \\;"
  run_command "find . -name '*.so.*' -exec rm -f {} \\;"
end

def get_branch_name
    return `git branch | head -1`[2..99].chomp
end

def write_version_header
    f = File.open("src/mysql/BuildInfo.h", "w")
    f.puts "#define WRAPPER_VERSION \"#{MYOSP_VERSION}.#{MYOSP_BUILDNUM}-#{get_branch_name}\""
    f.puts "#define WRAPPER_BUILD_TSTAMP \"#{MYOSP_TIMESTAMP}\""
    f.close
end

def do_compile (version)
    puts "Building version #{version}"

    # do some minimal cleaning to ensure this really is a clean build every time
    run_command "rm -f libmyosp*"
    run_command "find . -name '*.o' -exec rm -f {} \\;"
    run_command "find . -name '*.d' -exec rm -f {} \\;"
    run_command "find . -name '*.a' -exec rm -f {} \\;"

    run_command "make clean; make #{version}"
    run_command "cp libmyosp* libs/"
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

def get_platform
    if RUBY_PLATFORM.downcase =~ /darwin/
        return "mac"
    elsif RUBY_PLATFORM.downcase =~ /windows/
        return "win"
    else

        # get linux distro name and version (might now work on all distros)
        issue = `cat /etc/issue`.downcase.split

        if issue[0] == 'ubuntu'
            # Ubuntu 12.04 LTS \n \l
            return issue[0] + "-" + issue[1]
        elsif issue[0] == 'centos'
            if ! issue[3].match("(Final)") && ! issue[3].match("(final)")
                # CentOS Linux release 6.0 (Final)
                return issue[0] + "-" + issue[3]
            else
                #CentOS release 6.0 (Final)
                return issue[0] + "-" + issue[2]
            end
        else
            return issue
        end
    end
end

def create_tar

  platform = `cat /etc/issue`
  branch = get_branch_name

  tar_filename = ""
  if branch == "master"
      tar_filename = "myosp-#{MYOSP_VERSION}.#{MYOSP_BUILDNUM}-#{get_platform}-#{MYOSP_TIMESTAMP}.tgz"
  else
      tar_filename = "myosp-#{branch}-#{MYOSP_VERSION}.#{MYOSP_BUILDNUM}-#{get_platform}-#{MYOSP_TIMESTAMP}.tgz"
  end

  # create temp dir
  run_command "rm -rf _temp"
  run_command "mkdir _temp"

  # copy files into temp dir
  run_command "cp libs/lib* _temp"
  run_command "cp ../libopensharding/libopensharding.so.1.0.0 _temp"
  run_command "cp myosp.conf _temp"
  run_command "cp README _temp"

  # create tar
  run_command "tar cvzf #{tar_filename} -C _temp ."

  # remove temp dir
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

