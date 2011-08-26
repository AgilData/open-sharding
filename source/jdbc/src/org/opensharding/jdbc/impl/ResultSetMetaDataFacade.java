package org.opensharding.jdbc.impl;

import java.sql.ResultSetMetaData;
import java.sql.SQLException;

public class ResultSetMetaDataFacade implements ResultSetMetaData {

    protected CallLogger callLogger;
	protected ResultSetMetaData delegate;

    protected int connectionID; 
    
    protected int statementID;
    
    public ResultSetMetaDataFacade(int connectionID, int statementID, ResultSetMetaData delegate, CallLogger callLogger) {
        this.connectionID = connectionID;
        this.statementID = statementID;
        this.delegate = delegate;
        this.callLogger = callLogger;
    }

    public <T> T unwrap(Class<T> iface) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public boolean isWrapperFor(Class<?> iface) throws SQLException {
        return false;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public java.lang.String getCatalogName(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getCatalogName( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","getCatalogName(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.lang.String getColumnClassName(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getColumnClassName( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","getColumnClassName(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public int getColumnDisplaySize(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getColumnDisplaySize( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","getColumnDisplaySize(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.lang.String getColumnLabel(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getColumnLabel( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","getColumnLabel(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public int getColumnType(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getColumnType( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","getColumnType(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.lang.String getColumnTypeName(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getColumnTypeName( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","getColumnTypeName(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public int getPrecision(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getPrecision( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","getPrecision(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public int getScale(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getScale( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","getScale(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.lang.String getSchemaName(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getSchemaName( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","getSchemaName(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.lang.String getTableName(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getTableName( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","getTableName(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean isAutoIncrement(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.isAutoIncrement( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","isAutoIncrement(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean isCaseSensitive(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.isCaseSensitive( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","isCaseSensitive(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean isCurrency(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.isCurrency( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","isCurrency(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean isDefinitelyWritable(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.isDefinitelyWritable( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","isDefinitelyWritable(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public int isNullable(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.isNullable( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","isNullable(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean isSearchable(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.isSearchable( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","isSearchable(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean isSigned(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.isSigned( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","isSigned(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean isWritable(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.isWritable( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","isWritable(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean isReadOnly(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.isReadOnly( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","isReadOnly(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public int getColumnCount() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getColumnCount();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","getColumnCount()", __start, __finish);
		return __ret;
	}

	public java.lang.String getColumnName(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getColumnName( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSetMetaData","getColumnName(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

}
