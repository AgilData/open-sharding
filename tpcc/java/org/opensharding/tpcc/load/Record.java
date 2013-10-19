package org.opensharding.tpcc.load;

import java.util.Date;

/**
 * Simple object to represent a single row of data being loaded to the database (or written to a CSV file).
 */
public class Record {

    /**
     * Column values.
     */
    private final Object field[];

    /**
     * Index of next column to write value to.
     */
    private int index;

    /**
     * Re-usable buffer for building string representations of the row.
     */
    private final StringBuilder toStringBuilder = new StringBuilder();

    public Record(int columnCount) {
        this.field = new Object[columnCount];
    }

    public void reset() {
        index = 0;
    }

    public void add(Object value) {
        field[index++] = value;
    }

    public Object getField(int i) {
        return field[i];
    }

    public Object[] getField() {
        return field;
    }

    public void write(StringBuilder b, String delim) {
        for (int i=0; i< field.length; i++) {
            if (i>0) {
                b.append(delim);
            }
            boolean enquote = false;
            if (field[i] != null && (field[i] instanceof String || field[i] instanceof Date)) {
                enquote = true;
            }
            if (enquote) {
                b.append('\'');
            }
            b.append(field[i]);
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

    @Override
    public String toString() {
        return toString(",");
    }
}
