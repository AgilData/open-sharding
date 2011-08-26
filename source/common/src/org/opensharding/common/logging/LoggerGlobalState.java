package org.opensharding.common.logging;

import org.opensharding.common.instrumentation.Counter;

/**
 * Global variable to store latest error/warning and has counters.
 */
public class LoggerGlobalState {

    public static final LoggerGlobalState INSTANCE = new LoggerGlobalState();

    private Counter warningCounter = new Counter();
    
    private Counter errorCounter = new Counter();
    
    private String warnLoggerName;
    private String warnMessage;

    private String errorLoggerName;
    private String errorMessage;

    private LoggerGlobalState() {
    }
    
    public synchronized void warn(String loggerName, String message) {
        warningCounter.increment();
        warnLoggerName = loggerName;
        warnMessage = message;
    }

    public synchronized void error(String loggerName, String message) {
        errorCounter.increment();
        errorLoggerName = loggerName;
        errorMessage = message;
    }

    public synchronized LogSummary getWarningSummary() {
        return new LogSummary(warnLoggerName, warnMessage, warningCounter.getTotal());
    }

    public synchronized LogSummary getErrorSummary() {
        return new LogSummary(errorLoggerName, errorMessage, errorCounter.getTotal());
    }

}
