-- OpenSharding MyOSP Driver --

-- Installation --

  Packaged in the .tar.gz archive are the following files:
    libboost_thread-gcc41-mt-1_38.so.1.38.0
    libmyosp.so.15
    libmyosp_r.so.15
    libmyosp.so.16
    libmyosp_r.so.16
    libopensharding.so.1.0.0
    setup.rb
    myosp.conf

  The tar should be extracted to its own directory. We recommend /usr/lib/myosp/

  setup.rb will create the necessary symlinks within the installation directory.
Usage is "ruby setup.rb [5.0|5.1]" where the argument is the version of the
mysql client to use; 5.0 or 5.1.

  This directory needs to be on the library path ahead of the native mysql
driver so that applications load it instead of the native driver. To do this,
export LD_LIBRARY_PATH=/usr/lib/myosp/ in the application's environment.

-- Configuration --
  The default location for myosp configs is /etc/osp/myosp.conf. The directory
can be set using the environment variable "MYOSP_CONF_DIR=/path/to/configdir/"
The driver will look for myosp.conf in the directory set.
  A sample myosp.conf file is supplied in the archive which explains
configuration options.
