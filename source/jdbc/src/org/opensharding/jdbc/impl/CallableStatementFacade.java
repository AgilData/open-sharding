package org.opensharding.jdbc.impl;

import java.io.InputStream;
import java.io.Reader;
import java.sql.*;

public class CallableStatementFacade extends PreparedStatementFacade implements CallableStatement {

    protected CallLogger callLogger;
	protected CallableStatement delegate;
    
    protected int connectionID;
    
    protected int statementID;

    public CallableStatementFacade(int connectionID, int statementID, CallableStatement delegate, CallLogger callLogger) {
        super(connectionID, statementID, delegate, callLogger);
        this.delegate = delegate;
        this.callLogger = callLogger;
    }

    public RowId getRowId(int parameterIndex) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public RowId getRowId(String parameterName) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setRowId(String parameterName, RowId x) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setNString(String parameterName, String value) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setNCharacterStream(String parameterName, Reader value, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setNClob(String parameterName, NClob value) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setClob(String parameterName, Reader reader, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setBlob(String parameterName, InputStream inputStream, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setNClob(String parameterName, Reader reader, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public NClob getNClob(int parameterIndex) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public NClob getNClob(String parameterName) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setSQLXML(String parameterName, SQLXML xmlObject) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public SQLXML getSQLXML(int parameterIndex) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public SQLXML getSQLXML(String parameterName) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public String getNString(int parameterIndex) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public String getNString(String parameterName) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public Reader getNCharacterStream(int parameterIndex) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public Reader getNCharacterStream(String parameterName) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public Reader getCharacterStream(int parameterIndex) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public Reader getCharacterStream(String parameterName) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setBlob(String parameterName, Blob x) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setClob(String parameterName, Clob x) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setAsciiStream(String parameterName, InputStream x, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setBinaryStream(String parameterName, InputStream x, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setCharacterStream(String parameterName, Reader reader, long length) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setAsciiStream(String parameterName, InputStream x) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setBinaryStream(String parameterName, InputStream x) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setCharacterStream(String parameterName, Reader reader) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setNCharacterStream(String parameterName, Reader value) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setClob(String parameterName, Reader reader) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setBlob(String parameterName, InputStream inputStream) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public void setNClob(String parameterName, Reader reader) throws SQLException {
        //To change body of implemented methods use File | Settings | File Templates.
    }

    public java.sql.Blob getBlob(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Blob __ret = delegate.getBlob( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getBlob(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Blob getBlob(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Blob __ret = delegate.getBlob( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getBlob(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Clob getClob(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Clob __ret = delegate.getClob( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getClob(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Clob getClob(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Clob __ret = delegate.getClob( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getClob(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public void registerOutParameter(java.lang.String p0,int p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.registerOutParameter( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","registerOutParameter(java.lang.String,int,int)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void registerOutParameter(java.lang.String p0,int p1,java.lang.String p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.registerOutParameter( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","registerOutParameter(java.lang.String,int,java.lang.String)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void registerOutParameter(int p0,int p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.registerOutParameter( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","registerOutParameter(int,int)",new Object[] {p0,p1}, __start, __finish);
	}

	public void registerOutParameter(int p0,int p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.registerOutParameter( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","registerOutParameter(int,int,int)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void registerOutParameter(int p0,int p1,java.lang.String p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.registerOutParameter( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","registerOutParameter(int,int,java.lang.String)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void registerOutParameter(java.lang.String p0,int p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.registerOutParameter( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","registerOutParameter(java.lang.String,int)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setAsciiStream(java.lang.String p0,java.io.InputStream p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setAsciiStream( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setAsciiStream(java.lang.String,java.io.InputStream,int)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void setBigDecimal(java.lang.String p0,java.math.BigDecimal p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setBigDecimal( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setBigDecimal(java.lang.String,java.math.BigDecimal)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setBinaryStream(java.lang.String p0,java.io.InputStream p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setBinaryStream( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setBinaryStream(java.lang.String,java.io.InputStream,int)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void setBytes(java.lang.String p0,byte[] p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setBytes( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setBytes(java.lang.String,byte[])",new Object[] {p0,p1}, __start, __finish);
	}

	public void setNull(java.lang.String p0,int p1,java.lang.String p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setNull( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setNull(java.lang.String,int,java.lang.String)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void setNull(java.lang.String p0,int p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setNull( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setNull(java.lang.String,int)",new Object[] {p0,p1}, __start, __finish);
	}

	public boolean wasNull() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.wasNull();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","wasNull()", __start, __finish);
		return __ret;
	}

	public java.lang.Object getObject(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.Object __ret = delegate.getObject( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getObject(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.lang.Object getObject(int p0,java.util.Map p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.Object __ret = delegate.getObject( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getObject(int,java.util.Map)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public java.lang.Object getObject(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.Object __ret = delegate.getObject( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getObject(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.lang.Object getObject(java.lang.String p0,java.util.Map p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.Object __ret = delegate.getObject( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getObject(java.lang.String,java.util.Map)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public boolean getBoolean(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.getBoolean( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getBoolean(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean getBoolean(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.getBoolean( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getBoolean(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public byte getByte(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final byte __ret = delegate.getByte( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getByte(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public byte getByte(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final byte __ret = delegate.getByte( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getByte(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public short getShort(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final short __ret = delegate.getShort( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getShort(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public short getShort(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final short __ret = delegate.getShort( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getShort(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public int getInt(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getInt( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getInt(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public int getInt(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getInt( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getInt(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public long getLong(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final long __ret = delegate.getLong( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getLong(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public long getLong(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final long __ret = delegate.getLong( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getLong(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public float getFloat(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final float __ret = delegate.getFloat( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getFloat(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public float getFloat(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final float __ret = delegate.getFloat( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getFloat(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public double getDouble(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final double __ret = delegate.getDouble( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getDouble(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public double getDouble(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final double __ret = delegate.getDouble( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getDouble(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public byte[] getBytes(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final byte[] __ret = delegate.getBytes( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getBytes(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public byte[] getBytes(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final byte[] __ret = delegate.getBytes( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getBytes(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Array getArray(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Array __ret = delegate.getArray( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getArray(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Array getArray(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Array __ret = delegate.getArray( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getArray(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.net.URL getURL(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.net.URL __ret = delegate.getURL( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getURL(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.net.URL getURL(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.net.URL __ret = delegate.getURL( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getURL(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public void setBoolean(java.lang.String p0,boolean p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setBoolean( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setBoolean(java.lang.String,boolean)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setByte(java.lang.String p0,byte p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setByte( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setByte(java.lang.String,byte)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setDouble(java.lang.String p0,double p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setDouble( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setDouble(java.lang.String,double)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setFloat(java.lang.String p0,float p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setFloat( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setFloat(java.lang.String,float)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setInt(java.lang.String p0,int p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setInt( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setInt(java.lang.String,int)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setLong(java.lang.String p0,long p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setLong( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setLong(java.lang.String,long)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setShort(java.lang.String p0,short p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setShort( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setShort(java.lang.String,short)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setTimestamp(java.lang.String p0,java.sql.Timestamp p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setTimestamp( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setTimestamp(java.lang.String,java.sql.Timestamp)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setTimestamp(java.lang.String p0,java.sql.Timestamp p1,java.util.Calendar p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setTimestamp( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setTimestamp(java.lang.String,java.sql.Timestamp,java.util.Calendar)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public java.sql.Ref getRef(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Ref __ret = delegate.getRef( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getRef(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Ref getRef(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Ref __ret = delegate.getRef( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getRef(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public void setURL(java.lang.String p0,java.net.URL p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setURL( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setURL(java.lang.String,java.net.URL)",new Object[] {p0,p1}, __start, __finish);
	}

	public java.sql.Date getDate(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Date __ret = delegate.getDate( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getDate(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Date getDate(java.lang.String p0,java.util.Calendar p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Date __ret = delegate.getDate( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getDate(java.lang.String,java.util.Calendar)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public java.sql.Date getDate(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Date __ret = delegate.getDate( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getDate(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Date getDate(int p0,java.util.Calendar p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Date __ret = delegate.getDate( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getDate(int,java.util.Calendar)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public java.sql.Time getTime(int p0,java.util.Calendar p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Time __ret = delegate.getTime( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getTime(int,java.util.Calendar)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public java.sql.Time getTime(java.lang.String p0,java.util.Calendar p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Time __ret = delegate.getTime( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getTime(java.lang.String,java.util.Calendar)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public java.sql.Time getTime(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Time __ret = delegate.getTime( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getTime(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Time getTime(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Time __ret = delegate.getTime( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getTime(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public void setTime(java.lang.String p0,java.sql.Time p1,java.util.Calendar p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setTime( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setTime(java.lang.String,java.sql.Time,java.util.Calendar)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void setTime(java.lang.String p0,java.sql.Time p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setTime( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setTime(java.lang.String,java.sql.Time)",new Object[] {p0,p1}, __start, __finish);
	}

	public java.lang.String getString(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getString( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getString(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.lang.String getString(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getString( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getString(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Timestamp getTimestamp(java.lang.String p0,java.util.Calendar p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Timestamp __ret = delegate.getTimestamp( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getTimestamp(java.lang.String,java.util.Calendar)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public java.sql.Timestamp getTimestamp(int p0,java.util.Calendar p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Timestamp __ret = delegate.getTimestamp( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getTimestamp(int,java.util.Calendar)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public java.sql.Timestamp getTimestamp(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Timestamp __ret = delegate.getTimestamp( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getTimestamp(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.Timestamp getTimestamp(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Timestamp __ret = delegate.getTimestamp( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getTimestamp(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public void setDate(java.lang.String p0,java.sql.Date p1,java.util.Calendar p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setDate( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setDate(java.lang.String,java.sql.Date,java.util.Calendar)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void setDate(java.lang.String p0,java.sql.Date p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setDate( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setDate(java.lang.String,java.sql.Date)",new Object[] {p0,p1}, __start, __finish);
	}

	public java.math.BigDecimal getBigDecimal(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.math.BigDecimal __ret = delegate.getBigDecimal( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getBigDecimal(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.math.BigDecimal getBigDecimal(int p0,int p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.math.BigDecimal __ret = delegate.getBigDecimal( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getBigDecimal(int,int)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public java.math.BigDecimal getBigDecimal(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.math.BigDecimal __ret = delegate.getBigDecimal( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","getBigDecimal(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public void setObject(java.lang.String p0,java.lang.Object p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setObject( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setObject(java.lang.String,java.lang.Object)",new Object[] {p0,p1}, __start, __finish);
	}

	public void setObject(java.lang.String p0,java.lang.Object p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setObject( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setObject(java.lang.String,java.lang.Object,int)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void setObject(java.lang.String p0,java.lang.Object p1,int p2,int p3) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setObject( p0, p1, p2, p3);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setObject(java.lang.String,java.lang.Object,int,int)",new Object[] {p0,p1,p2,p3}, __start, __finish);
	}

	public void setCharacterStream(java.lang.String p0,java.io.Reader p1,int p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setCharacterStream( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setCharacterStream(java.lang.String,java.io.Reader,int)",new Object[] {p0,p1,p2}, __start, __finish);
	}

	public void setString(java.lang.String p0,java.lang.String p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		delegate.setString( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, statementID, "CallableStatement","setString(java.lang.String,java.lang.String)",new Object[] {p0,p1}, __start, __finish);
	}

}
