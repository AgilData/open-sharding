package org.opensharding.jdbc.impl;


import java.io.*;
import java.sql.SQLException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.*;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

import org.opensharding.common.logging.Logger;
import org.opensharding.common.logging.LoggerFactory;
import org.opensharding.common.util.ByteUtil;
import org.opensharding.common.util.StringUtil;

/**
 * CallLogger asynchronously but sequentially writes logging information to a log file and takes
 * care of log rotation tasks.
 *
 * @author Andy Grove
 * @created 15-Sep-2007 09:17:00
 */
public final class CallLogger {
	
	// Logger used for informational and error messages.
	private final static Logger logger = LoggerFactory.getLogger(CallLogger.class);

    public static final String logPrefix = "[Shard Analysis Logger] ";
    
    private static final String PROPFILENAME = "dbshards-analyze.properties";
    private static final String PROPFILEPATHENV = "DBS_ANALYZE_PROPS";
    
	// Maximum param length. If longer than 10K, the value is truncated.
	private final static int MAXPARAMLEN = 1024*10;


    /**
     * Configuration setting that determines whether actual data should be logged. If this is set to false
     * then only the SQL statements will be logged and not the actual data values bound to the statement
     * via PreparedStatement. However, any data values encoded directly in the SQL statement will still be
     * logged.
     */
    protected boolean logData=true;

    /**
     * Configuration setting that determines the maximum size of each log file in bytes.
     */
    private long maxFileSize = 10*1024*1024;

    /**
     * Configuration setting that determines the maximum number of log files to write. Once this limit
     * has been reached the logger will quietly stop working.
     */
    private long maxFiles = 100;

    /**
     * Maximum time to log, in hours, obtained from the log.maxtime property.
     * If logMaxHours set to 0, ignore.
     */
    private int logMaxHours = 0;

    /**
     * End time to cease logging, calculated from the log.maxtime property.
     */
    private long logEndTime = 0;

    /**
     * The log directory.
     */
    private String logDir = ".";

    /**
     * System file separator.
     */
    final static String FS = System.getProperty("file.separator");

    /**
     * Queue for storing log statements that have not yet been written to file.
     */
    private final BlockingQueue<LogInfo> queue = new ArrayBlockingQueue<LogInfo>(32768);

    /**
     * LogWriter is the Runnable class that takes items from the queue and writes them to the log file.
     */
    private LogWriter logWriter;

    /**
     * Thread that runs the logWriter.
     */
    private Thread logWriterThread;

    private boolean init = false;
    

    private DateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd_HH-mm-ss-SSSS");

    private static final Map<String,CallLogger> loggerMap = new HashMap<String, CallLogger>();

    /**
     * Static method to get a logger instance.
     * @param url
     * @param props
     * @return
     * @throws Exception
     */
    public static CallLogger getLogger(String url, Properties props) throws Exception {
        CallLogger logger;
        synchronized (loggerMap) {
            logger = loggerMap.get(url);
            if (logger == null) {
                logger = new CallLogger(url, props);
                loggerMap.put(url, logger);
            }
        }
        return logger;
    }

    /**
     * Private constructor for logger instance.
     * @param url
     * @param props
     */
    private CallLogger(String url, Properties props) {

        log("dbShards CallLogger init for: " + url);

        synchronized (CallLogger.class) {
            if (!init) {

                try {

                	if(props == null) {
                		throw new Exception("The props parameter is required for Call Logger init.");
                	}
                    configure(props);
 
                }
                catch (Throwable e) {
                    log("constructor failed", e);
                }

                if (logMaxHours > 0) {
                    Calendar c = Calendar.getInstance();
                    c.add(Calendar.HOUR, logMaxHours);
                    logEndTime = c.getTimeInMillis();
                }

                try {
                    logWriter = new LogWriter();
                    logWriterThread = new Thread(logWriter);
                    logWriterThread.setDaemon(true);
                    logWriterThread.start();
                } catch (Throwable e) {
                    log("CallLogger ctor failed", e);
                    throw new ExceptionInInitializerError(e);
                }
            }

            init = true;
        }
    }

    public void configure(Properties props) throws Exception {

        String value = props.getProperty("shard.analyze.log.data");
        log("configure: shard.analyze.log.data: " + value);
        logData = (value != null && value.equalsIgnoreCase("true"));

        value = props.getProperty("shard.analyze.log.limit");
        if (value != null) {
            maxFileSize = Integer.parseInt(value);
        }

        value = props.getProperty("shard.analyze.log.maxfiles");
        if (value != null) {
            maxFiles = Integer.parseInt(value);
        }

        value = props.getProperty("shard.analyze.log.maxhours");
        if (value != null) {
            logMaxHours = Integer.parseInt(value);
        }

        value = props.getProperty("shard.analyze.log.dir");
        if (value != null) {
            logDir = value;

            /* Ensure the logDir ends with a file separator.*/
            if (!logDir.endsWith(FS)) {
                logDir = logDir + FS;
            }

            log("Changing log output directory to " + logDir);
            
        	// Create the logDir if required.
        	File dir = new File(logDir);
        	if(!dir.exists()) {
        		dir.mkdirs();
        	}

        } else {
            throw new Exception("The shard.analyze.log.dir property is required.");
        }
        
    }

