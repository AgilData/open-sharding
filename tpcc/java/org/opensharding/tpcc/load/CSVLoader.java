package org.opensharding.tpcc.load;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Copyright (C) 2011 CodeFutures Corporation. All rights reserved.
 */
public class CSVLoader implements RecordLoader {

    protected BufferedOutputStream os;

    protected final StringBuilder b = new StringBuilder();

    public CSVLoader(File file) throws IOException {
        this.os = new BufferedOutputStream(new FileOutputStream(file));
    }

    public void load(Record r) throws Exception {
        b.setLength(0);
        r.write(b, ",");
        os.write(b.toString().getBytes());
        os.write("\n".getBytes());
    }

    public void commit() throws Exception {
        // ignore
    }

    public void close() throws Exception {
        os.close();
    }
}
