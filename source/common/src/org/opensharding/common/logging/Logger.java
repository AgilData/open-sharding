package org.opensharding.common.logging;

/**
 * Wrapper for log4j so we can integrate with products that use older versions of log4j
 * that lack the isTraceEnabled method.
 *
 * @author Andy Grove
 */
public interface Logger {

    boolean isTraceEnabled();

    boolean isDebugEnabled();

    boolean isInfoEnabled();

    void debug(Object o);

    void debug(Object o, Throwable throwable);

    void trace(Object o);

    void trace(Object o, Throwable throwable);

    void error(Object o);

    void error(Object o, Throwable throwable);

    void info(Object o);

    void info(Object o, Throwable throwable);

    void warn(Object o, Throwable throwable);

    void warn(Object o);

    void fatal(Object o);

    void fatal(Object o, Throwable throwable);
}
