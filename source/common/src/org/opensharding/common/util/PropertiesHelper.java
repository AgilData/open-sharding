package org.opensharding.common.util;

import java.util.Properties;
import java.io.File;
import java.io.IOException;
import java.io.FileInputStream;

/**
 * @author Andy Grove
 */
public class PropertiesHelper {

    public static Properties loadProperties(File file) throws IOException {
        Properties prop = new Properties();
        final FileInputStream is = new FileInputStream(file);
        prop.load(is);
        is.close();
        return prop;
    }

    public static String getStringProperty(Properties prop, String name) {
        return getStringProperty(prop, name, null);
    }

    public static String getStringProperty(Properties prop, String name, String defaultValue) {
        final String value = prop.getProperty(name);
        return value == null ? defaultValue : value;
    }

    public static int getIntProperty(Properties prop, String name, int defaultValue) {
        final String value = prop.getProperty(name);
        if (value == null) {
            return defaultValue;
        }
        return Integer.parseInt(value);
    }

    public static boolean getBoolProperty(Properties prop, String name) {
        return getBoolProperty( prop, name, false );
    }
    
    public static boolean getBoolProperty(Properties prop, String name, boolean defaultValue) {
        final String value = prop.getProperty(name);
        if (value == null) {
            return defaultValue;
        }
        if (value.equals("true")) {
            return true;
        }
        if (value.equals("false")) {
            return false;
        }
        throw new IllegalStateException( "Invalid value '" + value + "' for boolean property '" + name + "'");
    }
}
