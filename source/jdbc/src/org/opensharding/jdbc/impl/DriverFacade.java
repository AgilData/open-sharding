package org.opensharding.jdbc.impl;

import java.sql.Driver;

public abstract class DriverFacade implements Driver {

    protected CallLogger callLogger;
	protected Driver delegate;



	public boolean acceptsURL(java.lang.String p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.acceptsURL( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(0, 0, "Driver","acceptsURL(java.lang.String)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public java.sql.DriverPropertyInfo[] getPropertyInfo(java.lang.String p0,java.util.Properties p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.DriverPropertyInfo[] __ret = delegate.getPropertyInfo( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(0, 0, "Driver","getPropertyInfo(java.lang.String,java.util.Properties)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public boolean jdbcCompliant() {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.jdbcCompliant();
		final long __finish = System.currentTimeMillis();
		callLogger.log(0, 0, "Driver","jdbcCompliant()", __start, __finish);
		return __ret;
	}

	abstract public java.sql.Connection connect(java.lang.String p0,java.util.Properties p1) throws java.sql.SQLException;

	public int getMajorVersion() {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMajorVersion();
		final long __finish = System.currentTimeMillis();
		callLogger.log(0, 0, "Driver","getMajorVersion()", __start, __finish);
		return __ret;
	}

	public int getMinorVersion() {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMinorVersion();
		final long __finish = System.currentTimeMillis();
		callLogger.log(0, 0, "Driver","getMinorVersion()", __start, __finish);
		return __ret;
	}

}