    private String getLogFilename() {
    	
    	// Return the filename.
        Calendar c = Calendar.getInstance();
        Date currentDate = c.getTime();
        File file = new File(logDir == null ? "." : logDir, "dbSAnalyze-" + dateFormat.format(currentDate) + ".log");
        return file.getAbsolutePath();
    }


    protected void finalize() throws Throwable {
        super.finalize();
        logWriter.requestStop();
    }

    public void log(final int connectionID, final int statementID, String className, String methodSignature, long start, long finish) {
        log(connectionID, statementID, className, methodSignature, null, start, finish);
    }

    public void log(final int connectionID, final int statementID, final String className, final String methodSignature, final Object param[], long start, long finish) {
        _log(connectionID, statementID, className, methodSignature, param, start, finish);
    }

    public void _log(final int connectionID, final int statementID, final String className, final String methodSignature, final Object param[], long start, long finish) {
        try {
            if (className.equals("ResultSet")) {
                // log would be too verbose if we log all result set calls
                return;
            }

            if (!logData) {
                if (className.endsWith("Statement") && methodSignature.startsWith("set")) {
                    return;
                }
            }

            queue.add(new LogInfo(Thread.currentThread().getId(), connectionID, statementID, className, methodSignature, param, start, finish));
        } catch (Throwable th) {
            // we definitely don't want the log call to ever throw an exception
            log("Failed to write message to log file", th);
        }
    }

    static class LogInfo {
        public long thread;
        public int connectionID;
        public int statementID;
        public String className;
        public String methodSignature;
        public Object param[];
        public long start;
        public long finish;

        public LogInfo(long thread, final int connectionID, final int statementID, String className, String methodSignature, Object[] param, long start, long finish) {
            this.thread = thread;
            this.connectionID = connectionID;
            this.statementID = statementID;
            this.className = className;
            this.methodSignature = methodSignature;
            this.param = param;
            this.start = start;
            this.finish = finish;
        }
    }

    class LogWriter implements Runnable {

        /**
         * Current PrintWriter.
         */
        private FileWriter w;
        private StringBuffer buf = new StringBuffer();
        private boolean stopRequested;
        private int currentFileSize;
        private int currentFile;

        public LogWriter() throws IOException {
        }

        public void run() {
            try {

                while (!stopRequested) {
                    write(queue.take());
                }

                if (w != null) {
                    w.close();
                }

            } catch (Throwable e) {
                log("LOG WRITER THREAD HAS DIED", e);
            }
        }

        private static final char QUOTE = '\"';

