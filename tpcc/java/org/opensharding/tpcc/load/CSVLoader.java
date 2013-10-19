package org.opensharding.tpcc.load;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Copyright (C) 2011 CodeFutures Corporation. All rights reserved.
 */
public class CSVLoader implements RecordProcessor {

    protected BufferedOutputStream os;

    protected final StringBuilder b = new StringBuilder();

    public CSVLoader(File file) throws IOException {
        this.os = new BufferedOutputStream(new FileOutputStream(file));
    }

    public void process(Record r) throws Exception {
        r.write(b, ",");
    }

    public void close() throws Exception {
        os.close();
    }
}
