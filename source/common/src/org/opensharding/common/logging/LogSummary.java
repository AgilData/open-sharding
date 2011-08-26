package org.opensharding.common.logging;

public class LogSummary {

    private String loggerName;

    private String message;

    private long count;

    public LogSummary(String loggerName, String message, long count) {
        this.loggerName = loggerName;
        this.message = message;
        this.count = count;
    }

    public String getLoggerName() {
        return loggerName;
    }

    public String getMessage() {
        return message;
    }

    public long getCount() {
        return count;
    }
}
