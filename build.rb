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

BOOST_VER = "1.38.0"
BOOST_DIR = "boost_1_38_0"
BOOST_TAR = "boost_1_38_0.tar.gz"
BOOST_DOWNLOAD = "http://downloads.sourceforge.net/project/boost/boost/#{BOOST_VER}/#{BOOST_TAR}"

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
## Set up environment
#################################################################################################
def setup_environment
    check_dep
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
    run_command "./configure --with-libraries=thread"
    run_command "sudo make install"
end

#################################################################################################
## Building libmyosp
#################################################################################################
def build(mysql-verison)

    puts "Building libopensharding"
    run_command "cd libopensharding ; ruby build.rb"

    puts "Building libmyosp"
    run_command "cd libmyospfacade ; ruby build.rb release #{mysql_version}"

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
        puts "Usage: ruby build.rb OPTION [ARGS] [MYSQL VERSION]"
        puts "\tAvailable options: "
        puts "\t\t - setup-env "
        puts "\t\t - build "
        puts "\t MYSQL VERSION OPTIONS"
        puts "\t\t - 5.0 "
        puts "\t\t - 5.1 "
        exit
    end

    option = ARGV[0]

    if option == "build"
        if ARGV.length==1
            puts "Usage: ruby build.rb build [MYSQL VERSION]"
            puts "\tAvailable options: "
            puts "\t\t - 5.0 "
            puts "\t\t - 5.1 "
            exit
        end
         mysql_verison = ARGV[1]
        if mysql_verison.match("5.0") || mysql-verison.match("5.1")
            build(mysql_version)
        else
            puts "Error: Not supported MySQL type."
        end
    elsif option == "setup-env"
      setup_environment
    else
        puts "Error: Invalid Argument"
        exit 1
    end

end