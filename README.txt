=====================================
Open Sharding Protocol Driver Readme
=====================================

----INTRODUCTION

The Open Sharding Protocol (OSP) provides a generic, open source framework for support of high-performance
database sharding to virtually any DBMS. The official OSP Web site is located at:

http://www.opensharding.org  

The OSP project provides the protocol definition itself, along with a variety of open source plug-compatible database 
drivers, called Facade Drivers (see more below). OSP is highly flexible, and the protocol itself is extremely efficient, 
based on message encoding using the Google Protocol Buffers wire format: 

http://code.google.com/apis/protocolbuffers/docs/encoding.html

Note that it is also easy to support commercial database drivers using OSP as well.


FACADE DRIVERS

OSP supports thin Facade Drivers, these are plug-compatible drivers for virtually any type of DBMS. The 
Facade Drivers support the following characteristics:

- Emulate all (or most common) APIs of the native driver they emulate.
- Support for the OSP protocol (when in "osp" mode), communicating with any OSP compatible server or process. 
- Support "delegate" mode, by loading an actual native driver for standard operation.
- Optionally provide Shard Analyze log output, a special logging format that can be 
used to perform a database sharding analysis.

To use a driver to generate Shard Analyze log output, set 

A Facade Driver typically communicates with an OSP process running in the same server as an application, 
via an efficient inter-process communication protocol (several options are supported).
The OSP process provides full database sharding access or other useful services to the application in a
seamless manner. Optionally a Facade Driver can communicate to a remote OSP server via TCP/IP.

The following OSP Facade Drivers are included in the project (more will be added over time):

- JDBC
- Native MySQL(r) libmysqlclient
- ODBC
- Native PostgreSQL libpq (planned)

An OSP driver can operate in either of two modes:

- OSP Mode, for communicating with a local OSP process or remote OSP server
- Delegate Mode, which delegates all actual JDBC calls to a third-party driver

An OSP driver can be used to generate a Shard Analyze log, recording all API calls and SQL statements 
which can be used for shard analysis purposes. The Shard Analyze log can be configured in either 
OSP or Delegate mode, but is normally used with Delegate mode for upfront analysis of a database
to which database sharding shall be implemented.

There is a section for using and configuring each driver, as well as a section on LOG CONFIGURATION
which applies to all drivers. This section includes configuration for the Shard Analyze log as 
well as debug system logging for monitoring driver operation.

-----------------
LOG CONFIGURATION
-----------------

An OSP driver contains two types of logs:

- OSP System Log: This is a log which provides output regarding the operation of the driver itself, usually for debugging purposes.
The default system log file name is: osp-driver-system.log
- Shard Analyze Log: A special log which provides API calls and SQL statements, used for database shard analysis.

Both types of logging are configured using the following properties file:

log.properties

The format of log.properties is used for all drivers. However, the location of the file is driver specific. 
See the instructions for the specific driver(s) you will be using.  

A sample log.properties file is included with the release, and it contains documentation on the various
property names and their function.

---------------
OSP JDBC DRIVER
---------------

The JDBC driver is contained in the following jar file:

osp-jdbc-with-dependencies.jar

Alternatively, you can use the following component jar files (using any compatible version of third-party jars):

osp-common.jar
osp-jdbc-driver.jar
apache-codec.jar
log4j-1.2.15.jar

The classname for the driver is:

org.opensharding.jdbc.Driver

To install the driver, do the following:

- Add the osp-jdbc-with-dependencies.jar to the classpath of your application.
- If using the driver in delegate mode (see below) add the third party driver to the classpath as well.
- Add the log.properties file to the classpath, to configure logging, and to generate a Shard Analyze log.

Here are the steps required to use the OSP JDBC driver with your application:

Register the driver:

Class.forName("org.opensharding.jdbc.Driver").newInstance();

Get a connection to the OSP JDBC driver using the appropriate URL:

Connection conn = DriverManager.getConnection(url, userName, password);

 
OSP MODE

The OSP JDBC URL format is described here:

When operating in "osp" mode, communicating with an OSP process or server, use the following JDBC URL format:

jdbc:osp:[protocol]:[details]

The supported protocol values, along with required details and example URL strings are provided here:

api (Direct API connectivity)
jdbc:osp:api:[ops-server-class-name]:[details]

tcp
jdbc:osp:tcp:host[:port]
jdbc:osp:tcp:myosphost:4001

uds (Unix Domain Socket)
jdbc:osp:uds:[path-to-socket-file]
uds:osp:uds:/var/sock/myospprocess.sock

named-pipes (Unix Named Pipes or Microsoft Windows Named Pipes)
jdbc:osp:pipes:[pipe-name-prefix]
jdbc:osp:pipes:/var/pipe/myosppipe_

DELEGATE MODE

When operating in "delegate" mode, an osp driver loads any third party JDBC driver, delegating all functionality
to the specified driver. This is useful to be able to communicate directly to a DBMS via a third party driver,
and also to support the Shard Analyze log format provided by all OSP Facade Drivers (see Shard Analyze Logging below).

The delegate mode URL format is:

jdbc:osp:delegate:[third-party-driver-classname]:[third-party-url]

The third-party-driver-classname is the qualified classname for the delegate driver.

The third-party-url is the standard JDBC URL supported by the specific third-party-driver driver.

Example:

jdbc:osp:delegate:org.postgresql.Driver:jdbc:postgresql:mydb"

When operating in delegate mode, you must include both the OSP JDBC jar file and the third-party
driver jar file in the classpath of the application, and you must register both drivers as described above.




Licensing

The OSP project components are provided under one of the following open source licenses. See the specific
license(s) provided with any specific component.

Apache License, Version 2.0
http://www.apache.org/licenses/LICENSE-2.0.html

GNU Public License, Version 2
http://www.gnu.org/licenses/gpl-2.0.html

GNU Public License, Version 3
http://www.gnu.org/licenses/gpl-3.0.html

The OSP Protocol itself is a documented, copyright work that is available under the Apache License, Version 2.0.
  

Trademarks and copyright notices:
 
MySQL, Oracle are trademarks of the Oracle Corporation. Microsoft, Windows are trademarks of the Microsoft Corporation. DB2 is a trademark of IBM.

