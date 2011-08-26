package org.opensharding.common.logging;

/**
 * Implementation of dbShards Logger interface that delegates to log4j versions prior to 1.2.12
 * which did not support the isTraceEnabled() or trace() methods.
 *
 * @author Andy Grove
 */
public class LegacyLog4jWrapper implements Logger {

    private org.apache.log4j.Logger logger;

    public LegacyLog4jWrapper(org.apache.log4j.Logger logger) {
        this.logger = logger;
    }

    public boolean isTraceEnabled() {
        return false;
    }

    public void trace(Object o) {
        // not supported
    }

    public void trace(Object o, Throwable throwable) {
        // not supported
    }

    public boolean isDebugEnabled() {
        return logger.isDebugEnabled();
    }

    public boolean isInfoEnabled() {
        return logger.isInfoEnabled();
    }

    public void debug(Object o) {
        logger.debug(o);
    }

    public void debug(Object o, Throwable throwable) {
        logger.debug(o, throwable);
    }

    public void error(Object o) {
        logger.error(o);
    }

    public void error(Object o, Throwable throwable) {
        logger.error(o, throwable);
    }

    public void info(Object o) {
        logger.info(o);
    }

    public void info(Object o, Throwable throwable) {
        logger.info(o, throwable);
    }

    public void warn(Object o, Throwable throwable) {
        logger.warn(o, throwable);
    }

    public void warn(Object o) {
        logger.warn(o);
    }

    public void fatal(Object o) {
        logger.fatal(o);
    }

    public void fatal(Object o, Throwable throwable) {
        logger.fatal(o, throwable);
    }
}
