package org.opensharding.jdbc.impl;

import java.sql.SQLException;
import java.sql.Statement;

public class StatementFacade implements Statement {

    protected CallLogger callLogger;
    
	protected Statement delegate;

    protected int connectionID;
    
    protected int statementID;
    

    public StatementFacade(int connectionID, int statementID, Statement delegate, CallLogger callLogger) {
        this.connectionID = connectionID;
        this.statementID = statementID;
        this.delegate = delegate;
        this.callLogger = callLogger;
    }

    public boolean isClosed() throws SQLException {
        return false;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setPoolable(boolean poolable) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public boolean isPoolable() throws SQLException {
        return false;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public <T> T unwrap(Class<T> iface) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public boolean isWrapperFor(Class<?> iface) throws SQLException {
        return false;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void clearWarnings() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.clearWarnings();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","clearWarnings()", __start, __finish);
	}

	public java.sql.SQLWarning getWarnings() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.SQLWarning __ret = delegate.getWarnings();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","getWarnings()", __start, __finish);
		return __ret;
	}

	public void addBatch(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.addBatch( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","addBatch(java.lang.String)",new Object[] {p0}, __start, __finish);
	}

	public void clearBatch() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.clearBatch();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","clearBatch()", __start, __finish);
	}

	public int[] executeBatch() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int[] __ret = delegate.executeBatch();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","executeBatch()", __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet executeQuery(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.executeQuery( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","executeQuery(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret==null ? null : new ResultSetFacade(connectionID, statementID, __ret, callLogger);
	}

	public int executeUpdate(java.lang.String p0,int p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.executeUpdate( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","executeUpdate(java.lang.String,int)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public int executeUpdate(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.executeUpdate( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","executeUpdate(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public int executeUpdate(java.lang.String p0,java.lang.String[] p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.executeUpdate( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","executeUpdate(java.lang.String,java.lang.String[])",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public int executeUpdate(java.lang.String p0,int[] p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.executeUpdate( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","executeUpdate(java.lang.String,int[])",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public int getFetchDirection() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getFetchDirection();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","getFetchDirection()", __start, __finish);
		return __ret;
	}

	public int getFetchSize() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getFetchSize();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","getFetchSize()", __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getGeneratedKeys() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getGeneratedKeys();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","getGeneratedKeys()", __start, __finish);
		return __ret==null ? null : new ResultSetFacade(connectionID, statementID, __ret, callLogger);
	}

	public int getMaxFieldSize() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxFieldSize();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","getMaxFieldSize()", __start, __finish);
		return __ret;
	}

	public int getMaxRows() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxRows();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","getMaxRows()", __start, __finish);
		return __ret;
	}

	public boolean getMoreResults(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.getMoreResults( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","getMoreResults(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean getMoreResults() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.getMoreResults();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","getMoreResults()", __start, __finish);
		return __ret;
	}

	public int getQueryTimeout() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getQueryTimeout();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","getQueryTimeout()", __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getResultSet() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getResultSet();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","getResultSet()", __start, __finish);
		return __ret==null ? null : new ResultSetFacade(connectionID, statementID, __ret, callLogger);
	}

	public int getResultSetConcurrency() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getResultSetConcurrency();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","getResultSetConcurrency()", __start, __finish);
		return __ret;
	}

	public int getResultSetHoldability() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getResultSetHoldability();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","getResultSetHoldability()", __start, __finish);
		return __ret;
	}

	public int getResultSetType() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getResultSetType();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","getResultSetType()", __start, __finish);
		return __ret;
	}

	public int getUpdateCount() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getUpdateCount();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","getUpdateCount()", __start, __finish);
		return __ret;
	}

	public void setCursorName(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setCursorName( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","setCursorName(java.lang.String)",new Object[] {p0}, __start, __finish);
	}

	public void setEscapeProcessing(boolean p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setEscapeProcessing( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","setEscapeProcessing(boolean)",new Object[] {p0}, __start, __finish);
	}

	public void setFetchDirection(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setFetchDirection( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","setFetchDirection(int)",new Object[] {p0}, __start, __finish);
	}

	public void setFetchSize(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setFetchSize( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","setFetchSize(int)",new Object[] {p0}, __start, __finish);
	}

	public void setMaxFieldSize(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setMaxFieldSize( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","setMaxFieldSize(int)",new Object[] {p0}, __start, __finish);
	}

	public void setMaxRows(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setMaxRows( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","setMaxRows(int)",new Object[] {p0}, __start, __finish);
	}

	public void setQueryTimeout(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setQueryTimeout( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","setQueryTimeout(int)",new Object[] {p0}, __start, __finish);
	}

	public void close() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.close();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","close()", __start, __finish);
	}

	public void cancel() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.cancel();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","cancel()", __start, __finish);
	}

	public java.sql.Connection getConnection() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Connection __ret = delegate.getConnection();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","getConnection()", __start, __finish);
		return new ConnectionFacade(__ret, callLogger);
	}

	public boolean execute(java.lang.String p0,int[] p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.execute( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","execute(java.lang.String,int[])",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public boolean execute(java.lang.String p0,java.lang.String[] p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.execute( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","execute(java.lang.String,java.lang.String[])",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public boolean execute(java.lang.String p0,int p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.execute( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","execute(java.lang.String,int)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public boolean execute(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.execute( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "Statement","execute(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

}
