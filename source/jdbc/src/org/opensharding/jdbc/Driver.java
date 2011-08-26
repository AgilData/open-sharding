package org.opensharding.jdbc;


import java.io.File;
import java.io.FileInputStream;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Properties;

import org.opensharding.jdbc.impl.AbstractDriver;

/**
 * Oracle Delegate JDBC Driver.
 *
 * @author Andy Grove
 * @created 16-Sep-2007 08:55:22
 */
public class Driver extends AbstractDriver {

	private static String delegateDriverClassName;
    /**
     * Static initialization method.
     */
    static {
        try {
            DriverManager.registerDriver( new Driver() );
        } catch (SQLException e) {
            throw new ExceptionInInitializerError(e);
        }
    }
    
    public String getDelegateDriverClassName() throws Exception {
        String propFile = "/etc/dbshards-analyze.properties";

        /* Initialize the properties.*/
        final File file = new File(propFile);
        if (file.exists()) {
        	Properties prop = new Properties();
        	final FileInputStream fis = new FileInputStream(file);
        	prop.load(fis);
        	delegateDriverClassName = prop.getProperty("driver.class.name");
        	if(delegateDriverClassName == null) {
        		throw new Exception("The driver.class.name property was not found. Cannot initialize AnalyzeDriver."
        				+ " Properties file: /etc/dbshards-analyze.properties");
        	}
        } else {
        	throw new Exception("The /etc/dbshards-analyze.properties file could not be found."
        			+ " Cannot initialize AnalyzeDriver.");
        }
            
    	return delegateDriverClassName;
    }
    
}
