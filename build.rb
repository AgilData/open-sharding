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
## Building MySQl
#################################################################################################
def mysql_install(mysql_version)
    mysql-dir = "/root/mysql-install/"
    puts "Installing mysql libraries based on the version: #{mysql_version}"
    run_command("groupadd mysql")
    run_command("useradd -g mysql mysql")
    Dir.mkdir(mysql-dir)
    if mysql_version.match("5.0")
        run_command("svn export https://subversion.assembla.com/svn/open-sharding-test/trunk/mysql/mysql-5.0.96-myosp.tar.gz")
        run_command("tar xvzf mysql-5.0.96-myosp.tar.gz -C #{mysql-dir}")
    elsif mysql_version.match("5.1")
        run_command("svn export https://subversion.assembla.com/svn/open-sharding-test/trunk/mysql5.1.62/mysql5.1real.tar.gz")
        run_command("tar xvfz mysql5.1real.tar.gz -C #{mysql_version}")
    else
        puts "Invalid version for installation of mysql."
        exit
    end
    Dir.chdir(mysql-dir)
    run_command("./configure --prefix=/usr/local/mysql")
    run_command("make")
    run_command("make install")
    Dir.chdir("/usr/local/mysql")
    run_command("chown -R mysql .")
    run_command("chgrp -R mysql .")
    run_command("bin/mysql-install-db --user=mysql")
    run_command("chown -R root .")
    run_command("chown -R mysql var")
    run_command("cp #{mysql-dir}support-files/my-medium.cnf /etc/my.cnf")
    run_command("bin/mysqld_safe --user=mysql &")
    run_command("cp #{mysql-dir}support-files/mysql.server /etc/init.d/mysql.server")
    run_command("ln bin/mysql /usr/bin/mysql")
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
        puts "\t\t - build-real"
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
        mysql_version = ARGV[1]
        if mysql_version.match("5.0") || mysql_version.match("5.1")
            build(mysql_version)
            else
            puts "Error: Not supported MySQL type."
        end
    elsif option == "build-real"
        mysql_install(mysql_version)
        build(mysql_version)
    elsif option == "check-dep"
        check_dep
    else
        puts "Error: Invalid Argument"
        exit 1
    end
    
end