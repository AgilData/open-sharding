package org.opensharding.jdbc.impl;

import java.sql.*;
import java.util.Properties;

public class ConnectionFacade implements Connection {

	protected Connection delegate;
    
    protected CallLogger callLogger;
    
    protected int connectionID;
    
    protected static int nextConnectionID = 1;

    protected static final Object connectionIDMonitor = new Object();

    protected static int nextStatementID = 1;

    protected static final Object statementIDMonitor = new Object();

    public ConnectionFacade(Connection delegate, CallLogger callLogger) {
        this.delegate = delegate;
        this.callLogger = callLogger;
        
        synchronized (connectionIDMonitor) {
            connectionID = nextConnectionID++;
        }

        if (delegate == null) {
            throw new IllegalStateException("delegate can not be null");
        }
    }
    
    //TODO: implement new JDK 1.6 methods
    
    /**
     * Generate globally unique statementID values.
     * @return
     */
    public static int getNextStatementID() {
        synchronized (statementIDMonitor) {
            return nextStatementID++;
        }
    }


    public Clob createClob() throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public Blob createBlob() throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public NClob createNClob() throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public SQLXML createSQLXML() throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public boolean isValid(int timeout) throws SQLException {
        return false;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setClientInfo(String name, String value) throws SQLClientInfoException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setClientInfo(Properties properties) throws SQLClientInfoException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public String getClientInfo(String name) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public Properties getClientInfo() throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public Array createArrayOf(String typeName, Object[] elements) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public Struct createStruct(String typeName, Object[] attributes) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
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
		callLogger.log(connectionID, 0, "Connection","clearWarnings()", __start, __finish);
	}

	public java.sql.Statement createStatement(int p0,int p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Statement __ret = delegate.createStatement( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
        final int statementID = getNextStatementID();getNextStatementID();
        callLogger.log(connectionID, statementID, "Connection","createStatement(int,int,int)",new Object[] {p0,p1,p2}, __start, __finish);
        return new StatementFacade(connectionID, statementID, __ret, callLogger);
	}

	public java.sql.Statement createStatement(int p0,int p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Statement __ret = delegate.createStatement( p0, p1);
		final long __finish = System.currentTimeMillis();
        final int statementID = getNextStatementID();getNextStatementID();
		callLogger.log(connectionID, statementID, "Connection","createStatement(int,int)",new Object[] {p0,p1}, __start, __finish);
        return new StatementFacade(connectionID, statementID, __ret, callLogger);
	}

	public java.sql.Statement createStatement() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Statement __ret = delegate.createStatement();
		final long __finish = System.currentTimeMillis();
        final int statementID = getNextStatementID();getNextStatementID();
		callLogger.log(connectionID, statementID, "Connection","createStatement()", __start, __finish);
        return new StatementFacade(connectionID, statementID, __ret, callLogger);
	}

