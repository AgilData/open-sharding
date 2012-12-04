package org.opensharding.tpcc;

/**
 * Copyright (C) 2011 CodeFutures Corporation. All rights reserved.
 */
public interface RecordProcessor {
    void process(Record r) throws Exception;

    void close() throws Exception;
}