        public void write(Object o) throws IOException, SQLException {

            final LogInfo info = (LogInfo) o;

            buf.setLength(0);
            buf.append("[LOGSTART][");
            buf.append(info.thread);
            buf.append("][");
            buf.append(info.connectionID);
            buf.append("][");
            buf.append(info.statementID);
            buf.append("][");
            buf.append(info.start);
            buf.append("][");
            buf.append(info.finish - info.start);
            buf.append(']');
            buf.append(info.className);
            buf.append('.');
            buf.append(info.methodSignature);
            if (info.param != null) {
                buf.append('{');
                for (int i = 0; i < info.param.length; i++) {
                    Object value = info.param[i];
                    if (i > 0) {
                        buf.append(',');
                    }
                    if(value == null) {
                    	value = "null";
                    	
                    } else if (value instanceof String) {
                    	// Escape the string for nested QUOTE chars.
                    	String escapedStr = StringUtil.escape((String)value, "\"");
                        buf.append(QUOTE).append(escapedStr).append(QUOTE);
                        
                    } else if (value instanceof java.util.Date) {
                    	buf.append(((java.util.Date)value).getTime());
                    	
                    } else if (value instanceof java.sql.Date) {
                    	buf.append(((java.sql.Date)value).getTime());
                    	
                    } else if (value instanceof java.sql.Time) {
                    	buf.append(((java.sql.Time)value).getTime());

                    } else if (value instanceof java.sql.Timestamp) {
                    	buf.append(((java.sql.Timestamp)value).getTime());

                    } else if(value instanceof Calendar) {
                    	// There is no method to get the local for a calendar, so the default is always used.
                    	// The Timezone is the only important field.
                    	buf.append(QUOTE).append(((Calendar)value).getTimeZone().getID());
                    	
                    } else if(value instanceof java.sql.SQLXML) {
                    	String escapedStr = StringUtil.escape(((java.sql.SQLXML)value).getString(), "\"");
                    	buf.append(QUOTE).append(escapedStr).append(QUOTE);
                    	
                    } else if(value instanceof java.net.URL) {
                    	String escapedStr = StringUtil.escape(((java.net.URL)value).toString(), "\"");
                    	buf.append(QUOTE).append(escapedStr).append(QUOTE);
                    	
                    } else if(value instanceof java.sql.Blob) {
                		// Convert to hex.
                		String data = null;
                		if(((java.sql.Blob)value).length() <= MAXPARAMLEN) {
                			data =  ByteUtil.toHexString(((java.sql.Blob)value).getBytes(0, (int)((java.sql.Blob)value).length()));
                		} else {
                			data = ByteUtil.toHexString(((java.sql.Blob)value).getBytes(0, MAXPARAMLEN));
                		}
                		
                    } else if(value instanceof java.sql.Clob) {
                    	int clobLength = (int)((java.sql.Clob)value).length();
                    	String escapedStr = null;
                    	if(clobLength <= MAXPARAMLEN) {
                    		escapedStr =  StringUtil.escape(((java.sql.Clob)value).getSubString(0, clobLength), "\"");
                    	} else {
                    		escapedStr = StringUtil.escape(((java.sql.Clob)value).getSubString(0, MAXPARAMLEN), "\"");
                    	}
                    	buf.append(QUOTE).append(escapedStr).append(QUOTE);

                    } else if(value instanceof java.sql.NClob) {
                    	int clobLength = (int)((java.sql.NClob)value).length();
                    	String escapedStr = null;
                    	if(clobLength <= MAXPARAMLEN) {
                    		escapedStr =  StringUtil.escape(((java.sql.NClob)value).getSubString(0, clobLength), "\"");
                    	} else {
                    		escapedStr = StringUtil.escape(((java.sql.NClob)value).getSubString(0, MAXPARAMLEN), "\"");
                    	}
                    	buf.append(QUOTE).append(escapedStr).append(QUOTE);
                    	
                    } else if(value instanceof java.sql.RowId) {
                    	// RowID is output, but cannot be supported in replay.
                    	buf.append(value.toString());

                    } else {
                        buf.append(value.toString());
                    }
                    
                    // TODO: Not supported:
                    // Streams
                    // Readers
                    // For Streams and Readers, just output up to 10K of the value).
                    // Array
                    // Ref (may be infeasible)
                    // 
                }
                buf.append('}');
            }

            final String logString = buf.toString();

            currentFileSize += logString.length();

            /* If logEndTime has been reached, end logging.*/
            if (logEndTime > 0 && info.start >= logEndTime) {
                log("REACHED END TIME");
                stopRequested = true;
            }

            /* If the max file size is reached, rotate file or end logging.*/
            if (currentFileSize > maxFileSize || w==null) {

                /* If the max number of files has been hit, end logging.*/
                if (++currentFile == maxFiles) {
                    log("REACHED MAX FILES");
                    stopRequested = true;
                }
                else {

                    /* Else create/rotate the log file.*/
                    // simple log rotation
                    File file = new File(getLogFilename());
                    try {

                        if (w != null) {
                            w.flush();
                            w.close();
                        }

                        log("Writing to " + file.getAbsolutePath());
                        w = new FileWriter(file);

                    } catch (Exception e) {
                        log("FAILED TO ROTATE LOG", e);
                        stopRequested = true;
                    }
                    currentFileSize = 0;
                }
            }

            if (w!=null) {
                w.write(logString);
                w.write("\n[LOGEND]\n");
                w.flush();
            }
        }

        public void requestStop() {
            stopRequested = true;
        }
    }

    private static DateFormat df;

    public void log(String message) {
        log(message, null);
    }

    public void log(String message, Throwable th) {
//        StringBuilder b = new StringBuilder();
//        b.append(logPrefix);
//        if (df == null) {
//            df = new SimpleDateFormat("dd MMM yyyy HH:mm:ss");
//        }
//        synchronized (df) {
//            b.append(df.format(new Date()));
//        }
//        b.append(" > ").append(message);
//
//        if (th!=null) {
//            b.append(" :: Exception: ").append(th.getClass().getName()).append(": ").append(th.getMessage());
//        }

//        System.out.println(b.toString());
    	if(th != null) {
    		logger.error("Error", th);
    	} else {
            logger.info(message);
    	}

//        if (th != null) {
//            th.printStackTrace();
//        }
    }

}
