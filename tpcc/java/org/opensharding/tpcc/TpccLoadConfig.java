package org.opensharding.tpcc;

import org.opensharding.tpcc.load.CSVLoader;
import org.opensharding.tpcc.load.JdbcLoader;
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

    private LoadType loadType = LoadType.CSV;

    private Connection conn;

    public RecordLoader createLoader(String tableName, String columnName[]) throws IOException {
        switch (loadType) {
            case JDBC:
                return new JdbcLoader(conn, tableName, columnName, 100);
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
