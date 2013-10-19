package org.opensharding.tpcc;

import org.opensharding.tpcc.load.RecordProcessor;

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

    private LoadType loadType;

    private Connection conn;

    public RecordProcessor createLoader(String tableName, String columnName[], int batchSize) throws IOException {
        return null;

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
