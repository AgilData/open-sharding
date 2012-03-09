#!/usr/bin/env ruby

require 'fileutils'
require 'find'

MYSQL_SUPPORTED = "5.0|5.1"

if ARGV[0] == "5.0"
  mysql_client_vers="15"
elsif ARGV[0] == "5.1"
  mysql_client_vers="16"
else
  puts "Usage: ruby setup.rb " + MYSQL_SUPPORTED + "[/clientlib/search/path/]"
  exit 2
end

libdir = "/usr/"
if ARGV[1] != nil
  libdir = ARGV[1]
end

puts 'Removing old symlinks...'
FileUtils.remove_file('libmysqlclient.so.' + mysql_client_vers, :force => true)
FileUtils.remove_file('libmysqlclient_r.so.' + mysql_client_vers, :force => true)
FileUtils.remove_file('libmysqlclient_real_r.so.' + mysql_client_vers + '.0.0', :force => true)
FileUtils.remove_file('libopensharding.so', :force => true)

puts 'Searching for MySQL client in ' + libdir
i=0
mysqlclient_lib = ""
mysqlclient_libs = Array.new

Find.find(libdir) do |path|
  if !File.directory?(path)
    if File.basename(path)=="libmysqlclient_r.so." + mysql_client_vers
      mysqlclient_libs[i] = path
      i+=1
    end
  end
end

if mysqlclient_libs.size == 0
  puts 'Unable to find mysql client library'
  exit 1
elsif mysqlclient_libs.size == 1
  mysqlclient_lib = mysqlclient_libs[0]
else
  i=1
  mysqlclient_libs.each do |lib|
    puts "[" + i.to_s + "] " + lib
    i+=1
  end
  puts 'Found multiple mysql client libraries. Please choose one: '
  choice = STDIN.gets
  while (choice.to_i > mysqlclient_libs.size) || (choice.to_i < 1)
    puts "Choice: " + choice.chomp + " - is not a valid option. Choose again or Ctrl+C to exit."
    i=1
    mysqlclient_libs.each do |lib|
      puts "[" + i.to_s + "] " + lib
      i+=1
    end
    puts 'Found multiple mysql client libraries. Please choose one:'
      choice = STDIN.gets
  end
  mysqlclient_lib = mysqlclient_libs[choice.to_i-1]
end

if mysqlclient_lib != ""
  puts 'Found mysql library, using ' + mysqlclient_lib
else
  puts 'Unable to find libmysqlclient_r.so.' + mysql_client_vers + ' in ' + libdir
  exit 1
end

puts 'Creating symlinks...'
pwd = ENV['PWD']

if pwd == nil
  puts 'Unable to read PWD environment variable.'
  puts 'If you are using sudo, try "sudo -E ruby setup.rb ..."'
  puts '   to preserve environment variables.'
  exit 1
end

pwd = pwd + "/"

FileUtils.ln_s(pwd + 'libmyosp.so.' + mysql_client_vers,
               pwd + 'libmysqlclient.so.' + mysql_client_vers)

FileUtils.ln_s(pwd + 'libmyosp_r.so.' + mysql_client_vers,
               pwd + 'libmysqlclient_r.so.' + mysql_client_vers)

FileUtils.ln_s(pwd + 'libopensharding.so.1.0.0', pwd + 'libopensharding.so')

FileUtils.ln_s(mysqlclient_lib,
               'libmysqlclient_real_r.so.' + mysql_client_vers + '.0.0')
puts 'Installation Complete'

