package org.opensharding.tpcc;

import org.opensharding.tpcc.load.CSVLoader;
import org.opensharding.tpcc.load.JdbcStatementLoader;
import org.opensharding.tpcc.load.RecordLoader;

import java.io.File;
import java.io.IOException;
import java.sql.Connection;

/**
 * Copyright (C) 2011 CodeFutures Corporation. All rights reserved.
 */
public class TpccLoadConfig {

    enum LoadType {
        JDBC,
        CSV
    }

    private LoadType loadType = LoadType.JDBC;

    private Connection conn;

    private boolean jdbcInsertIgnore = true;

    private int jdbcBatchSize = 100;

    public RecordLoader createLoader(String tableName, String columnName[]) throws IOException {
        switch (loadType) {
            case JDBC:
                return new JdbcStatementLoader(conn, tableName, columnName, jdbcInsertIgnore, jdbcBatchSize);
            case CSV:
                return new CSVLoader(new File("test_" + tableName + ".csv"));
            default:
                throw new IllegalStateException();
        }
    }

    public Connection getConn() {
        return conn;
    }

    public void setConn(Connection conn) {
        this.conn = conn;
    }

    public void setLoadType(LoadType loadType) {
        this.loadType = loadType;
    }

    public LoadType getLoadType() {
        return loadType;
    }
}
