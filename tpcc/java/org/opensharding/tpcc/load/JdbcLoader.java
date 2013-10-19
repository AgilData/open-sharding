package org.opensharding.tpcc.load;

import java.sql.Connection;
import java.sql.Statement;

/**
 * Copyright (C) 2011 CodeFutures Corporation. All rights reserved.
 */
public class JdbcLoader implements RecordProcessor {

    Connection conn;

    Statement stmt;

    String insertStub;

    int maxBatchSize;

    int currentBatchSize;

    StringBuilder b = new StringBuilder();

    public JdbcLoader(Connection conn, String tableName, String columnName[], int maxBatchSize) {
        this.conn = conn;
        this.insertStub = insertStub;
        this.maxBatchSize = maxBatchSize;
    }

    public void process(Record r) throws Exception {
        if (currentBatchSize==0) {
            b.append(insertStub);
        }
        else {
            b.append(',');
        }
        b.append('(');
        r.write(b, ",");
        b.append(')');
        if (++currentBatchSize==maxBatchSize) {
            stmt.execute(b.toString());
            b.setLength(0);
            currentBatchSize = 0;
        }
    }

    public void close() throws Exception {
        if (currentBatchSize>0) {
            stmt.execute(b.toString());
        }
        stmt.close();
    }
}
