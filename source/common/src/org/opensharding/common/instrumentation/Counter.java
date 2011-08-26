package org.opensharding.common.instrumentation;

public class Counter {

    private long count = 0;
    private long total = 0;

    public synchronized long increment() {
        count++;
        total++;
        return total;
    }

    public synchronized long get() {
        return count;
    }

    public synchronized long reset() {
        long ret = count;
        count = 0;
        return ret;
    }

    public long getTotal() {
        return total;
    }

    public String toCSV() {
        return "" + count + "," + total;
    }
}
