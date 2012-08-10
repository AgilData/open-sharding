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
## Building libmyosp
#################################################################################################
def build

    # check for any old libs already deployed that might be on LD_LIBRARY_PATH
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

    # look for libmyosp
    puts "Searching for deployed versions of libmyosp..."
    deployed_myosp_libs = `find /usr -name \"libmyosp*\"`
    #puts "FOUND: #{deployed_myosp_libs}"
    if deployed_myosp_libs != ""
      puts "Cannot build until you delete these already deployed versions: #{deployed_myosp_libs}"
      exit
    end
    puts "No deployed versions of libmyosp found"

    # build libopensharding
    puts "Building libopensharding"
    run_command "cd libopensharding ; ruby build.rb"

    # build libmyosp
    puts "Building libmyosp"
    run_command "cd libmyospfacade ; ruby build.rb release"

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
        puts "\t\t - check-dep "
        puts "\t\t - build "
        exit
    end

    option = ARGV[0]

    if option == "build"
      build
    elsif option == "check-dep"
      check_dep
    else
        puts "Error: Invalid Argument"
        exit 1
    end

end