#!/bin/env ruby

require 'fileutils'
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

puts 'Creating symlinks...'

FileUtils.ln_s('libmyospfacade.so.' + mysql_client_vers,
               'libmysqlclient.so.' + mysql_client_vers)

FileUtils.ln_s('libmyospfacade_r.so.' + mysql_client_vers,
               'libmysqlclient_r.so.' + mysql_client_vers)

#TODO: we need to link to the real mysql driver here, but where is it ??
FileUtils.ln_s('libmysqlclient_real_r.so.' + mysql_client_vers + '.0.0',
               'libmysqlclient_real_r.so.' + mysql_client_vers)

