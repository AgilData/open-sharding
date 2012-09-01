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
## Get Platform
#################################################################################################
def get_platform
    if RUBY_PLATFORM.downcase =~ /darwin/
        return "mac"
        elsif RUBY_PLATFORM.downcase =~ /windows/
        return "win"
        else
        
        # get linux distro name and version (might now work on all distros)
        issue = `cat /etc/issue`.downcase.split
        arch = `uname -i`
        platform = "#{issue} #{arch}"
        return platform
    end
end
#################################################################################################
## Install Dependencies
#################################################################################################
def install_dependencies
    puts "Installing Dependencies *****************************************"
    platform = get_platform
    if platform.match("centos")
        run_command("yum install -y gcc e2fsprogs-devel ncurses-devel libtool-ltdl-devel python-devel subversion-devel openssl-devel java-1.6.0-openjdk-devel kernel-devel uuid-devel")
        elsif platform.match("ubuntu") || platform.match("debian")
        run_command("sudo apt-get install -y build-essential libncurses5-dev libltdl3-dev uuid-dev")
        else
        puts "Platform not found: #{platform}"
        exit
    end
end
#################################################################################################
## Building libmyosp
#################################################################################################
def build(mysql_version)
    
    puts "Building MyOSP *****************************************"
    
    # we need to make sure we already have the mysql headers for the _real libs since we need
    # to compile against the same exact version or we will end up with hard to diagnose segmentation
    # faults at runtime
    if mysql_version.match("5.0")
        mysql_real_dir = "mysql-install-#{mysql_version}/mysql-5.0.45"
        elsif mysql_version.match("5.1")
        mysql_real_dir = "mysql-install-#{mysql_version}/mysql-5.1.36"
        else
        puts "Support for mysql #{mysql_version} is not avaliable."
        exit -1
    end
    
    if !File.exists?(mysql_real_dir)
        puts "Could not locate mysql directory '#{mysql_real_dir}' - you should run build-real first (this is a one-time requirement)"
        exit -1
    end
    
    # check for any libopensharding.so already deployed that might be on LD_LIBRARY_PATH
    # this is REALLY important or we will end up with corrupt binaries or a mix
    # of old and new functionality
    
    # look for libopensharding
    puts "Searching for deployed versions of libopensharding..."
    deployed_opensharding_libs = `find /usr -name \"libopensharding*\"`
    #puts "FOUND: #{deployed_opensharding_libs}"
    if deployed_opensharding_libs != ""
        puts "Cannot build until you delete these already deployed versions: #{deployed_opensharding_libs}"
        exit
    end
    puts "No deployed versions of libopensharding found"
    
    # build libopensharding
    puts "Building libopensharding"
    run_command "cd libopensharding ; ruby build.rb"
    
    # build libmyosp
    puts "Building libmyosp"
    run_command "cd libmyospfacade ; ruby build.rb release #{mysql_version}"
    
end

#################################################################################################
## Building MySQl
#################################################################################################
def boost-build
    puts "Down loading the boost libraries."
    cmd = 'wget http://downloads.sourceforge.net/project/boost/boost/1.38.0/boost_1_38_0.tar.gz'
    run_command cmd
    cmd = 'tar xvfz boost_1_38_0.tar.gz'
    run_command cmd
    Dir.chdir "boost_1_38_0"
    puts "Changed Directory to boost_1_38_0"
    cmd = './configure --with-libraries=thread'
    run_command cmd
    cmd = 'make'
    run_command cmd
    cmd = 'sudo make install'
    run_command cmd
    
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
    
    
    puts "All Dependencies are found"
end

#################################################################################################
## Command-line
#################################################################################################
begin
    if ARGV.length==0
        puts "Usage: ruby build.rb OPTION [ARGS] [MYSQL VERSION]"
        puts "\tAvailable options: "
        puts "\t\t - setup-env "
        puts "\t\t - build (Installation of myosp only)"
        puts "\t\t - build-boost (Installation of boost and dependencies. Only needs to be executed once.)"
        puts "\t MYSQL VERSION OPTIONS"
        puts "\t\t - 5.0 "
        puts "\t\t - 5.1 "
        exit
    end
    
    option = ARGV[0]
    
    if option == "build"
        puts "Building with the option: #{option}"
        if ARGV.length==1
            puts "Usage: ruby build.rb build [MYSQL VERSION]"
            puts "\tAvailable options: "
            puts "\t\t - 5.0 "
            puts "\t\t - 5.1 "
            exit
        end
        mysql_version = ARGV[1]
        if mysql_version.match("5.0") || mysql_version.match("5.1")
            build(mysql_version)
            else
            puts "Error: Not supported MySQL type."
        end
        elsif option == "build-boost"
        puts "Building with the option: #{option}"
        if ARGV.length==1
            puts "Usage: ruby build.rb build-real [MYSQL VERSION]"
            puts "\tAvailable options: "
            puts "\t\t - 5.0 "
            puts "\t\t - 5.1 "
            exit
        end
        mysql_version = ARGV[1]
        if ! mysql_version.match("5.0") &&  ! mysql_version.match("5.1")
            puts "Error: Not supported MySQL type."
            exit
        end
        install_dependencies
        boost-build
        elsif option == "check-dep"
        check_dep
        else
        puts "Error: Invalid Argument"
        exit 1
    end
    
end
