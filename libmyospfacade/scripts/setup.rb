#!/bin/env ruby

require 'fileutils'
require 'find'

MYSQL_SUPPORTED = "[5.0,5.1]"

if ARGV[0] == "5.0"
  mysql_client_vers="15"
elsif ARGV[0] == "5.1"
  mysql_client_vers="16"
else
  puts "Usage: ruby setup.rb" + MYSQL_SUPPORTED
  exit 2
end

puts 'Removing old symlinks...'
FileUtils.remove_file('libmysqlclient.so.' + mysql_client_vers, :force => true)
FileUtils.remove_file('libmysqlclient_r.so.' + mysql_client_vers, :force => true)
FileUtils.remove_file('libmysqlclient_real_r.so.' + mysql_client_vers, :force => true)
FileUtils.remove_file('libopensharding.so', :force => true)

puts 'Finding MySQL installation...'
mysqlclient_lib = ""
Find.find("/usr/") do |path|
  if !File.directory?(path)
    if File.basename(path)=="libmysqlclient_r.so." + mysql_client_vers
      if mysqlclient_lib != ""
        puts 'Found multiple libraries, unable to determine which to use'
        exit 2
      end
      mysqlclient_lib=path
    end
  end
end
if mysqlclient_lib == ""
  puts 'Unable to find mysql client library'
  exit 1
end
puts 'Found mysql library, using ' + mysqlclient_lib

puts 'Creating symlinks...'
pwd = ENV["PWD"] + "/"
FileUtils.ln_s(pwd + 'libmyosp.so.' + mysql_client_vers,
               pwd + 'libmysqlclient.so.' + mysql_client_vers)

FileUtils.ln_s(pwd + 'libmyosp_r.so.' + mysql_client_vers,
               pwd + 'libmysqlclient_r.so.' + mysql_client_vers)

FileUtils.ln_s(pwd + 'libopensharding.so.1.0.0', pwd + 'libopensharding.so')

FileUtils.ln_s(mysqlclient_lib,
               'libmysqlclient_real_r.so.' + mysql_client_vers + '0.0')
puts 'Installation Complete'
