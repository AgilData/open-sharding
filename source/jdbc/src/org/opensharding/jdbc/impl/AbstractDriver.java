package org.opensharding.jdbc.impl;

import java.io.StringWriter;
import java.sql.DriverManager;
import java.sql.SQLException;

/**
 * Generic JDBC Driver that acts as a facade to any other JDBC driver and logs each
 * call to a log file.
 *
 * Driver class name: org.opensharding.jdbc.jdbc.XxxDriver where Xxx is MySQL, Oracle, or SQLServer
 * Driver URL format: jdbc:dbShardsAnalyze:[properties-file-name]
 *
 * Currently, the dbShards Analyze driver can only be used with a single delegate driver per
 * JVM instance (e.g. it is not possible to wrap both MySQL and SQL Server drivers within the
 * same VM).
 *
 */
public abstract class AbstractDriver extends DriverFacade implements java.sql.Driver {

    /**
     * The actual driver to delegate to.
     */
    protected static java.sql.Driver delegate;

    /**
     * Required URL prefix for this driver.
     */
    public static final String URL_PREFIX = "jdbc:dbShardsAnalyze:";

    /**
     * Construct the Driver and register it with the DriverManager.
     */
    public AbstractDriver() {
        try {
            // load and register the delegate driver class
            String driverClassName = getDelegateDriverClassName();
            delegate = (java.sql.Driver) Class.forName(driverClassName).newInstance();
            //System.out.println("delegate: " + delegate);
            DriverManager.registerDriver(delegate);
            DriverManager.registerDriver(this);
        } catch (Throwable e) {
            System.out.println("Error: " + e.getMessage());
            throw new ExceptionInInitializerError(e);
        }
    }

    public abstract String getDelegateDriverClassName() throws Exception;
    
    public boolean acceptsURL(java.lang.String url) throws java.sql.SQLException {
        return url.toLowerCase().startsWith(URL_PREFIX.toLowerCase());
    }

	public java.sql.Connection connect(final java.lang.String url,java.util.Properties p1) throws java.sql.SQLException {
        final long __start = System.currentTimeMillis();
        // Properties parameter is not logged because it will contain the user's password
        try {
        	/* Get the target URL from the delegate driver.
        	 */


            String nativeURL = url.substring(URL_PREFIX.length());

            CallLogger callLogger = null;

            // nativeURL could be just the URL (for most simple uses this is fine) or can be a properties file
            // for more advanced use
            try {
            	callLogger = CallLogger.getLogger(url);
            } catch (Exception e) {
                System.out.println("Error: " + e.getMessage());
                e.printStackTrace();
            }

            CallLogger.log("Connecting to " + nativeURL);

            java.sql.Connection conn = delegate.connect(nativeURL, p1);
        	
        	if (conn == null) {
        		throw new SQLException("The delegate driver failed to return a connection. " 
        				+ "Ensure the connection URL (url property) for the driver is correct.");
        	}
        	
        	//System.out.println("conn: " + conn);
        	final ConnectionFacade ret = new ConnectionFacade(conn, callLogger);
            final long __finish = System.currentTimeMillis();
            // Only log properties param if log
            String propertiesParam = null;

            if (callLogger!=null) {
                if (callLogger.logData) {
                    StringWriter stringWriter = new StringWriter();
                    p1.store(stringWriter, null);
                    propertiesParam = stringWriter.toString();
                }
                callLogger.log(ret.connectionID, 0, "Driver","connect(java.lang.String,java.util.Properties)",new Object[] {nativeURL,propertiesParam}, __start, __finish);
            }

            return ret;

        } catch (Throwable th) {
            System.out.println("Error: " + th.getMessage());
            throw new SQLException( "Failed to initialize driver: " + th.getMessage(), th);
        }
    }

}
