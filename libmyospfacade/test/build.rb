#!/bin/env ruby
puts "Compiling"
system "gcc -I /usr/include/mysql -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -fno-inline -MMD -MP multithreadtest.cpp"
puts "Linking"
system "gcc -o multithreadtest multithreadtest.o -lc -lstdc++ -ldl -lpthread -lmysqlclient -L/usr/lib64/myosp"


