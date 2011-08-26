package org.opensharding.common.util;

import org.opensharding.common.logging.*;

/**
 * Convenience class to remove need for try/catch blocks around Thread.sleep() throughout
 * the codebase.
 *
 */
public class Sleep {

    private static final Logger logger = LoggerFactory.getLogger(Sleep.class);

    public static final int ONE_SECOND = 1000;
    public static final int ONE_MINUTE = 60 * ONE_SECOND;
    public static final int ONE_HOUR = 60 * ONE_MINUTE;

    /**
     * Sleep for the specified number of milliseconds and log a warning if sleep is interuppted.
     *
     * @param time Time to sleep in milliseconds
     */
    public static void ms(long time) {
        try {
            Thread.sleep(time);
        } catch (InterruptedException e) {
            logger.warn("Sleep interrupted");
        }
    }

    public static void ms(int time) {
        ms((long)time);
    }
    
    /**
     * Sleep for the specified number of seconds and log a warning if sleep is interuppted.
     *
     * @param time Time to sleep in seconds
     */
    public static void seconds(int time) {
        ms(time*1000);
    }
}
