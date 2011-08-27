package org.opensharding.jdbc.impl;

import java.io.InputStream;
import java.io.StringWriter;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Properties;

import org.opensharding.common.logging.Logger;
import org.opensharding.common.logging.LoggerFactory;

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
	
	private static final Logger logger = LoggerFactory.getLogger(AbstractDriver.class);

    /**
     * The actual driver to delegate to.
     */
    protected static java.sql.Driver delegate;

    /**
     * Required URL prefix for this driver.
     */
    public static final String URL_PREFIX = "jdbc:osp:";
    
    private static final String DELEGATE_PREFIX = "delegate:";
    
    private static final int OSP_MODE = 1;
    private static final int DELEGATE_MODE = 2;
    
    // The driverMode, either OSP or DELEGATE.
    protected static int driverMode = -1;
        
    private Properties logProps = null;
    
    private static final String PROPERTIESFILENAME = "log.properties";
    
    private CallLogger callLogger = null;

    /**
     * Construct the Driver and register it with the DriverManager.
     */
    public AbstractDriver() {
        try {
        	// Register the driver.
            DriverManager.registerDriver(this);
                        
        } catch (Throwable th) {
            logger.error("OSP JDBC Driver init error", th);
            throw new ExceptionInInitializerError(th);
        }
    }
    
    /**
     * Initialize the driver using the log.properties.
     * @throws Exception
     */
    private void init(String url) throws Exception {
    	
    	// Attempt to find the log.properties.
		InputStream inputStream = getClass().getClassLoader().getResourceAsStream(PROPERTIESFILENAME);
		if(inputStream != null) {
			logProps = new Properties();
			logProps.load(inputStream);
		} else {
			logger.warn("init: The " + PROPERTIESFILENAME + " could not be found on the classpath. Using default system log settings.");
		}
		
		// Get the CallLogger. If props is null, or shard analyze logging is not enabled, the log methods do nothing.
		callLogger = CallLogger.getLogger(url, logProps);
		
    }
    
    public boolean acceptsURL(java.lang.String url) throws java.sql.SQLException {
        return url.toLowerCase().startsWith(URL_PREFIX.toLowerCase());
    }

	public java.sql.Connection connect(final java.lang.String url,java.util.Properties p1) throws java.sql.SQLException {
        final long __start = System.currentTimeMillis();
        // Properties parameter is not logged because it will contain the user's password
        try {
        	
        	// Parse the URL details. The URL can be in the form of:
        	//
        	// Delegate Mode:
        	// delegate:[delegate-url]
        	//
        	// or:
        	//
        	// OSP Mode:
        	// [protocol]:[details]

        	// The ConnectionFacade return variable.
        	ConnectionFacade ret = null;
        	
        	// Init the driver, loading the properties file and CallLogger if required.
        	init(url);
        	
            String urlDetails = url.substring(URL_PREFIX.length());


            // If this is a delegate driver
            if(urlDetails.startsWith(DELEGATE_PREFIX)) {
            	driverMode = DELEGATE_MODE;
            	String delegateUrl = url.substring(URL_PREFIX.length() + DELEGATE_PREFIX.length());
            	callLogger.log("Delegate mode enabled. Connecting to: " + delegateUrl);
            	logger.info("Delegate mode enabled. Connecting to: " + delegateUrl);
            	
            	// Get the delegate driver.
            	delegate = DriverManager.getDriver(delegateUrl);
            	
            	// Open the connection to the delegate.
                java.sql.Connection conn = delegate.connect(delegateUrl, p1);
            	
            	if (conn == null) {
            		throw new SQLException("The delegate driver failed to return a connection. " 
            				+ "Ensure the connection URL (url property) for the driver is correct.");
            	}

            	ret = new ConnectionFacade(conn, callLogger);

                String propertiesParam = null;
                final long __finish = System.currentTimeMillis();
                if (callLogger.logData) {
                    StringWriter stringWriter = new StringWriter();
                    p1.store(stringWriter, null);
                    propertiesParam = stringWriter.toString();
                }
                callLogger.log(ret.connectionID, 0, "Driver","connect(java.lang.String,java.util.Properties)",new Object[] {delegateUrl,propertiesParam}, __start, __finish);
            	
            // Else this is OSP mode.
            } else {
            	driverMode = OSP_MODE;
            	callLogger.log("OSP model enabled. Connecting to: " + urlDetails);
            	logger.info("OSP mode enabled. Connecting to: " + urlDetails);
            	
            	// Not yet implemented.
            	throw new Exception("OSP mode not yet implemented.");
            }

            return ret;

        } catch (Throwable th) {
            logger.error("connect error", th);
            throw new SQLException( "Failed to initialize driver: " + th.getMessage(), th);
        }
    }
	
}
