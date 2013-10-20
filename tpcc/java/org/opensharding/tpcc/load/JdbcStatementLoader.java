package org.opensharding.tpcc.load;

import java.sql.Connection;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Date;

/**
 * Copyright (C) 2011 CodeFutures Corporation. All rights reserved.
 */
public class JdbcStatementLoader implements RecordLoader {

    Connection conn;

    Statement stmt;

    String tableName;

    String columnName[];

    boolean ignore;

    int maxBatchSize;

    int currentBatchSize;

    StringBuilder b = new StringBuilder();

    public JdbcStatementLoader(Connection conn, String tableName, String columnName[], boolean ignore, int maxBatchSize) {
        this.conn = conn;
        this.tableName = tableName;
        this.columnName = columnName;
        this.ignore = ignore;
        this.maxBatchSize = maxBatchSize;
    }

    public void load(Record r) throws Exception {
        if (currentBatchSize==0) {
            b.append("INSERT ");
            if (ignore) {
                b.append("IGNORE ");
            }
            b.append("INTO `").append(tableName).append("` (");
            for (int i=0; i<columnName.length; i++) {
                if (i>0) {
                    b.append(',');
                }
                b.append(columnName[i].trim());
            }
            b.append(") VALUES ");
        }
        else {
            b.append(',');
        }
        b.append('(');
        write(b, r, ",");
        b.append(')');

        if (++currentBatchSize==maxBatchSize) {
            executeBulkInsert();
        }
    }

    private void executeBulkInsert() throws SQLException {
        if (stmt == null) {
            stmt = conn.createStatement();
        }
        final String sql = b.toString();
        b.setLength(0);
        try {
            stmt.execute(sql);
        } catch (SQLException e) {
            throw new RuntimeException("Error loading into table '" + tableName + "' with SQL: " + sql, e);
        }
        currentBatchSize = 0;
    }

    public void write(StringBuilder b, Record r, String delim) throws Exception {
        b.setLength(0);
        final Object[] field = r.getField();
        for (int i=0; i< field.length; i++) {
            if (i>0) {
                b.append("\t");
            }
            if (field[i] instanceof Date) {
//                b.append("'").append(dateTimeFormat.format((Date)field[i])).append("'");
                b.append("'").append((Date)field[i]).append("'");
            }
            else if (field[i] instanceof String) {
                b.append("'").append(field[i]).append("'");
            }
            else {
                b.append((String)field[i]);
            }
        }
    }

    public void commit() throws Exception {
        if (!conn.getAutoCommit()) {
            conn.commit();
        }
    }

    public void close() throws Exception {
        if (currentBatchSize>0) {
            executeBulkInsert();
        }
        stmt.close();
        if (!conn.getAutoCommit()) {
            conn.commit();
        }
    }
}