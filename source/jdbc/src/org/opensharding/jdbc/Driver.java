package org.opensharding.jdbc;


import java.io.File;
import java.io.FileInputStream;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Properties;

import org.opensharding.jdbc.impl.AbstractDriver;

/**
 * OSP JDBC Driver.
 *
 * @author Andy Grove
 * @created 16-Sep-2007 08:55:22
 * Modified for OSP support: 27-Aug-2011
 */
public class Driver extends AbstractDriver {

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
        
}
