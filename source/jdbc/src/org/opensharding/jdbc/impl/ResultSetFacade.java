package org.opensharding.jdbc.impl;

import java.io.InputStream;
import java.io.Reader;
import java.sql.*;

public class ResultSetFacade implements ResultSet {

    protected CallLogger callLogger;
	protected ResultSet delegate;
    
    protected int connectionID;
    
    protected int statementID;

    public ResultSetFacade(int connectionID, int statementID, ResultSet delegate, CallLogger callLogger) {
        this.connectionID = connectionID;
        this.statementID = statementID;
        this.delegate = delegate;
        this.callLogger = callLogger;
    }

    public RowId getRowId(int columnIndex) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public RowId getRowId(String columnLabel) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateRowId(int columnIndex, RowId x) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateRowId(String columnLabel, RowId x) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public int getHoldability() throws SQLException {
        return 0;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public boolean isClosed() throws SQLException {
        return false;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateNString(int columnIndex, String nString) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateNString(String columnLabel, String nString) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateNClob(int columnIndex, NClob nClob) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateNClob(String columnLabel, NClob nClob) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public NClob getNClob(int columnIndex) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public NClob getNClob(String columnLabel) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public SQLXML getSQLXML(int columnIndex) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public SQLXML getSQLXML(String columnLabel) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateSQLXML(int columnIndex, SQLXML xmlObject) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateSQLXML(String columnLabel, SQLXML xmlObject) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public String getNString(int columnIndex) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public String getNString(String columnLabel) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public Reader getNCharacterStream(int columnIndex) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public Reader getNCharacterStream(String columnLabel) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateNCharacterStream(int columnIndex, Reader x, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateNCharacterStream(String columnLabel, Reader reader, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateAsciiStream(int columnIndex, InputStream x, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateBinaryStream(int columnIndex, InputStream x, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateCharacterStream(int columnIndex, Reader x, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateAsciiStream(String columnLabel, InputStream x, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateBinaryStream(String columnLabel, InputStream x, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateCharacterStream(String columnLabel, Reader reader, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateBlob(int columnIndex, InputStream inputStream, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateBlob(String columnLabel, InputStream inputStream, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateClob(int columnIndex, Reader reader, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateClob(String columnLabel, Reader reader, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateNClob(int columnIndex, Reader reader, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateNClob(String columnLabel, Reader reader, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateNCharacterStream(int columnIndex, Reader x) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateNCharacterStream(String columnLabel, Reader reader) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateAsciiStream(int columnIndex, InputStream x) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateBinaryStream(int columnIndex, InputStream x) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateCharacterStream(int columnIndex, Reader x) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateAsciiStream(String columnLabel, InputStream x) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateBinaryStream(String columnLabel, InputStream x) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateCharacterStream(String columnLabel, Reader reader) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateBlob(int columnIndex, InputStream inputStream) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateBlob(String columnLabel, InputStream inputStream) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateClob(int columnIndex, Reader reader) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateClob(String columnLabel, Reader reader) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateNClob(int columnIndex, Reader reader) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void updateNClob(String columnLabel, Reader reader) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
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
		callLogger.log(connectionID, statementID, "ResultSet","clearWarnings()", __start, __finish);
	}

	public java.sql.ResultSetMetaData getMetaData() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSetMetaData __ret = delegate.getMetaData();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getMetaData()", __start, __finish);
		return new ResultSetMetaDataFacade(connectionID, statementID, __ret, callLogger);
	}

	public java.sql.SQLWarning getWarnings() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.SQLWarning __ret = delegate.getWarnings();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getWarnings()", __start, __finish);
		return __ret;
	}

	public int getFetchDirection() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getFetchDirection();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getFetchDirection()", __start, __finish);
		return __ret;
	}

	public int getFetchSize() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getFetchSize();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getFetchSize()", __start, __finish);
		return __ret;
	}

	public void setFetchDirection(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setFetchDirection( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","setFetchDirection(int)",new Object[] {p0}, __start, __finish);
	}

	public void setFetchSize(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setFetchSize( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","setFetchSize(int)",new Object[] {p0}, __start, __finish);
	}

	public java.sql.Blob getBlob(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Blob __ret = delegate.getBlob( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getBlob(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Blob getBlob(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Blob __ret = delegate.getBlob( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getBlob(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Clob getClob(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Clob __ret = delegate.getClob( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getClob(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Clob getClob(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Clob __ret = delegate.getClob( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getClob(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean wasNull() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.wasNull();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","wasNull()", __start, __finish);
		return __ret;
	}

	public boolean absolute(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.absolute( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","absolute(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public void afterLast() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.afterLast();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","afterLast()", __start, __finish);
	}

	public void beforeFirst() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.beforeFirst();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","beforeFirst()", __start, __finish);
	}

	public void cancelRowUpdates() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.cancelRowUpdates();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","cancelRowUpdates()", __start, __finish);
	}

	public void deleteRow() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.deleteRow();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","deleteRow()", __start, __finish);
	}

	public java.io.InputStream getAsciiStream(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.io.InputStream __ret = delegate.getAsciiStream( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getAsciiStream(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.io.InputStream getAsciiStream(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.io.InputStream __ret = delegate.getAsciiStream( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getAsciiStream(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.io.InputStream getBinaryStream(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.io.InputStream __ret = delegate.getBinaryStream( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getBinaryStream(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.io.InputStream getBinaryStream(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.io.InputStream __ret = delegate.getBinaryStream( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getBinaryStream(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public int getConcurrency() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getConcurrency();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getConcurrency()", __start, __finish);
		return __ret;
	}

	public java.lang.String getCursorName() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getCursorName();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getCursorName()", __start, __finish);
		return __ret;
	}

	public java.sql.Statement getStatement() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Statement __ret = delegate.getStatement();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getStatement()", __start, __finish);
		return new StatementFacade(connectionID, statementID, __ret, callLogger);
	}

	public java.io.InputStream getUnicodeStream(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.io.InputStream __ret = delegate.getUnicodeStream( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getUnicodeStream(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.io.InputStream getUnicodeStream(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.io.InputStream __ret = delegate.getUnicodeStream( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getUnicodeStream(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean isAfterLast() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.isAfterLast();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","isAfterLast()", __start, __finish);
		return __ret;
	}

	public boolean isBeforeFirst() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.isBeforeFirst();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","isBeforeFirst()", __start, __finish);
		return __ret;
	}

	public boolean isFirst() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.isFirst();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","isFirst()", __start, __finish);
		return __ret;
	}

	public boolean isLast() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.isLast();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","isLast()", __start, __finish);
		return __ret;
	}

	public void moveToCurrentRow() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.moveToCurrentRow();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","moveToCurrentRow()", __start, __finish);
	}

	public void moveToInsertRow() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.moveToInsertRow();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","moveToInsertRow()", __start, __finish);
	}

	public void refreshRow() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.refreshRow();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","refreshRow()", __start, __finish);
	}

	public boolean relative(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.relative( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","relative(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean rowDeleted() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.rowDeleted();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","rowDeleted()", __start, __finish);
		return __ret;
	}

	public boolean rowInserted() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.rowInserted();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","rowInserted()", __start, __finish);
		return __ret;
	}

	public boolean rowUpdated() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.rowUpdated();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","rowUpdated()", __start, __finish);
		return __ret;
	}

	public void updateArray(int p0,java.sql.Array p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateArray( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateArray(int,java.sql.Array)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateArray(java.lang.String p0,java.sql.Array p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateArray( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateArray(java.lang.String,java.sql.Array)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateAsciiStream(java.lang.String p0,java.io.InputStream p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateAsciiStream( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateAsciiStream(java.lang.String,java.io.InputStream,int)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void updateAsciiStream(int p0,java.io.InputStream p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateAsciiStream( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateAsciiStream(int,java.io.InputStream,int)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void updateBigDecimal(java.lang.String p0,java.math.BigDecimal p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateBigDecimal( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateBigDecimal(java.lang.String,java.math.BigDecimal)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateBigDecimal(int p0,java.math.BigDecimal p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateBigDecimal( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateBigDecimal(int,java.math.BigDecimal)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateBinaryStream(java.lang.String p0,java.io.InputStream p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateBinaryStream( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateBinaryStream(java.lang.String,java.io.InputStream,int)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void updateBinaryStream(int p0,java.io.InputStream p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateBinaryStream( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateBinaryStream(int,java.io.InputStream,int)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void updateBlob(java.lang.String p0,java.sql.Blob p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateBlob( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateBlob(java.lang.String,java.sql.Blob)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateBlob(int p0,java.sql.Blob p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateBlob( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateBlob(int,java.sql.Blob)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateBoolean(int p0,boolean p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateBoolean( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateBoolean(int,boolean)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateBoolean(java.lang.String p0,boolean p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateBoolean( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateBoolean(java.lang.String,boolean)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateByte(int p0,byte p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateByte( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateByte(int,byte)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateByte(java.lang.String p0,byte p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateByte( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateByte(java.lang.String,byte)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateCharacterStream(int p0,java.io.Reader p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateCharacterStream( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateCharacterStream(int,java.io.Reader,int)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void updateCharacterStream(java.lang.String p0,java.io.Reader p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateCharacterStream( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateCharacterStream(java.lang.String,java.io.Reader,int)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void updateClob(int p0,java.sql.Clob p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateClob( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateClob(int,java.sql.Clob)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateClob(java.lang.String p0,java.sql.Clob p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateClob( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateClob(java.lang.String,java.sql.Clob)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateDate(int p0,java.sql.Date p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateDate( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateDate(int,java.sql.Date)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateDate(java.lang.String p0,java.sql.Date p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateDate( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateDate(java.lang.String,java.sql.Date)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateDouble(int p0,double p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateDouble( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateDouble(int,double)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateDouble(java.lang.String p0,double p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateDouble( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateDouble(java.lang.String,double)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateFloat(int p0,float p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateFloat( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateFloat(int,float)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateFloat(java.lang.String p0,float p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateFloat( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateFloat(java.lang.String,float)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateInt(int p0,int p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateInt( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateInt(int,int)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateInt(java.lang.String p0,int p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateInt( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateInt(java.lang.String,int)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateLong(int p0,long p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateLong( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateLong(int,long)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateLong(java.lang.String p0,long p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateLong( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateLong(java.lang.String,long)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateNull(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateNull( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateNull(int)",new Object[] {p0}, __start, __finish);
	}

	public void updateNull(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateNull( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateNull(java.lang.String)",new Object[] {p0}, __start, __finish);
	}

	public void updateObject(int p0,java.lang.Object p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateObject( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateObject(int,java.lang.Object)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateObject(int p0,java.lang.Object p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateObject( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateObject(int,java.lang.Object,int)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void updateObject(java.lang.String p0,java.lang.Object p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateObject( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateObject(java.lang.String,java.lang.Object,int)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void updateObject(java.lang.String p0,java.lang.Object p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateObject( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateObject(java.lang.String,java.lang.Object)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateRef(java.lang.String p0,java.sql.Ref p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateRef( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateRef(java.lang.String,java.sql.Ref)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateRef(int p0,java.sql.Ref p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateRef( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateRef(int,java.sql.Ref)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateRow() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateRow();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateRow()", __start, __finish);
	}

	public void updateShort(java.lang.String p0,short p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateShort( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateShort(java.lang.String,short)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateShort(int p0,short p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateShort( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateShort(int,short)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateString(int p0,java.lang.String p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateString( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateString(int,java.lang.String)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateString(java.lang.String p0,java.lang.String p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateString( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateString(java.lang.String,java.lang.String)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateTimestamp(int p0,java.sql.Timestamp p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateTimestamp( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateTimestamp(int,java.sql.Timestamp)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateTimestamp(java.lang.String p0,java.sql.Timestamp p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateTimestamp( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateTimestamp(java.lang.String,java.sql.Timestamp)",new Object[] {p0,p1}, __start, __finish);
	}

	public java.lang.Object getObject(java.lang.String p0,java.util.Map p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.Object __ret = delegate.getObject( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getObject(java.lang.String,java.util.Map)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public java.lang.Object getObject(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.Object __ret = delegate.getObject( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getObject(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.lang.Object getObject(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.Object __ret = delegate.getObject( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getObject(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.lang.Object getObject(int p0,java.util.Map p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.Object __ret = delegate.getObject( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getObject(int,java.util.Map)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public boolean getBoolean(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.getBoolean( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getBoolean(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean getBoolean(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.getBoolean( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getBoolean(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public byte getByte(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final byte __ret = delegate.getByte( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getByte(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public byte getByte(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final byte __ret = delegate.getByte( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getByte(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public short getShort(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final short __ret = delegate.getShort( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getShort(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public short getShort(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final short __ret = delegate.getShort( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getShort(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public int getInt(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getInt( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getInt(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public int getInt(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getInt( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getInt(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public long getLong(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final long __ret = delegate.getLong( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getLong(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public long getLong(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final long __ret = delegate.getLong( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getLong(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public float getFloat(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final float __ret = delegate.getFloat( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getFloat(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public float getFloat(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final float __ret = delegate.getFloat( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getFloat(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public double getDouble(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final double __ret = delegate.getDouble( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getDouble(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public double getDouble(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final double __ret = delegate.getDouble( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getDouble(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public byte[] getBytes(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final byte[] __ret = delegate.getBytes( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getBytes(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public byte[] getBytes(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final byte[] __ret = delegate.getBytes( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getBytes(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Array getArray(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Array __ret = delegate.getArray( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getArray(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Array getArray(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Array __ret = delegate.getArray( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getArray(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean next() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.next();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","next()", __start, __finish);
		return __ret;
	}

	public java.net.URL getURL(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.net.URL __ret = delegate.getURL( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getURL(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.net.URL getURL(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.net.URL __ret = delegate.getURL( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getURL(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public int getType() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getType();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getType()", __start, __finish);
		return __ret;
	}

	public boolean previous() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.previous();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","previous()", __start, __finish);
		return __ret;
	}

	public void close() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.close();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","close()", __start, __finish);
	}

	public java.sql.Ref getRef(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Ref __ret = delegate.getRef( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getRef(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Ref getRef(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Ref __ret = delegate.getRef( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getRef(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Date getDate(java.lang.String p0,java.util.Calendar p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Date __ret = delegate.getDate( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getDate(java.lang.String,java.util.Calendar)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public java.sql.Date getDate(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Date __ret = delegate.getDate( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getDate(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Date getDate(int p0,java.util.Calendar p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Date __ret = delegate.getDate( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getDate(int,java.util.Calendar)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public java.sql.Date getDate(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Date __ret = delegate.getDate( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getDate(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Time getTime(java.lang.String p0,java.util.Calendar p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Time __ret = delegate.getTime( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getTime(java.lang.String,java.util.Calendar)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public java.sql.Time getTime(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Time __ret = delegate.getTime( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getTime(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Time getTime(int p0,java.util.Calendar p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Time __ret = delegate.getTime( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getTime(int,java.util.Calendar)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public java.sql.Time getTime(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Time __ret = delegate.getTime( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getTime(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.lang.String getString(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getString( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getString(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.lang.String getString(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getString( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getString(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean first() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.first();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","first()", __start, __finish);
		return __ret;
	}

	public boolean last() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.last();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","last()", __start, __finish);
		return __ret;
	}

	public java.sql.Timestamp getTimestamp(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Timestamp __ret = delegate.getTimestamp( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getTimestamp(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Timestamp getTimestamp(int p0,java.util.Calendar p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Timestamp __ret = delegate.getTimestamp( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getTimestamp(int,java.util.Calendar)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public java.sql.Timestamp getTimestamp(java.lang.String p0,java.util.Calendar p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Timestamp __ret = delegate.getTimestamp( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getTimestamp(java.lang.String,java.util.Calendar)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public java.sql.Timestamp getTimestamp(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Timestamp __ret = delegate.getTimestamp( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getTimestamp(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public void updateTime(int p0,java.sql.Time p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateTime( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateTime(int,java.sql.Time)",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateTime(java.lang.String p0,java.sql.Time p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateTime( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateTime(java.lang.String,java.sql.Time)",new Object[] {p0,p1}, __start, __finish);
	}

	public java.math.BigDecimal getBigDecimal(int p0,int p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.math.BigDecimal __ret = delegate.getBigDecimal( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getBigDecimal(int,int)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public java.math.BigDecimal getBigDecimal(java.lang.String p0,int p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.math.BigDecimal __ret = delegate.getBigDecimal( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getBigDecimal(java.lang.String,int)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public java.math.BigDecimal getBigDecimal(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.math.BigDecimal __ret = delegate.getBigDecimal( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getBigDecimal(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.math.BigDecimal getBigDecimal(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.math.BigDecimal __ret = delegate.getBigDecimal( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getBigDecimal(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public void updateBytes(java.lang.String p0,byte[] p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateBytes( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateBytes(java.lang.String,byte[])",new Object[] {p0,p1}, __start, __finish);
	}

	public void updateBytes(int p0,byte[] p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.updateBytes( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","updateBytes(int,byte[])",new Object[] {p0,p1}, __start, __finish);
	}

	public java.io.Reader getCharacterStream(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.io.Reader __ret = delegate.getCharacterStream( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getCharacterStream(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.io.Reader getCharacterStream(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.io.Reader __ret = delegate.getCharacterStream( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getCharacterStream(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public int findColumn(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.findColumn( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","findColumn(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public void insertRow() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.insertRow();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","insertRow()", __start, __finish);
	}

	public int getRow() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getRow();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "ResultSet","getRow()", __start, __finish);
		return __ret;
	}

}
