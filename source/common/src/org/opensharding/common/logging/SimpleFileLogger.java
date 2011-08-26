package org.opensharding.common.logging;

import java.io.*;
import java.text.DateFormat;
import java.util.Date;
import java.util.Properties;
import java.util.concurrent.*;

/**
 * Logger that outputs to stdout or file.
 *
 * @author Andy Grove
 */
public final class SimpleFileLogger implements Logger {

    public static final int LEVEL_TRACE = 1;
    public static final int LEVEL_DEBUG = 2;
    public static final int LEVEL_INFO = 3;
    public static final int LEVEL_WARN = 4;
    public static final int LEVEL_ERROR = 5;

    protected static final DateFormat sdf = DateFormat.getDateTimeInstance();

    protected volatile int level = LEVEL_INFO; // default to INFO

    protected volatile static boolean active = true;
    
    private boolean isTraceEnabled = false;
    private boolean isDebugEnabled = false;
    private boolean isInfoEnabled = true;
    private boolean isWarnEnabled = true;
    private boolean isErrorEnabled = true;

    protected static Properties prop;

    static int count = 0;

    static final int flushInterval = 10;

    static File logFile;
    static PrintWriter logWriter;

    //TODO: make configurable
    final static int MAX_LOG_FILE_SIZE = 50 * 1024 * 1024;
    final static int MAX_LOG_FILE_COUNT = 10;

    static int currentLogFileSize = 0;

    //static final BlockingQueue<String> queue = new ArrayBlockingQueue<String>(1000);

    final static Object monitor = new Object();

    static {

        // add shutdown hook to delete PID
       Runtime.getRuntime().addShutdownHook(new Thread() {
           @Override
           public void run() {
               try {
                   shutdown();
               } catch (Throwable th) {
                   System.err.println("Error closing log: " + th.getClass().getName() + ": " + th.getMessage());
               }
           }
       });


    }

    static ThreadLocal threadName = new ThreadLocal() {
        protected synchronized Object initialValue() {
            final String threadName = Thread.currentThread().getName();
            return threadName==null ? "N/A" : threadName;
        }
    };

    static final ExecutorService logWriterExec = Executors.newSingleThreadExecutor(new ThreadFactory(){
        public Thread newThread(Runnable runnable) {
            return new Thread(runnable, "cfcommons-SimpleFileLogger");
        }
    });

    String className;

    static final SimpleFileLogger INSTANCE = new SimpleFileLogger(SimpleFileLogger.class.getName());

    public SimpleFileLogger(String className) {
        this.className = className;
    }

    public String getClassName() {
        return className;
    }

    public void setLevel(int level) {

        this.level = level;

        isTraceEnabled = level <= LEVEL_TRACE;
        isDebugEnabled = level <= LEVEL_DEBUG;
        isInfoEnabled = level <= LEVEL_INFO;
        isWarnEnabled = level <= LEVEL_WARN;
        isErrorEnabled = level <= LEVEL_ERROR;
}

    public int getLevel() {
        return level;
    }
    
    public boolean isTraceEnabled() {
        return isTraceEnabled;
    }

    public boolean isDebugEnabled() {
        return isDebugEnabled;
    }

    public boolean isInfoEnabled() {
        return isInfoEnabled;
    }

    public String getLevelName() {
        switch (level) {
            case LEVEL_TRACE:
                return "TRACE";
            case LEVEL_DEBUG:
                return "DEBUG";
            case LEVEL_INFO:
                return "INFO";
            case LEVEL_WARN:
                return "WARN";
            case LEVEL_ERROR:
                return "ERROR";
            default:
                return "?????";
        }
    }

    public static void changeLogOutput(File f) {
        changeLogOutput(f, true);

    }
    public static void changeLogOutput(File f, boolean verbose) {
        try {
        	
        	if (f == null)
        		return;

            if (logFile!=null && logFile.getAbsolutePath().equals(f.getAbsolutePath())) {
                return;
            }
        	
            logFile = f;
            if (verbose) {
                System.out.println("** Change log output to " + f.getAbsolutePath());
                INSTANCE.log("INFO", "Changing log output to " + f.getAbsolutePath(), null);
            }

            synchronized (monitor) {
                if (logWriter != null) {
                    try {
                        logWriter.close();
                    } catch (Exception e) {
                    }
                }
                logWriter = new PrintWriter(new FileWriter(f, logFile.exists()));
            }

        } catch (IOException e) {
            System.err.println("Failed to change log output file");
        }
    }

