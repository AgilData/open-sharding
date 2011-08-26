package org.opensharding.common.logging;

import org.opensharding.common.util.FileUtil;
import org.apache.log4j.BasicConfigurator;
import org.apache.log4j.xml.DOMConfigurator;

import java.io.*;
import java.net.URL;
import java.lang.reflect.Method;
import java.util.*;
import java.util.concurrent.Executor;
import java.util.concurrent.Executors;

import org.opensharding.common.util.Sleep;

/**
 * Replacement for log4j (because log4j caused problems on EC2).
 *
 * @author Andy Grove
 */
public class LoggerFactory {

    private static final Logger _logger = new SimpleFileLogger("Logger");

    private static final int LOGGER_TBD = 0;

    private static final int LOGGER_LOG4J = 1;

    private static final int LOGGER_SIMPLE = 2;

    private static int loggerMode = LOGGER_TBD;

    private static final boolean TRACE = false;

    static final Map<String,SimpleFileLogger> loggerMap = new HashMap<String,SimpleFileLogger>();

    static final Executor exec = Executors.newSingleThreadExecutor();

    static Executor execReloadConfig;

    static File propFile;

    static String fileContents;

    static volatile Properties prop;

    static Logger logListener;

    static {

        try {
            // look for log4j.xml on classpath
            InputStream is = LoggerFactory.class.getClassLoader().getResourceAsStream("log4j.xml");
            if (is != null) {
                loggerMode = LOGGER_LOG4J;
                is.close();
            }
            else {
                loggerMode = LOGGER_SIMPLE;

                // start a thread to flush this log every half second
                exec.execute(new Runnable() {
                    public void run() {
                        while (true) {
                            try {
                                SimpleFileLogger.doFlush();
                            } catch (Exception e) {
                                // ignore
                            }
                            Sleep.ms(500); //TODO: make configurable?
                        }
                    }
                });
            }
        }
        catch (Throwable th) {
            loggerMode = LOGGER_SIMPLE;
        }

    }

    public void setLogListener(Logger logListener) {
        LoggerFactory.logListener = logListener;
    }

    public static Logger getLogger(Class c) {
        return getLogger(c.getName());
    }

    public static Logger getLogger(String c) {

        if (loggerMode == LOGGER_LOG4J) {
            return new Log4jWrapper(org.apache.log4j.Logger.getLogger(c));
        }
        else {

            SimpleFileLogger logger = null;
            synchronized (loggerMap) {
                logger = loggerMap.get(c);
                if (logger==null) {
                    logger = new SimpleFileLogger(c);
                    doConfigure(logger);
                    loggerMap.put(c, logger);
                }
            }
            return logger;

        }
    }

    public static void configure(File propFile) {
        configure(propFile, false);
    }

    public static void configure(final File _propFile, final boolean watchAndReload) {
        propFile = _propFile;
        try {
            _logger.info("Configuring logging from " + propFile.getAbsolutePath());
            fileContents = FileUtil.readFile(propFile);
            final ByteArrayInputStream is = new ByteArrayInputStream(fileContents.getBytes());
            configure(is);
        } catch (IOException e) {
            getLogger(LoggerFactory.class).error("Error", e);
        }

        synchronized (Logger.class) {
            if (execReloadConfig == null && watchAndReload) {
                execReloadConfig = Executors.newSingleThreadExecutor();
                execReloadConfig.execute(new Runnable() {
                    public void run() {
                        while (true) {
                            Sleep.ms(Sleep.ONE_MINUTE);
                            try {
                                final String newFileContents = FileUtil.readFile(propFile);
                                if (!newFileContents.equals(fileContents)) {
                                    _logger.info("Reloading log properties from " + propFile.getAbsolutePath());
                                    fileContents = newFileContents;
                                    final ByteArrayInputStream is = new ByteArrayInputStream(fileContents.getBytes());
                                    configure(is);
                                }
                            } catch (Throwable e) {
                                _logger.error("Failed to reload properties file", e);
                            }

                        }
                    }
                });
            }
        }
    }

    public static void configure(InputStream is) {

        try {               
            prop = new Properties();
            prop.load(is);
            is.close();

            configure(prop);
        } catch (Throwable e) {
           getLogger(LoggerFactory.class).error("Error", e);
        }

    }

    public static void configure(Properties _prop) {

        prop = _prop;

        try {
            synchronized (loggerMap) {
                for (SimpleFileLogger logger : loggerMap.values()) {
                    doConfigure(logger);
                }
            }

        } catch (Exception e) {
            getLogger(LoggerFactory.class).error("Error configuring logging", e);
        }
    }

    private static void doConfigure(SimpleFileLogger logger) {

        if (TRACE) {
            _logger.trace("doConfigure() logger = " + logger.getClassName());
        }

        if (prop==null) {
            return;
        }
        
        // Get any log.file property.
        String logFileName = prop.getProperty("log.file");
        if(logFileName != null && logFileName.length() > 0) {
        	File logFile = new File(logFileName);
        	// Create the directories if required.
        	if(logFile.getParentFile() != null) {
        		logFile.getParentFile().mkdirs();
        	}
        	
        	setLogOutput(logFile);
        }

        TreeSet<String> set = new TreeSet<String>();


        Enumeration en = prop.keys();
        while (en.hasMoreElements()) {
            String key = (String) en.nextElement();

            // Skip the log.file key.
            if(key.equals("log.file")) {
            	continue;
            }
            
            set.add(key);
        }


        String spec;
        Iterator<String> it = set.iterator();
        while (it.hasNext()) {
            spec = it.next();

            if (TRACE) {
                _logger.trace("spec=" + spec);
            }

            if (logger.className.startsWith(spec)) {

                String value = prop.getProperty(spec);

                if (value==null) {
                    value = "INFO";
                }
                if (value.equals("TRACE")) {
                    logger.setLevel(SimpleFileLogger.LEVEL_TRACE);
                }
                if (value.equals("DEBUG")) {
                    logger.setLevel(SimpleFileLogger.LEVEL_DEBUG);
                }
                if (value.equals("INFO")) {
                    logger.setLevel(SimpleFileLogger.LEVEL_INFO);
                }
                if (value.equals("WARN")) {
                    logger.setLevel(SimpleFileLogger.LEVEL_WARN);
                }
                if (value.equals("ERROR")) {
                    logger.setLevel(SimpleFileLogger.LEVEL_ERROR);
                }

            }
        }

        if (TRACE) {
            _logger.trace("Logger: " + logger.className + " level set to " + logger.getLevelName());
        }
    }

    public static void setLogOutput(File f) {
        SimpleFileLogger.changeLogOutput(f);
    }

    public static void setLogOutput(File f, boolean verbose) {
        SimpleFileLogger.changeLogOutput(f, verbose);
    }


}
