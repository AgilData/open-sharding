#!/usr/bin/ ruby

require 'fileutils'

##############################
#
# Setup Up Script to 
#
##############################

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
## Myosp Placement
#################################################################################################
def myosp_conf_placement
    platfrom = get_platform
    if platfrom.match("ubuntu") || platfrom.match("i386")
        library = "lib"
    elsif platfrom.match("centos") && platfrom.match("x86_64")
        library = "lib64"
    else
        puts "Non existing platform."
        exit
    end
    if File.exist?("/etc/osp/myosp.conf")
        puts "The myosp.conf file is already in place."
        else
        puts "Moving the myosp.conf in the directory /etc/osp/, please edit this config for proper use of MyOSP."
        Dir.mkdir("/etc/osp/")
        run_command("cp /usr/#{library}/myosp/myosp.conf /etc/osp/")
        if File.exist?("/etc/osp/myosp.conf")
            puts "Succesfully copied myosp.conf to the appropriate location."
            else
            puts "Failed to copy the myosp.conf"
            exit
        end
    end
end

#################################################################################################
## Set Up LDCONFIG
#################################################################################################
def myosp_ldconfig
    platfrom = get_platform
    if platfrom.match("ubuntu") || platfrom.match("i386")
        library = "lib"
        elsif (platfrom.match("centos") || platfrom.match("redhat")) && platfrom.match("x86_64")
        library = "lib64"
        else
        puts "Non existing platform."
        exit
    end
    if File.exist?("/etc/ld.so.conf.d/1.so.myosp.conf")
        puts "File /etc/ld.so.conf.d/1.so.myosp.conf already exsists, now running ldconfig."
    else
        puts "Creating the /etc/ld.so.conf.d/1.so.myosp.conf for setting up myosp."
        ld_config=File.open("/etc/ld.so.conf.d/1.so.myosp.conf", 'w')
        if ! ld_config
            puts "Failed to open the file /etc/ld.so.conf.d/1.so.myosp.conf"
            exit
        else
            ld_config.puts "/usr/#{library}/myosp"
            ld_config.close
        end
    end
    run_command("ldconfig")
    #TODO: This section is only a rough manner in which to remove the desired libmysqlcient path, however for the time being this will solve this issue.

    #    ldd_value = `ldd /usr/bin/mysql | grep mysql`
    #ldd_array = ldd_value.split(" => ")
    #lib_path = ldd_array[1].split(" (")
    #puts "Removing:: #{lib_path[0]}"
    #run_command("rm #{lib_path[0]}")
    #puts `ldd /usr/bin/mysql`
    #puts "If the ldd shows that it is using the mysql lib found in myosp, then installation has completed."
    #puts `export MYSQL_PS1="MyOSP(Mysql)>  "`
end

#################################################################################################
## Command Line
#################################################################################################
begin 
    
    puts "Beginning the Set Up of MYOSP"
    puts "Checking the myosp.conf"
    myosp_conf_placement
    puts "Checking the ldconfig"
    myosp_ldconfig
    puts "Completed!"
   
        
end
