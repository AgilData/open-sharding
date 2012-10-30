#!/usr/bin/ ruby

require 'fileutils'

##################################
#
# Setup Up Script For MyOSP Driver
#
##################################
DEFAULTSERVER = "centos"
UBUNTU = "ubuntu"
CENTOS = "centos"
REDHAT = "redhat"
CARBON = "carbon"
FREEBDSD = "freebdsd"
MAC  = "mac"
WINDOW = "window"
LIBRARY64 = "lib64"
LIBRARY = "lib"
MYOSPCONFDIR = "/etc/osp/myosp.conf"

#################################################################################################
## Run an operating system command
#################################################################################################
def run_command(cmd)
    puts "Executing: #{cmd}"
    if ! system cmd
        puts "FAILED to execute the  following command: #{cmd}"
        exit 1
    end
end

#################################################################################################
## Get Platform 
#################################################################################################
def get_platform
    if RUBY_PLATFORM.downcase =~ /darwin/
        return MAC
    elsif RUBY_PLATFORM.downcase =~ /windows/
        return WINDOW
    else
        # get linux distro name and version (might now work on all distros)
        
        if(File.exists?("/etc/issue")) 
          issue = `cat /etc/issue`.downcase.split
          platform = "#{issue}"
          return platform
        else
          puts "Unknown platform. Defaulting to a #{DEFAULTSERVER} procedure."
          return DEFAULTSERVER
        end
    end
end

#################################################################################################
## myosp.conf Placement
#################################################################################################
def myosp_conf_placement
    
    puts "Obtaining architecture."
    architecture = `uname -i`
    
    puts "Obtaining platform"
    platform = get_platform
    
    #Checking to see if the platform is Ubuntu
    if platform.match(UBUNTU)
        puts "Platform obtained: #{platform}"
        library = LIBRARY
        puts "Library file set to #{LIBRARY}"
    else
      
      #Checking platform 
      if platform.match(DEFAULTSERVER)
         puts "Using the default setup for Centos."
         puts "This is only called for centos and unsupported platforms."
      else
        puts "Using the platform: #{platform}"
      end
      
      #Checking Architecture
      if (architecture.match("x86_64"))
          puts "Architecture of #{architecture}"
          library = LIBRARY64
          puts "Library file set to #{LIBRARY64}"
      elsif (architecture.match("i386"))
          puts "Architecture of #{architecture}"
          library = LIBRARY
          puts "Library file set to #{LIBRARY}"
      else
          puts "Undefined architecture defaulting to a x86_64 architecture."
          library = LIBRARY64
      end
      
    end
    
    #Checking to see if the Myosp.conf is already in place.
    if File.exist?(MYOSPCONFDIR)
        puts "The file #{MYOSPCONFDIR} exists, nothing else needs to be done."
    else
        #Moving the template myosp.conf
        puts "Moving the template myosp.conf in the directory /etc/osp/, please edit this config for proper use of MyOSP."
        Dir.mkdir("/etc/osp/")
        run_command("cp /usr/#{library}/myosp/myosp.conf /etc/osp/")
        
        #Double Checking
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
  puts "Obtaining architecture for ldconfig setup."
  architecture = `uname -m`
  
  puts "Obtaining platform for ldconfig setup."
  platform = get_platform
  
  #Checking for ubuntu platform
  if platform.match(UBUNTU)
      puts "Platform obtained: #{platform}"
      library = LIBRARY
      puts "Library file set to #{library}"
  else
    #Checking platform 
    if platform.match(DEFAULTSERVER)
       puts "Using the default setup for Centos."
       puts "This is only called for centos and unsupported platforms."
    else
      puts "Using the platform: #{platform}"
    end
    
    #Checking Architecture
    if (architecture.match("x86_64"))
        puts "Architecture of #{architecture}"
        library = LIBRARY64
        puts "Library file set to #{LIBRARY64}"
    elsif (architecture.match("i386"))
        puts "Architecture of #{architecture}"
        library = LIBRARY
        puts "Library file set to #{LIBRARY}"
    else
        puts "Undefined architecture defaulting to a x86_64 architecture."
        library = LIBRARY64
    end
    
  end
  
  #Checking to see if ldconfig has been setup before.   
  if File.exist?("/etc/ld.so.conf.d/1.so.myosp.conf")
      puts "File /etc/ld.so.conf.d/1.so.myosp.conf already exsists, now running ldconfig."
  else
      puts "Creating the /etc/ld.so.conf.d/1.so.myosp.conf for setting up myosp libraries."
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