	public boolean getAutoCommit() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.getAutoCommit();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","getAutoCommit() returning " + __ret, __start, __finish);
		return __ret;
	}

	public java.lang.String getCatalog() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getCatalog();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","getCatalog()", __start, __finish);
		return __ret;
	}

	public int getHoldability() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getHoldability();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","getHoldability()", __start, __finish);
		return __ret;
	}

	public java.sql.DatabaseMetaData getMetaData() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.DatabaseMetaData __ret = delegate.getMetaData();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","getMetaData()", __start, __finish);
		return new DatabaseMetaDataFacade(connectionID, __ret, callLogger);
	}

	public int getTransactionIsolation() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getTransactionIsolation();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","getTransactionIsolation()", __start, __finish);
		return __ret;
	}

	public java.util.Map getTypeMap() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.util.Map __ret = delegate.getTypeMap();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","getTypeMap()", __start, __finish);
		return __ret;
	}

	public java.sql.SQLWarning getWarnings() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.SQLWarning __ret = delegate.getWarnings();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","getWarnings()", __start, __finish);
		return __ret;
	}

	public java.lang.String nativeSQL(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.nativeSQL( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","nativeSQL(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.CallableStatement prepareCall(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.CallableStatement __ret = delegate.prepareCall( p0);
		final long __finish = System.currentTimeMillis();
        final int statementID = getNextStatementID();getNextStatementID();
		callLogger.log(connectionID, statementID, "Connection","prepareCall(java.lang.String)",new Object[] {p0}, __start, __finish);
		return new CallableStatementFacade(connectionID, statementID, __ret, callLogger);
	}

	public java.sql.CallableStatement prepareCall(java.lang.String p0,int p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.CallableStatement __ret = delegate.prepareCall( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
        final int statementID = getNextStatementID();getNextStatementID();
		callLogger.log(connectionID, statementID, "Connection","prepareCall(java.lang.String,int,int)",new Object[] {p0,p1,p2}, __start, __finish);
        return new CallableStatementFacade(connectionID, statementID, __ret, callLogger);
	}

	public java.sql.CallableStatement prepareCall(java.lang.String p0,int p1,int p2,int p3) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.CallableStatement __ret = delegate.prepareCall( p0, p1, p2, p3);
		final long __finish = System.currentTimeMillis();
        final int statementID = getNextStatementID();getNextStatementID();
		callLogger.log(connectionID, statementID, "Connection","prepareCall(java.lang.String,int,int,int)",new Object[] {p0,p1,p2,p3}, __start, __finish);
        return new CallableStatementFacade(connectionID, statementID, __ret, callLogger);
	}

	public java.sql.PreparedStatement prepareStatement(java.lang.String p0,int p1,int p2,int p3) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.PreparedStatement __ret = delegate.prepareStatement( p0, p1, p2, p3);
		final long __finish = System.currentTimeMillis();
        final int statementID = getNextStatementID();getNextStatementID();
        callLogger.log(connectionID, statementID, "Connection","prepareStatement(java.lang.String,int,int,int)",new Object[] {p0,p1,p2,p3}, __start, __finish);
        return new PreparedStatementFacade(connectionID, statementID, __ret, callLogger);
	}

	public java.sql.PreparedStatement prepareStatement(java.lang.String p0,int p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.PreparedStatement __ret = delegate.prepareStatement( p0, p1);
		final long __finish = System.currentTimeMillis();
        final int statementID = getNextStatementID();getNextStatementID();
		callLogger.log(connectionID, statementID, "Connection","prepareStatement(java.lang.String,int)",new Object[] {p0,p1}, __start, __finish);
        return new PreparedStatementFacade(connectionID, statementID, __ret, callLogger);
	}

	public java.sql.PreparedStatement prepareStatement(java.lang.String p0,int[] p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.PreparedStatement __ret = delegate.prepareStatement( p0, p1);
		final long __finish = System.currentTimeMillis();
        final int statementID = getNextStatementID();getNextStatementID();
		callLogger.log(connectionID, statementID, "Connection","prepareStatement(java.lang.String,int[])",new Object[] {p0,p1}, __start, __finish);
        return new PreparedStatementFacade(connectionID, statementID, __ret, callLogger);
	}

	public java.sql.PreparedStatement prepareStatement(java.lang.String p0,int p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.PreparedStatement __ret = delegate.prepareStatement( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
        final int statementID = getNextStatementID();getNextStatementID();
		callLogger.log(connectionID, statementID, "Connection","prepareStatement(java.lang.String,int,int)",new Object[] {p0,p1,p2}, __start, __finish);
        return new PreparedStatementFacade(connectionID, statementID, __ret, callLogger);
	}

	public java.sql.PreparedStatement prepareStatement(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.PreparedStatement __ret = delegate.prepareStatement( p0);
		final long __finish = System.currentTimeMillis();
        final int statementID = getNextStatementID();getNextStatementID();
		callLogger.log(connectionID, statementID, "Connection","prepareStatement(java.lang.String)",new Object[] {p0}, __start, __finish);
        return new PreparedStatementFacade(connectionID, statementID, __ret, callLogger);
	}

	public java.sql.PreparedStatement prepareStatement(java.lang.String p0,java.lang.String[] p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.PreparedStatement __ret = delegate.prepareStatement( p0, p1);
		final long __finish = System.currentTimeMillis();
        final int statementID = getNextStatementID();getNextStatementID();
		callLogger.log(connectionID, statementID, "Connection","prepareStatement(java.lang.String,java.lang.String[])",new Object[] {p0,p1}, __start, __finish);
        return new PreparedStatementFacade(connectionID, statementID, __ret, callLogger);
	}

	public void releaseSavepoint(java.sql.Savepoint p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.releaseSavepoint( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","releaseSavepoint(java.sql.Savepoint)",new Object[] {p0}, __start, __finish);
	}

	public void rollback() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.rollback();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","rollback()", __start, __finish);
	}

	public void rollback(java.sql.Savepoint p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.rollback( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","rollback(java.sql.Savepoint)",new Object[] {p0}, __start, __finish);
	}

	public void setAutoCommit(boolean p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setAutoCommit( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","setAutoCommit(boolean)",new Object[] {p0}, __start, __finish);
	}

	public void setCatalog(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setCatalog( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","setCatalog(java.lang.String)",new Object[] {p0}, __start, __finish);
	}

	public void setHoldability(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setHoldability( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","setHoldability(int)",new Object[] {p0}, __start, __finish);
	}

	public java.sql.Savepoint setSavepoint(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Savepoint __ret = delegate.setSavepoint( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","setSavepoint(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Savepoint setSavepoint() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Savepoint __ret = delegate.setSavepoint();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","setSavepoint()", __start, __finish);
		return __ret;
	}

	public void setTransactionIsolation(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setTransactionIsolation( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","setTransactionIsolation(int)",new Object[] {p0}, __start, __finish);
	}

	public void setTypeMap(java.util.Map p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setTypeMap( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","setTypeMap(java.util.Map)",new Object[] {p0}, __start, __finish);
	}

	public void setReadOnly(boolean p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setReadOnly( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","setReadOnly(boolean)",new Object[] {p0}, __start, __finish);
	}

	public boolean isReadOnly() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.isReadOnly();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","isReadOnly()", __start, __finish);
		return __ret;
	}

	public void close() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.close();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","close()", __start, __finish);
	}

	public boolean isClosed() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.isClosed();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","isClosed()", __start, __finish);
		return __ret;
	}

	public void commit() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.commit();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "Connection","commit()", __start, __finish);
	}

}
