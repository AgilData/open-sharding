package org.opensharding.tpcc.load;

import java.io.IOException;
import java.io.OutputStream;
import java.util.Date;

/**
 * Copyright (C) 2011 CodeFutures Corporation. All rights reserved.
 */
public class Record {

    private int index;

    private Object data[];

    final StringBuilder toStringBuilder = new StringBuilder();

    public Record(int columnCount) {
        this.data = new Object[columnCount];
    }

    public void reset() {
        index = 0;
    }

    public void add(Object value) {
        data[index++] = value;
    }

    public void write(OutputStream os, String delim) throws IOException {
        StringBuilder b = new StringBuilder();
        write(b, delim);
        os.write(b.toString().getBytes());
        os.write("\n".getBytes());
    }

    public void write(StringBuilder b, String delim) {
        for (int i=0; i<data.length; i++) {
            if (i>0) {
                b.append(delim);
            }
            boolean enquote = false;
            if (data[i] != null && (data[i] instanceof String || data[i] instanceof Date)) {
                enquote = true;
            }
            if (enquote) {
                b.append('\'');
            }
            b.append(data[i]);
            if (enquote) {
                b.append('\'');
            }
        }
    }

    public String toString(String delim) {
        toStringBuilder.setLength(0);
        write(toStringBuilder, delim);
        return toStringBuilder.toString();
    }
}
