#!/bin/env ruby

#################################################################################################
###
### libmyosp Build Script
###
#################################################################################################

#################################################################################################
## IMPORTS
#################################################################################################

require 'fileutils'

#################################################################################################
## Constants
#################################################################################################

BRANCH = "master"
REPOSITORY = "git://git.assembla.com/open-sharding.git"
BRANCH_LIST = ["HEAD", "logging_changes", "osp-virtual-hosts","master", "myospbinlog", "mysql_compatibility", "osp-client-host-list", "threaded_fifo", "windows"]
BOOST_VER = "1.38.0"
BOOST_DIR = "boost_1_38_0"
BOOST_TAR = "boost_1_38_0.tar.gz"
BOOST_DOWNLOAD = "http://downloads.sourceforge.net/project/boost/boost/#{BOOST_VER}/#{BOOST_TAR}"
INIT_DIR = Dir.pwd
OS_DIR = "#{INIT_DIR}"
OPERSYS = "ubuntu1104" # update this to set the build operating system
ARCH = "x64" # update this to set the build architecture
LIBMYOSP_VER = "pre-release-1.1.113" # update this to set the libmyosp version
LIBMYOSP_TAR = "libmyosp-#{OPERSYS}-#{ARCH}-#{LIBMYOSP_VER}.tar.gz"

#################################################################################################
## Run an operating system command
#################################################################################################
def run_command(cmd)
    puts "Executing: #{cmd}"
    if ! system cmd
        puts "FAILED to execute #{cmd}"
        exit 1
    end
end

#################################################################################################
## Pull Opensharding Branch
#################################################################################################
def clone_rep(branch_name)
  # This method depends on having the repo checked out already in the current folder.
    puts "Cloning Opensharding Repositories"
    if !File.exists? "open-sharding"
        puts "WARN: open-sharding Not Found, aborting."
        return
    end
    cmd = "git pull origin #{branch_name}"
    run_command cmd
end

#################################################################################################
## Building libmyosp
#################################################################################################
def build(branch_name)
    check_dep
=begin
    puts "Building from branch #{branch_name}"
    clone_rep branch_name
    puts "Downloading Boost Libraries"
    if RUBY_PLATFORM.downcase.include?("darwin")
        if !File.exists? "#{BOOST_DIR}"
            run_command "curl -L #{BOOST_DOWNLOAD} | tar xz"
        end
    else
        if !File.exists? "#{BOOST_TAR}"
            run_command "wget #{BOOST_DOWNLOAD}"
        end
        if !File.exists? "#{BOOST_DIR}"
            run_command "tar xzf #{BOOST_TAR}"
        end
    end
    puts "Building and Installing Boost Libraries"
    Dir.chdir BOOST_DIR
    run_command "./configure"
    run_command "sudo make install"
    Dir.chdir OS_DIR

    puts "Building and installing libopensharding"
    Dir.chdir "libopensharding"
    run_command "make clean"
    run_command "make"
    run_command "sudo make install"
    Dir.chdir OS_DIR
=end
    puts "Building libmyosp"
    Dir.chdir "libmyospfacade"
    run_command "ant"
    puts "Creating libmyosp tarball"
    run_command "./scripts/mktarball.sh #{LIBMYOSP_TAR} libs"
    puts "Placing tarball in #{INIT_DIR}"
    run_command "mv #{LIBMYOSP_TAR} #{INIT_DIR}"
    Dir.chdir INIT_DIR
end

#################################################################################################
## Check Dependencies
#################################################################################################
def check_dep
    puts "Checking Dependencies"
    
    print "Make ... "
    x = `which make`.strip
    if x == ""
        puts ""
        puts "Error: Make is missing"
        exit 1
    end
    puts "Found"
    
    print "G++ ... "
    x = `which g++`.strip
    if x == ""
        puts ""
        puts "Error: G++ is missing"
        exit 1
    end
    puts "Found"
    
    print "Git ... "
    x = `which git`.strip
    if x == ""
        puts ""
        puts "Error: Git is missing"
        exit 1
    end
    puts "Found"
    
    print "Python-dev ... "
    x = `find /usr/include -name python*`.strip
    if x == ""
        puts ""
        puts "Error: Python-dev is missing"
        exit 1
    end
    puts "Found"
    
    print "Java ... "
    x = `which java`.strip
    if x == ""
        puts ""
        puts "Error: Java is missing"
        exit 1
    end
    puts "Found"
    
    print "Ant ... "
    x = `which ant`.strip
    if x == ""
        puts ""
        puts "Error: Ant is missing"
        exit 1
    end
    puts "Found"
    
    print "UUID ... "
    x = `find /usr/local/include -name 'uuid.h'`.strip
    if x == ""
        x = `find /usr/include -name 'uuid.h'`.strip
        if x == ""
            puts ""
            puts "Error: uuid/uuid.h is missing"
            exit 1
        end
    end
    puts "Found"
    
    puts "All Dependencies are found"
end

#################################################################################################
## Command-line
#################################################################################################
begin
    if ARGV.length==0
        puts "Usage: ruby build.rb OPTION [ARGS]"
        puts "\tAvailable options: "
        puts "\t\t - build [opensharding-branch]"
        exit 
    end
    
    option = ARGV[0]
    
    username = `whoami`.strip
    if username != 'root'
        puts "This script must be run as the root user (not as '#{username}')"
        exit 1
    end
    
    if option == "build"
        if ARGV.length < 2
            puts "Warn: No branch specified to build, defaulting to #{BRANCH}"
            build BRANCH
        else
            proposed_branch = ARGV[1]
            if !BRANCH_LIST.include? proposed_branch
                puts "Error: #{proposed_branch} is not a valid branch"
                exit 1
            else
                build proposed_branch
            end
        end
    elsif option == "check_dep"
        check_dep
    else
        puts "Error: Invalid Argument"
        exit 1
    end
    
end