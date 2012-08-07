#!/env/ruby

BOOST_INCLUDE_PATH	= "/usr/local/include/boost-1_38"
LIBXML_INCLUDE_PATH	= "/usr/include/libxml2"
BOOST_THREAD_LIB	= "boost_thread-gcc42-mt"

CPPFLAGS="-Isrc -I#{BOOST_INCLUDE_PATH} -I#{LIBXML_INCLUDE_PATH} -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -fno-inline -MMD -MP"

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

def compile
  run_command "find . -name '*.cpp' -exec gcc #{CPPFLAGS} {} \\;"
end

begin
  clean
  compile
end
