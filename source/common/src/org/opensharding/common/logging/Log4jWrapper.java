package org.opensharding.common.logging;

/**
 * TODO: write javadoc
 *
 * @author Andy Grove
 */
public class Log4jWrapper implements Logger {

    private org.apache.log4j.Logger logger;

    public Log4jWrapper(org.apache.log4j.Logger logger) {
        this.logger = logger;
    }

    public boolean isTraceEnabled() {
        return logger.isTraceEnabled();
    }

    public boolean isDebugEnabled() {
        return logger.isDebugEnabled();
    }

    public boolean isInfoEnabled() {
        return logger.isInfoEnabled();
    }

    public void trace(Object o) {
        logger.trace(o);
    }

    public void trace(Object o, Throwable throwable) {
        logger.trace(o, throwable);
    }

    public void debug(Object o) {
        logger.debug(o);
    }

    public void debug(Object o, Throwable throwable) {
        logger.debug(o, throwable);
    }

    public void info(Object o) {
        logger.info(o);
    }

    public void info(Object o, Throwable throwable) {
        logger.info(o, throwable);
    }

    public void warn(Object o) {
        logger.warn(o);
    }

    public void warn(Object o, Throwable throwable) {
        logger.warn(o, throwable);
    }
    public void error(Object o) {
        logger.error(o);
    }

    public void error(Object o, Throwable throwable) {
        logger.error(o, throwable);
    }

    public void fatal(Object o) {
        logger.fatal(o);
    }

    public void fatal(Object o, Throwable throwable) {
        logger.fatal(o, throwable);
    }

}
