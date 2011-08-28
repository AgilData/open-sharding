package org.opensharding.jdbc;


import java.io.File;
import java.io.FileInputStream;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Properties;

import org.opensharding.common.logging.Logger;
import org.opensharding.common.logging.LoggerFactory;
import org.opensharding.jdbc.impl.AbstractDriver;

/**
 * OSP JDBC Driver.
 *
 * @author Andy Grove
 * @created 16-Sep-2007 08:55:22
 * Modified for OSP support: 27-Aug-2011
 */
public class Driver extends AbstractDriver {

	private static final Logger logger = LoggerFactory.getLogger(Driver.class);
	
    /**
     * Static initialization method.
     */
    static {
        try {
            DriverManager.registerDriver( new Driver() );
        } catch (Throwable th) {
        	logger.error("static initializer error", th);
            throw new ExceptionInInitializerError(th);
        }
    }
        
}
