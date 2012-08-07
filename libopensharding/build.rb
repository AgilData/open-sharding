#!/env/ruby

require 'time'

# constants
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

  # compile all source files
  puts "Compiling..."
  run_command "find . -name '*.cpp' -print -exec g++ #{CPPFLAGS} {} \\;"

  # create a static library using the archiver utility
  run_command "ar rs libopensharding.a *.o"

  # create a shared library
  if RUBY_PLATFORM.downcase =~ /darwin/
    #TODO: run_command "gcc -shared -Wl,-soname,libopensharding.so -o libopensharding.so.1.0.0 *.o -lc -lstdc++ -l#{BOOST_THREAD_LIB} -L/usr/lib64 -L#{BOOST_THREAD_LIB}"
  elsif RUBY_PLATFORM.downcase =~ /linux/
    run_command "gcc -shared -Wl,-soname,libopensharding.so -o libopensharding.so.1.0.0 *.o -lc -lstdc++ -l#{BOOST_THREAD_LIB} -L/usr/lib64 -L#{BOOST_THREAD_LIB}"
  end

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
