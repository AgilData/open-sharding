package org.opensharding.tpcc.load;

import java.sql.Connection;
import java.sql.Statement;

/**
 * Copyright (C) 2011 CodeFutures Corporation. All rights reserved.
 */
public class JdbcLoader implements RecordLoader {

    Connection conn;

    Statement stmt;

    String tableName;

    String columnName[];

    int maxBatchSize;

    int currentBatchSize;

    StringBuilder b = new StringBuilder();

    public JdbcLoader(Connection conn, String tableName, String columnName[], int maxBatchSize) {
        this.conn = conn;
        this.tableName = tableName;
        this.columnName = columnName;
        this.maxBatchSize = maxBatchSize;
    }

    public void load(Record r) throws Exception {
        if (currentBatchSize==0) {
            b.append("INSERT INTO ").append(tableName).append(" (");
            for (int i=0; i<columnName.length; i++) {
                if (i>0) {
                    b.append(",");
                }
                b.append(columnName[i]);
            }
            b.append(" ) VALUES ");
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

    public void commit() throws Exception {
        conn.commit();
    }

    public void close() throws Exception {
        if (currentBatchSize>0) {
            stmt.execute(b.toString());
        }
        stmt.close();

        conn.commit();
    }
}