    private synchronized void log(final String level, final Object message, final Throwable throwable) {

        final ThreadLocal _threadLocal = threadName;
        final String _threadName = _threadLocal==null ? "N/A" : (String) _threadLocal.get();

        if (!active) {
            final String logMessage = buildMessageString(level, _threadName, message, throwable);
            System.out.println(logMessage);
            return;
        }

        try {
            logWriterExec.execute(new Runnable() {
                public void run() {
                    final String logMessage = buildMessageString(level, _threadName, message, throwable);

                    PrintWriter _w = null;

                    synchronized (monitor) {

                        // if writing to file (rather than stdout), see if it is time to rollover the logs
                        if (logWriter!=null) {

                            currentLogFileSize += logMessage.length();

                            if (currentLogFileSize >= MAX_LOG_FILE_SIZE) {

                                // reset
                                currentLogFileSize = 0;

                                // close current log
                                logWriter.close();

                                // rename log file
                                File dir = logFile.getParentFile();
                                String name = logFile.getName();

                                File oldestLog = new File(dir, name + "." + MAX_LOG_FILE_COUNT);
                                if (oldestLog.exists()) {
                                    if (!oldestLog.delete()) {
                                        throw new RuntimeException("Error deleting " + oldestLog);
                                    }
                                }

                                for (int logNumber=MAX_LOG_FILE_COUNT-1; logNumber>=1; logNumber--) {
                                    File currentName = new File(dir, name + "." + logNumber);
                                    File newName = new File(dir, name + "." + (logNumber+1));

                                    if (currentName.exists() && !currentName.renameTo(newName)) {
                                        throw new RuntimeException("Error renaming " + currentName + " to " + newName);
                                    }
                                }

                                // rename current log to .1
                                File newName = new File(dir, name + ".1");
                                if (!logFile.renameTo(newName)) {
                                    throw new RuntimeException("Error renaming " + logFile + " to " + newName);
                                }

                                // create new log file
                                logFile = new File(dir, name);
                                try {
                                    logWriter = new PrintWriter(logFile);
                                } catch (FileNotFoundException e) {
                                    throw new RuntimeException("Failed to create log file", e);
                                }
                            }

                            _w = logWriter;
                        }
                    }

                    if (_w==null) {
                        System.out.println(logMessage);
                    }
                    else {
                        _w.println(logMessage);
                    }

                    // register error with global variable
                    if (level.charAt(0) == 'W') {
                        LoggerGlobalState.INSTANCE.warn(className, logMessage);
                    }
                    else if (level.charAt(0) == 'E') {
                        LoggerGlobalState.INSTANCE.error(className, logMessage);
                    }
                }

            });
        }
        catch (Throwable th) {
            System.out.println("Error logging message: " + message + " - " + th.getClass().getName() + ": " + th.getMessage());
        }
    }

    private String buildMessageString(String level, String _threadName, Object message, Throwable throwable) {
        final StringBuilder b = new StringBuilder();
        b.append("[");
        synchronized (sdf) {
            b.append(sdf.format(new Date()));
        }
        b.append("] [");
        b.append(level);
        b.append("] [");
        b.append(className);
        b.append("] [");
        b.append(_threadName);
        b.append("] ");
        b.append(message);

        if (throwable != null) {
            final StringWriter sw = new StringWriter();
            final PrintWriter pw = new PrintWriter(sw);
            throwable.printStackTrace(pw);
            pw.close();
            b.append(": ").append(sw.getBuffer().toString());

        }

        final String logMessage = b.toString();
        return logMessage;
    }

    public static void doFlush() {
        PrintWriter _w;
        synchronized (monitor) {
            _w = logWriter;
        }
        if (_w != null) {
            _w.flush();
        }
    }

    public void trace(Object o) {
        if (!isTraceEnabled) return;
        log("TRACE", o, null);
    }

    public void trace(Object o, Throwable throwable) {
        if (!isTraceEnabled) return;
        log("TRACE", o, throwable);
    }

    public void debug(Object o) {
        if (!isDebugEnabled) return;
        log("DEBUG", o, null);
    }

    public void debug(Object o, Throwable throwable) {
        if (!isDebugEnabled) return;
        log("DEBUG", o, throwable);
    }

    public void info(Object o) {
        if (!isInfoEnabled) return;
        log("INFO", o, null);
    }

    public void info(Object o, Throwable throwable) {
        if (!isInfoEnabled) return;
        log("INFO", o, throwable);
    }

    public void warn(Object o) {
        if (!isWarnEnabled) return;
        log("WARN", o, null);
    }

    public void warn(Object o, Throwable throwable) {
        if (!isWarnEnabled) return;
        log("WARN", o, throwable);
    }

    public void error(Object o) {
        log("ERROR", o, null);
    }

    public void error(Object o, Throwable throwable) {
        log("ERROR", o, throwable);
    }

    public void fatal(Object o) {
        log("FATAL", o, null);
    }

    public void fatal(Object o, Throwable throwable) {
        log("FATAL", o, throwable);
    }

    protected static void shutdown() {
        try {
            synchronized (monitor) {
                active = false;
                logWriterExec.shutdown();
                if (logWriter != null) {
                    logWriter.println("Logging subsystem shutting down...");
                    logWriter.close();
                    logFile = null;
                }
            }
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }
}

