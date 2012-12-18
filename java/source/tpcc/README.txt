---------------------------------
---   Java Tpcc Benchmark     ---
---------------------------------

**** Tpcc Load ****

Database Creation:

The database must first be created with the following script.
bin/create_table.sql

Configuration: 

  The load script can be configured with two methods. 
  The first method is using the tpcc.properties file 
  located in the conf directory. The other method is
  to use the command line arguments option. 
  
  Example tpcc.properties:
    #Properties for tpcc run
    #
    #  This is the properties file in order to run the benchmark.
    #  The properties listed below must be completed with all the properties listed below.
    #  Commenting is done with #.
    
    
    # Settings used by TpccLoad and Tpcc benchmark
    HOST=hostaddress                              -> IP address of the database host or localhost
    DATABASE=dbname                               -> Name of the database
    USER=username                                 -> User-name to access the database
    PASSWORD=password                             -> Password used to access the database
    WAREHOUSECOUNT=10                             -> Number of warehouses to load
    DRIVER=com.mysql.jdbc.Driver                  -> JDBC connection driver
    JDBCURL=jdbc:mysql://localhost:3307/dbname    -> Url of the database to connect with

    
    # Settings used for Tpcc benchmark only
    CONNECTIONS=30
    RAMPUPTIME=45
    DURATION=60
    JDBCFETCHSIZE=-2147483648
    
    # Settings used for TpccLoad only
    SHARDCOUNT=1                                  -> Number of shards to load
    SHARDID=0                                     -> Shard id
    
  Example Command Line Arguments:
    bin/tpccLoad -h localhost -d tpcc -u username -p password -w 10 -j com.mysql.jdbc.Driver -l jdbc:mysql://localhost:3307/dbname -s 2 -i 1
    
    -h [database host]
    -d [database name]
    -u [database username]
    -p [database password]
    -w [number of warehouses]
    -j [java driver]
    -l [jdbc url]
    -s [shard count]
    -i [shard id]


Load Execution:
   
  The load can be carried out by executing the bin/tpccLoad script in two different ways.
  
  bin/tpccLoad -> Defaults to using the tpcc.properties file.
  bin/tpccLoad [ARGUMENTS] -> Uses the arguments passed in the command line.
  
Load Follow Up:

  After the completion of the load, the following index script must be run on the databases.
  dist/bin/add_fkey_idx.sql

**** Tpcc Benchmark ****

Configuration:

  The benchmark script can be configured with two methods. 
  The first method is using the tpcc.properties file 
  located in the conf directory. The other method is
  to use the command line arguments option. 
  
  Example tpcc.properties:
    #Properties for tpcc run
    #
    #  This is the properties file in order to run the benchmark.
    #  The properties listed below must be completed with all the properties listed below.
    #  Commenting is done with #.
    
    
    # Settings used by TpccLoad and Tpcc benchmark
    HOST=hostaddress                              -> IP address of the database host or localhost
    DATABASE=dbname                               -> Name of the database
    USER=username                                 -> User-name to access the database
    PASSWORD=password                             -> Password used to access the database
    WAREHOUSECOUNT=10                             -> Number of warehouses to load
    DRIVER=com.mysql.jdbc.Driver                  -> JDBC connection driver
    JDBCURL=jdbc:mysql://localhost:3307/dbname    -> Url of the database to connect with

    
    # Settings used for Tpcc benchmark only
    CONNECTIONS=30                                -> Number of connections to the database
    RAMPUPTIME=45                                 -> The ramp up time in seconds
    DURATION=60                                   -> Duration of the benchmark in seconds
    JDBCFETCHSIZE=-2147483648                     -> Fetch size 
    
    # Settings used for TpccLoad only
    SHARDCOUNT=1                                  
    SHARDID=0                                      
    
  Example Command Line Arguments:
  
    bin/tpcc -h localhost -d tpcc -u username -p password -w 10 -c 100 -r 10 -t 60 -j com.mysql.jdbc.Driver -l jdbc:mysql://localhost:3307/dbname -f 2147483648 
  
    -h [database host]
    -d [database name]
    -u [database username]
    -p [database password]
    -w [number of warehouses]
    -c [number of connections]
    -r [ramp up time]
    -t [duration of the benchmark (sec)]
    -j [java driver]
    -l [jdbc url]
    -h [jdbc fetch size]
    
Benchmark Execution:
   
  The load can be carried out by executing the bin/tpccLoad script in two different ways.
  
  bin/tpcc -> Defaults to using the tpcc.properties file.
  bin/tpcc [ARGUMENTS] -> Uses the arguments passed in the command line.
   