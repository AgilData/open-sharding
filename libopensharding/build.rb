#!/bin/env ruby

require 'time'

def run_command (cmd)
    puts "Executing: #{cmd}"
    if ! system cmd
        throw "FAILED: #{cmd}"
    end
end

def clean
    run_command "find . -name '*.o' -exec rm -f {} \\;"
    run_command "find . -name '*.a' -exec rm -f {} \\;"
    run_command "find . -name '*.d' -exec rm -f {} \\;"
    run_command "find . -name '*.so' -exec rm -f {} \\;"
    run_command "find . -name '*.so.*' -exec rm -f {} \\;"
end

def compile
    run_command "make clean ; make; make install"
    
    # do some minimal cleaning to get rid of temp files but don't delete libs
    run_command "find . -name '*.o' -exec rm -f {} \\;"
    run_command "find . -name '*.d' -exec rm -f {} \\;"
    
end

begin
    start = Time.now
    clean
    compile
    finish = Time.now
    
    elapsed = (finish-start).to_i
    puts "Compiled in #{elapsed} seconds"
end
