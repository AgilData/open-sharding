#!/bin/env ruby

#################################################################################################################
## THIS IS THE NEW OFFICIAL BUILD SCRIPT FOR OPEN SHARDING
#################################################################################################################

def run_command (cmd)
  puts "Executing: #{cmd}"
  if ! system cmd
    puts "FAILED: #{cmd}"
  end
end

def clean
  run_command "find . -name '*.o' -exec rm -f {} \\;"
  run_command "find . -name '*.a' -exec rm -f {} \\;"
  run_command "find . -name '*.d' -exec rm -f {} \\;"
  run_command "find . -name '*.so' -exec rm -f {} \\;"
  run_command "find . -name '*.so.*' -exec rm -f {} \\;"
end


begin

  # it is ABSOLUTELY CRITICAL that we do clean builds when deploying to production
  clean

  run_command "cd libopensharding ; make clean ; make"
  run_command "cd libmyospfacade ; make clean ; make sixteen"

end
