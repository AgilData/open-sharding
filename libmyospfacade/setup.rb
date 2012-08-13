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
## Myosp Placement
#################################################################################################
def myosp_conf_placement
    if File.exist?("/etc/osp/myosp.conf")
        puts "The myosp.conf file is already in place."
        else
        puts "Moving the myosp.conf in the directory /etc/osp/, please edit this config for proper use of MyOSP."
        Dir.mkdir("/etc/osp/")
        run_command("cp /usr/lib64/myosp/myosp.conf /etc/osp/")
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
    if File.exist?("/etc/ld.so.conf.d/1.so.myosp.conf")
        puts "File /etc/ld.so.conf.d/1.so.myosp.conf already exsists, now running ldconfig."
    else
        puts "Creating the /etc/ld.so.conf.d/1.so.myosp.conf for setting up myosp."
        ld_config=File.open("/etc/ld.so.conf.d/1.so.myosp.conf", 'w')
        if ! ld_config
            puts "Failed to open the file /etc/ld.so.conf.d/1.so.myosp.conf"
            exit
        else
            ld_config.puts "/usr/lib64/myosp"
            ld_config.close
        end
    end
    run_command("ldconfig")
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
