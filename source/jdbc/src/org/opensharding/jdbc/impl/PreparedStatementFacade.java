package org.opensharding.jdbc.impl;

import java.awt.print.Paper;
import java.io.InputStream;
import java.io.Reader;
import java.sql.*;

import org.opensharding.common.util.ByteUtil;

public class PreparedStatementFacade extends StatementFacade implements PreparedStatement {
    
    protected CallLogger callLogger;

	protected PreparedStatement delegate;
		    
    public PreparedStatementFacade(int connectionID, int statementID, PreparedStatement delegate, CallLogger callLogger) {
        super(connectionID, statementID, delegate, callLogger);
        this.delegate = delegate;
        this.callLogger = callLogger;
    }

    public void setRowId(int parameterIndex, RowId x) throws SQLException {
    	final long __start = System.currentTimeMillis();
    	delegate.setRowId(parameterIndex, x);
    	final long __finish = System.currentTimeMillis();
    	callLogger.log(connectionID, statementID, "PreparedStatement", "setRowId(int, java.sql.RowId)", new Object[] {parameterIndex,x}, __start, __finish);
    	
    }

    public void setNString(int parameterIndex, String value) throws SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setNString(parameterIndex, value);
		final long __finish = System.currentTimeMillis();
        callLogger.log(connectionID, statementID, "PreparedStatement","setNString(int, java.lang.String)",new Object[] {parameterIndex,value}, __start, __finish);
    }
    // TODO: Enable all set commands, in the generic driver and properties file too.
    public void setNCharacterStream(int parameterIndex, Reader value, long length) throws SQLException {
    	final long __start = System.currentTimeMillis();
    	delegate.setNCharacterStream(parameterIndex, value, length);
    	final long __finish = System.currentTimeMillis();
    	callLogger.log(connectionID, statementID, "PreparedStatement", "setNCharacterStream(int, java.io.Reader, long)", new Object[] {parameterIndex,value,length}, __start, __finish);	
    }

    public void setNClob(int parameterIndex, NClob value) throws SQLException {
    	final long __start = System.currentTimeMillis();
    	delegate.setNClob(parameterIndex, value);
    	final long __finish = System.currentTimeMillis();
    	callLogger.log(connectionID, statementID, "PreparedStatement", "setNClob(int, java.sql.NClob)", new Object[] {parameterIndex,value}, __start, __finish);
    }

    public void setClob(int parameterIndex, Reader reader, long length) throws SQLException {
    	final long __start = System.currentTimeMillis();
    	delegate.setClob(parameterIndex, reader, length);
    	final long __finish = System.currentTimeMillis();
    	callLogger.log(connectionID, statementID, "PreparedStatement", "setClob(int, java.io.Reader, long)", new Object[] {parameterIndex,reader,length}, __start, __finish);
    }

    public void setBlob(int parameterIndex, InputStream inputStream, long length) throws SQLException {
    	final long __start = System.currentTimeMillis();
    	delegate.setBlob(parameterIndex, inputStream, length);
    	final long __finish = System.currentTimeMillis();
    	callLogger.log(connectionID, statementID, "PreparedStatement", "setBlob(int, java.io.InputStream, long)", new Object[] {parameterIndex,inputStream,length}, __start, __finish);
    }

    public void setNClob(int parameterIndex, Reader reader, long length) throws SQLException {
    	final long __start = System.currentTimeMillis();
    	delegate.setNClob(parameterIndex, reader, length);
    	final long __finish = System.currentTimeMillis();
    	callLogger.log(connectionID, statementID, "PreparedStatement", "setNClob(int, java.io.Reader, long)", new Object[] {parameterIndex,reader,length}, __start, __finish);
    }
 
    public void setSQLXML(int parameterIndex, SQLXML xmlObject) throws SQLException {
    	final long __start = System.currentTimeMillis();
    	delegate.setSQLXML(parameterIndex, xmlObject);
    	final long __finish = System.currentTimeMillis();
    	callLogger.log(connectionID, statementID, "PreparedStatement", "setSQLXML(int, java.sql.SQLXML)", new Object[] {parameterIndex,xmlObject}, __start, __finish);
    }

    public void setAsciiStream(int parameterIndex, InputStream x, long length) throws SQLException {
    	final long __start = System.currentTimeMillis();
    	delegate.setAsciiStream(parameterIndex, x, length);
    	final long __finish = System.currentTimeMillis();
    	callLogger.log(connectionID, statementID, "PreparedStatement", "setAsciiStream(int, java.io.InputStream, long)", new Object[] {parameterIndex,x,length}, __start, __finish);
    }

    public void setBinaryStream(int parameterIndex, InputStream x, long length) throws SQLException {
    	final long __start = System.currentTimeMillis();
    	delegate.setBinaryStream(parameterIndex, x, length);
    	final long __finish = System.currentTimeMillis();
    	callLogger.log(connectionID, statementID, "PreparedStatement", "setBinaryStream(int, java.io.InputStream, long)", new Object[] {parameterIndex,x,length}, __start, __finish);
    }

    public void setCharacterStream(int parameterIndex, Reader reader, long length) throws SQLException {
    	final long __start = System.currentTimeMillis();
    	delegate.setCharacterStream(parameterIndex, reader, length);
    	final long __finish = System.currentTimeMillis();
    	callLogger.log(connectionID, statementID, "PreparedStatement", "setCharacterStream(int, java.io.Reader, long)", new Object[] {parameterIndex,reader,length}, __start, __finish);
    }

    public void setAsciiStream(int parameterIndex, InputStream x) throws SQLException {
    	final long __start = System.currentTimeMillis();
    	delegate.setAsciiStream(parameterIndex, x);
    	final long __finish = System.currentTimeMillis();
    	callLogger.log(connectionID, statementID, "PreparedStatement", "setAsciiStream(int, java.io.InputStream)", new Object[] {parameterIndex,x}, __start, __finish);
    }

    public void setBinaryStream(int parameterIndex, InputStream x) throws SQLException {
    	final long __start = System.currentTimeMillis();
    	delegate.setBinaryStream(parameterIndex, x);
    	final long __finish = System.currentTimeMillis();
    	callLogger.log(connectionID, statementID, "PreparedStatement", "setBinaryStream(int, java.io.InputStream)", new Object[] {parameterIndex,x}, __start, __finish);
    }

    public void setCharacterStream(int parameterIndex, Reader reader) throws SQLException {
    	final long __start = System.currentTimeMillis();
    	delegate.setCharacterStream(parameterIndex, reader);
    	final long __finish = System.currentTimeMillis();
    	callLogger.log(connectionID, statementID, "PreparedStatement", "setCharacterStream(int, java.io.Reader)", new Object[] {parameterIndex,reader}, __start, __finish);
    }

    public void setNCharacterStream(int parameterIndex, Reader value) throws SQLException {
    	final long __start = System.currentTimeMillis();
    	delegate.setNCharacterStream(parameterIndex, value);
    	final long __finish = System.currentTimeMillis();
    	callLogger.log(connectionID, statementID, "PreparedStatement", "setNCharacterStream(int, java.io.Reader)", new Object[] {parameterIndex,value}, __start, __finish);
    }

    public void setClob(int parameterIndex, Reader reader) throws SQLException {
    	final long __start = System.currentTimeMillis();
    	delegate.setClob(parameterIndex, reader);
    	final long __finish = System.currentTimeMillis();
    	callLogger.log(connectionID, statementID, "PreparedStatement", "setClob(int, java.io.Reader)", new Object[] {parameterIndex,reader}, __start, __finish);
    }

    public void setBlob(int parameterIndex, InputStream inputStream) throws SQLException {
    	final long __start = System.currentTimeMillis();
    	delegate.setBlob(parameterIndex, inputStream);
    	final long __finish = System.currentTimeMillis();
    	callLogger.log(connectionID, statementID, "PreparedStatement", "setBlob(int, java.io.InputStream)", new Object[] {parameterIndex,inputStream}, __start, __finish);
    }

    public void setNClob(int parameterIndex, Reader reader) throws SQLException {
    	final long __start = System.currentTimeMillis();
    	delegate.setNClob(parameterIndex, reader);
    	final long __finish = System.currentTimeMillis();
    	callLogger.log(connectionID, statementID, "PreparedStatement", "setNClob(int, java.io.Reader)", new Object[] {parameterIndex,reader}, __start, __finish);
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

    public java.sql.ResultSetMetaData getMetaData() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSetMetaData __ret = delegate.getMetaData();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","getMetaData()", __start, __finish);
		return new ResultSetMetaDataFacade(connectionID, statementID, __ret, callLogger);
	}

	public void addBatch() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.addBatch();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","addBatch()", __start, __finish);
	}

	public java.sql.ResultSet executeQuery() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.executeQuery();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","executeQuery()", __start, __finish);
		return __ret==null ? null : new ResultSetFacade(connectionID, statementID, __ret, callLogger);
	}

	public int executeUpdate() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.executeUpdate();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","executeUpdate()", __start, __finish);
		return __ret;
	}

	public void setAsciiStream(int p0,java.io.InputStream p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setAsciiStream( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setAsciiStream(int,java.io.InputStream,int)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void setBigDecimal(int p0,java.math.BigDecimal p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setBigDecimal( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setBigDecimal(int,java.math.BigDecimal)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setBinaryStream(int p0,java.io.InputStream p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setBinaryStream( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setBinaryStream(int,java.io.InputStream,int)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void setBytes(int p0,byte[] p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setBytes( p0, p1);
		final long __finish = System.currentTimeMillis();
		// Convert to hex.
		String data = ByteUtil.toHexString(p1);
		callLogger.log(connectionID, statementID, "PreparedStatement","setBytes(int,byte[])",new Object[] {p0,data}, __start, __finish);
	}

	public void setNull(int p0,int p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setNull( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setNull(int,int)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setNull(int p0,int p1,java.lang.String p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setNull( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setNull(int,int,java.lang.String)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void clearParameters() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.clearParameters();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","clearParameters()", __start, __finish);
	}

	public java.sql.ParameterMetaData getParameterMetaData() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ParameterMetaData __ret = delegate.getParameterMetaData();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","getParameterMetaData()", __start, __finish);
		return __ret;
	}

	public void setArray(int p0,java.sql.Array p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setArray( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setArray(int,java.sql.Array)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setBlob(int p0,java.sql.Blob p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setBlob( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setBlob(int,java.sql.Blob)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setClob(int p0,java.sql.Clob p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setClob( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setClob(int,java.sql.Clob)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setRef(int p0,java.sql.Ref p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setRef( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setRef(int,java.sql.Ref)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setUnicodeStream(int p0,java.io.InputStream p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setUnicodeStream( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setUnicodeStream(int,java.io.InputStream,int)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void setBoolean(int p0,boolean p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setBoolean( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setBoolean(int,java.lang.Boolean)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setByte(int p0,byte p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setByte( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setByte(int,java.lang.Byte)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setDouble(int p0,double p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setDouble( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setDouble(int,java.lang.Double)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setFloat(int p0,float p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setFloat( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setFloat(int,java.lang.Float)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setInt(int p0,int p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setInt( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setInt(int,java.lang.Integer)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setLong(int p0,long p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setLong( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setLong(int,java.lang.Long)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setShort(int p0,short p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setShort( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setShort(int,java.lang.Short)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setTimestamp(int p0,java.sql.Timestamp p1,java.util.Calendar p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setTimestamp( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setTimestamp(int,java.sql.Timestamp,java.util.Calendar)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void setTimestamp(int p0,java.sql.Timestamp p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setTimestamp( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setTimestamp(int,java.sql.Timestamp)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setURL(int p0,java.net.URL p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setURL( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setURL(int,java.net.URL)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setTime(int p0,java.sql.Time p1,java.util.Calendar p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setTime( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setTime(int,java.sql.Time,java.util.Calendar)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void setTime(int p0,java.sql.Time p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setTime( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setTime(int,java.sql.Time)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setDate(int p0,java.sql.Date p1,java.util.Calendar p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setDate( p0, p1, p2);
		final long __finish = System.currentTimeMillis();		
		callLogger.log(connectionID, statementID, "PreparedStatement","setDate(int,java.sql.Date,java.util.Calendar)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void setDate(int p0,java.sql.Date p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setDate( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setDate(int,java.sql.Date)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setObject(int p0,java.lang.Object p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setObject( p0, p1);
		final long __finish = System.currentTimeMillis();
		String dataType = getClassName(p1);
		// If p1 is a byte[] convert to hex.
		if(dataType.equals("byte[]")) {
			p1 = ByteUtil.toHexString((byte[])p1);
		}
		callLogger.log(connectionID, statementID, "PreparedStatement","setObject(int,"+dataType+")",new Object[] {p0,p1}, __start, __finish);
	}

	public void setObject(int p0,java.lang.Object p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setObject( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		String dataType = getClassName(p1);
		// If p1 is a byte[] convert to hex.
		if(dataType.equals("byte[]")) {
			p1 = ByteUtil.toHexString((byte[])p1);
		}
		callLogger.log(connectionID, statementID, "PreparedStatement","setObject(int,"+dataType+",int)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void setObject(int p0,java.lang.Object p1,int p2,int p3) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setObject( p0, p1, p2, p3);
		final long __finish = System.currentTimeMillis();
		String dataType = getClassName(p1);
		// If p1 is a byte[] convert to hex.
		if(dataType.equals("byte[]")) {
			p1 = ByteUtil.toHexString((byte[])p1);
		}
		callLogger.log(connectionID, statementID, "PreparedStatement","setObject(int,"+dataType+",int,int)",new Object[] {p0,p1,p2,p3}, __start, __finish);
	}

	public void setCharacterStream(int p0,java.io.Reader p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setCharacterStream( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setCharacterStream(int,java.io.Reader,int)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void setString(int p0,java.lang.String p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setString( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","setString(int,java.lang.String)",new Object[] {p0,p1}, __start, __finish);
	}

	public boolean execute() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.execute();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "PreparedStatement","execute()", __start, __finish);
		return __ret;
	}

	private String getClassName(Object value) {
		String dataType = null;
		// Set the actual Java type. A byte[] does not return a proper class name.
		if(value.getClass().isAssignableFrom(byte[].class)) {
			dataType = "byte[]";
		} else {
			dataType = value.getClass().getName();
		}

		return dataType;
	}
}
