package org.opensharding.jdbc.impl;

import java.sql.DatabaseMetaData;
import java.sql.ResultSet;
import java.sql.RowIdLifetime;
import java.sql.SQLException;

public class DatabaseMetaDataFacade implements DatabaseMetaData {

    protected CallLogger callLogger;
    
	protected DatabaseMetaData delegate;
    
    protected int connectionID;

    public DatabaseMetaDataFacade(int connectionID, DatabaseMetaData delegate, CallLogger callLogger) {
        this.connectionID = connectionID;
        this.delegate = delegate;
        this.callLogger = callLogger;
    }

    public RowIdLifetime getRowIdLifetime() throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public ResultSet getSchemas(String catalog, String schemaPattern) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public boolean supportsStoredFunctionsUsingCallSyntax() throws SQLException {
        return false;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public boolean autoCommitFailureClosesAllResultSets() throws SQLException {
        return false;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public ResultSet getClientInfoProperties() throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public ResultSet getFunctions(String catalog, String schemaPattern, String functionNamePattern) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public ResultSet getFunctionColumns(String catalog, String schemaPattern, String functionNamePattern, String columnNamePattern) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public <T> T unwrap(Class<T> iface) throws SQLException {
        return null;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public boolean isWrapperFor(Class<?> iface) throws SQLException {
        return false;  //To change body of implemented methods use File | Settings | File Templates.
    }

    public int getResultSetHoldability() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getResultSetHoldability();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getResultSetHoldability()", __start, __finish);
		return __ret;
	}

	public boolean allProceduresAreCallable() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.allProceduresAreCallable();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","allProceduresAreCallable()", __start, __finish);
		return __ret;
	}

	public boolean allTablesAreSelectable() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.allTablesAreSelectable();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","allTablesAreSelectable()", __start, __finish);
		return __ret;
	}

	public boolean dataDefinitionCausesTransactionCommit() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.dataDefinitionCausesTransactionCommit();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","dataDefinitionCausesTransactionCommit()", __start, __finish);
		return __ret;
	}

	public boolean dataDefinitionIgnoredInTransactions() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.dataDefinitionIgnoredInTransactions();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","dataDefinitionIgnoredInTransactions()", __start, __finish);
		return __ret;
	}

	public boolean deletesAreDetected(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.deletesAreDetected( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","deletesAreDetected(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean doesMaxRowSizeIncludeBlobs() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.doesMaxRowSizeIncludeBlobs();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","doesMaxRowSizeIncludeBlobs()", __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getBestRowIdentifier(java.lang.String p0,java.lang.String p1,java.lang.String p2,int p3,boolean p4) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getBestRowIdentifier( p0, p1, p2, p3, p4);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getBestRowIdentifier(java.lang.String,java.lang.String,java.lang.String,int,boolean)",new Object[] {p0,p1,p2,p3,p4}, __start, __finish);
		return __ret;
	}

	public java.lang.String getCatalogSeparator() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getCatalogSeparator();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getCatalogSeparator()", __start, __finish);
		return __ret;
	}

	public java.lang.String getCatalogTerm() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getCatalogTerm();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getCatalogTerm()", __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getCatalogs() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getCatalogs();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getCatalogs()", __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getColumnPrivileges(java.lang.String p0,java.lang.String p1,java.lang.String p2,java.lang.String p3) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getColumnPrivileges( p0, p1, p2, p3);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getColumnPrivileges(java.lang.String,java.lang.String,java.lang.String,java.lang.String)",new Object[] {p0,p1,p2,p3}, __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getCrossReference(java.lang.String p0,java.lang.String p1,java.lang.String p2,java.lang.String p3,java.lang.String p4,java.lang.String p5) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getCrossReference( p0, p1, p2, p3, p4, p5);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getCrossReference(java.lang.String,java.lang.String,java.lang.String,java.lang.String,java.lang.String,java.lang.String)",new Object[] {p0,p1,p2,p3,p4,p5}, __start, __finish);
		return __ret;
	}

	public int getDatabaseMajorVersion() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getDatabaseMajorVersion();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getDatabaseMajorVersion()", __start, __finish);
		return __ret;
	}

	public int getDatabaseMinorVersion() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getDatabaseMinorVersion();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getDatabaseMinorVersion()", __start, __finish);
		return __ret;
	}

	public java.lang.String getDatabaseProductName() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getDatabaseProductName();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getDatabaseProductName()", __start, __finish);
		return __ret;
	}

	public java.lang.String getDatabaseProductVersion() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getDatabaseProductVersion();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getDatabaseProductVersion()", __start, __finish);
		return __ret;
	}

	public int getDefaultTransactionIsolation() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getDefaultTransactionIsolation();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getDefaultTransactionIsolation()", __start, __finish);
		return __ret;
	}

	public int getDriverMajorVersion() {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getDriverMajorVersion();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getDriverMajorVersion()", __start, __finish);
		return __ret;
	}

	public int getDriverMinorVersion() {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getDriverMinorVersion();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getDriverMinorVersion()", __start, __finish);
		return __ret;
	}

	public java.lang.String getDriverName() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getDriverName();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getDriverName()", __start, __finish);
		return __ret;
	}

	public java.lang.String getDriverVersion() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getDriverVersion();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getDriverVersion()", __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getExportedKeys(java.lang.String p0,java.lang.String p1,java.lang.String p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getExportedKeys( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getExportedKeys(java.lang.String,java.lang.String,java.lang.String)",new Object[] {p0,p1,p2}, __start, __finish);
		return __ret;
	}

	public java.lang.String getExtraNameCharacters() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getExtraNameCharacters();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getExtraNameCharacters()", __start, __finish);
		return __ret;
	}

	public java.lang.String getIdentifierQuoteString() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getIdentifierQuoteString();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getIdentifierQuoteString()", __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getImportedKeys(java.lang.String p0,java.lang.String p1,java.lang.String p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getImportedKeys( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getImportedKeys(java.lang.String,java.lang.String,java.lang.String)",new Object[] {p0,p1,p2}, __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getIndexInfo(java.lang.String p0,java.lang.String p1,java.lang.String p2,boolean p3,boolean p4) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getIndexInfo( p0, p1, p2, p3, p4);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getIndexInfo(java.lang.String,java.lang.String,java.lang.String,boolean,boolean)",new Object[] {p0,p1,p2,p3,p4}, __start, __finish);
		return __ret;
	}

	public int getJDBCMajorVersion() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getJDBCMajorVersion();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getJDBCMajorVersion()", __start, __finish);
		return __ret;
	}

	public int getJDBCMinorVersion() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getJDBCMinorVersion();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getJDBCMinorVersion()", __start, __finish);
		return __ret;
	}

	public int getMaxBinaryLiteralLength() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxBinaryLiteralLength();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getMaxBinaryLiteralLength()", __start, __finish);
		return __ret;
	}

	public int getMaxCatalogNameLength() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxCatalogNameLength();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getMaxCatalogNameLength()", __start, __finish);
		return __ret;
	}

	public int getMaxCharLiteralLength() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxCharLiteralLength();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getMaxCharLiteralLength()", __start, __finish);
		return __ret;
	}

	public int getMaxColumnNameLength() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxColumnNameLength();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getMaxColumnNameLength()", __start, __finish);
		return __ret;
	}

	public int getMaxColumnsInGroupBy() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxColumnsInGroupBy();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getMaxColumnsInGroupBy()", __start, __finish);
		return __ret;
	}

	public int getMaxColumnsInIndex() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxColumnsInIndex();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getMaxColumnsInIndex()", __start, __finish);
		return __ret;
	}

	public int getMaxColumnsInOrderBy() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxColumnsInOrderBy();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getMaxColumnsInOrderBy()", __start, __finish);
		return __ret;
	}

	public int getMaxColumnsInSelect() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxColumnsInSelect();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getMaxColumnsInSelect()", __start, __finish);
		return __ret;
	}

	public int getMaxColumnsInTable() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxColumnsInTable();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getMaxColumnsInTable()", __start, __finish);
		return __ret;
	}

	public int getMaxConnections() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxConnections();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getMaxConnections()", __start, __finish);
		return __ret;
	}

	public int getMaxCursorNameLength() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxCursorNameLength();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getMaxCursorNameLength()", __start, __finish);
		return __ret;
	}

	public int getMaxIndexLength() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxIndexLength();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getMaxIndexLength()", __start, __finish);
		return __ret;
	}

	public int getMaxProcedureNameLength() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxProcedureNameLength();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getMaxProcedureNameLength()", __start, __finish);
		return __ret;
	}

	public int getMaxRowSize() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxRowSize();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getMaxRowSize()", __start, __finish);
		return __ret;
	}

	public int getMaxSchemaNameLength() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxSchemaNameLength();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getMaxSchemaNameLength()", __start, __finish);
		return __ret;
	}

	public int getMaxStatementLength() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxStatementLength();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getMaxStatementLength()", __start, __finish);
		return __ret;
	}

	public int getMaxStatements() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxStatements();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getMaxStatements()", __start, __finish);
		return __ret;
	}

	public int getMaxTableNameLength() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxTableNameLength();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getMaxTableNameLength()", __start, __finish);
		return __ret;
	}

	public int getMaxTablesInSelect() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxTablesInSelect();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getMaxTablesInSelect()", __start, __finish);
		return __ret;
	}

	public int getMaxUserNameLength() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getMaxUserNameLength();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getMaxUserNameLength()", __start, __finish);
		return __ret;
	}

	public java.lang.String getNumericFunctions() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getNumericFunctions();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getNumericFunctions()", __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getPrimaryKeys(java.lang.String p0,java.lang.String p1,java.lang.String p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getPrimaryKeys( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getPrimaryKeys(java.lang.String,java.lang.String,java.lang.String)",new Object[] {p0,p1,p2}, __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getProcedureColumns(java.lang.String p0,java.lang.String p1,java.lang.String p2,java.lang.String p3) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getProcedureColumns( p0, p1, p2, p3);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getProcedureColumns(java.lang.String,java.lang.String,java.lang.String,java.lang.String)",new Object[] {p0,p1,p2,p3}, __start, __finish);
		return __ret;
	}

	public java.lang.String getProcedureTerm() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getProcedureTerm();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getProcedureTerm()", __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getProcedures(java.lang.String p0,java.lang.String p1,java.lang.String p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getProcedures( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getProcedures(java.lang.String,java.lang.String,java.lang.String)",new Object[] {p0,p1,p2}, __start, __finish);
		return __ret;
	}

	public java.lang.String getSQLKeywords() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getSQLKeywords();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getSQLKeywords()", __start, __finish);
		return __ret;
	}

	public int getSQLStateType() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final int __ret = delegate.getSQLStateType();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getSQLStateType()", __start, __finish);
		return __ret;
	}

	public java.lang.String getSchemaTerm() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getSchemaTerm();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getSchemaTerm()", __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getSchemas() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getSchemas();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getSchemas()", __start, __finish);
		return __ret;
	}

	public java.lang.String getSearchStringEscape() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getSearchStringEscape();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getSearchStringEscape()", __start, __finish);
		return __ret;
	}

	public java.lang.String getStringFunctions() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getStringFunctions();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getStringFunctions()", __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getSuperTables(java.lang.String p0,java.lang.String p1,java.lang.String p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getSuperTables( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getSuperTables(java.lang.String,java.lang.String,java.lang.String)",new Object[] {p0,p1,p2}, __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getSuperTypes(java.lang.String p0,java.lang.String p1,java.lang.String p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getSuperTypes( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getSuperTypes(java.lang.String,java.lang.String,java.lang.String)",new Object[] {p0,p1,p2}, __start, __finish);
		return __ret;
	}

	public java.lang.String getSystemFunctions() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getSystemFunctions();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getSystemFunctions()", __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getTablePrivileges(java.lang.String p0,java.lang.String p1,java.lang.String p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getTablePrivileges( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getTablePrivileges(java.lang.String,java.lang.String,java.lang.String)",new Object[] {p0,p1,p2}, __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getTableTypes() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getTableTypes();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getTableTypes()", __start, __finish);
		return __ret;
	}

	public java.lang.String getTimeDateFunctions() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getTimeDateFunctions();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getTimeDateFunctions()", __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getTypeInfo() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getTypeInfo();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getTypeInfo()", __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getUDTs(java.lang.String p0,java.lang.String p1,java.lang.String p2,int[] p3) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getUDTs( p0, p1, p2, p3);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getUDTs(java.lang.String,java.lang.String,java.lang.String,int[])",new Object[] {p0,p1,p2,p3}, __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getVersionColumns(java.lang.String p0,java.lang.String p1,java.lang.String p2) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getVersionColumns( p0, p1, p2);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getVersionColumns(java.lang.String,java.lang.String,java.lang.String)",new Object[] {p0,p1,p2}, __start, __finish);
		return __ret;
	}

	public boolean insertsAreDetected(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.insertsAreDetected( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","insertsAreDetected(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean isCatalogAtStart() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.isCatalogAtStart();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","isCatalogAtStart()", __start, __finish);
		return __ret;
	}

	public boolean locatorsUpdateCopy() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.locatorsUpdateCopy();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","locatorsUpdateCopy()", __start, __finish);
		return __ret;
	}

	public boolean nullPlusNonNullIsNull() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.nullPlusNonNullIsNull();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","nullPlusNonNullIsNull()", __start, __finish);
		return __ret;
	}

	public boolean nullsAreSortedAtEnd() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.nullsAreSortedAtEnd();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","nullsAreSortedAtEnd()", __start, __finish);
		return __ret;
	}

	public boolean nullsAreSortedAtStart() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.nullsAreSortedAtStart();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","nullsAreSortedAtStart()", __start, __finish);
		return __ret;
	}

	public boolean nullsAreSortedHigh() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.nullsAreSortedHigh();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","nullsAreSortedHigh()", __start, __finish);
		return __ret;
	}

	public boolean nullsAreSortedLow() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.nullsAreSortedLow();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","nullsAreSortedLow()", __start, __finish);
		return __ret;
	}

	public boolean othersDeletesAreVisible(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.othersDeletesAreVisible( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","othersDeletesAreVisible(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean othersInsertsAreVisible(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.othersInsertsAreVisible( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","othersInsertsAreVisible(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean othersUpdatesAreVisible(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.othersUpdatesAreVisible( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","othersUpdatesAreVisible(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean ownDeletesAreVisible(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.ownDeletesAreVisible( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","ownDeletesAreVisible(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean ownInsertsAreVisible(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.ownInsertsAreVisible( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","ownInsertsAreVisible(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean ownUpdatesAreVisible(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.ownUpdatesAreVisible( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","ownUpdatesAreVisible(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean storesLowerCaseIdentifiers() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.storesLowerCaseIdentifiers();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","storesLowerCaseIdentifiers()", __start, __finish);
		return __ret;
	}

	public boolean storesLowerCaseQuotedIdentifiers() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.storesLowerCaseQuotedIdentifiers();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","storesLowerCaseQuotedIdentifiers()", __start, __finish);
		return __ret;
	}

	public boolean storesMixedCaseIdentifiers() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.storesMixedCaseIdentifiers();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","storesMixedCaseIdentifiers()", __start, __finish);
		return __ret;
	}

	public boolean storesMixedCaseQuotedIdentifiers() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.storesMixedCaseQuotedIdentifiers();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","storesMixedCaseQuotedIdentifiers()", __start, __finish);
		return __ret;
	}

	public boolean storesUpperCaseIdentifiers() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.storesUpperCaseIdentifiers();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","storesUpperCaseIdentifiers()", __start, __finish);
		return __ret;
	}

	public boolean storesUpperCaseQuotedIdentifiers() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.storesUpperCaseQuotedIdentifiers();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","storesUpperCaseQuotedIdentifiers()", __start, __finish);
		return __ret;
	}

	public boolean supportsANSI92EntryLevelSQL() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsANSI92EntryLevelSQL();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsANSI92EntryLevelSQL()", __start, __finish);
		return __ret;
	}

	public boolean supportsANSI92FullSQL() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsANSI92FullSQL();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsANSI92FullSQL()", __start, __finish);
		return __ret;
	}

	public boolean supportsANSI92IntermediateSQL() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsANSI92IntermediateSQL();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsANSI92IntermediateSQL()", __start, __finish);
		return __ret;
	}

	public boolean supportsAlterTableWithAddColumn() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsAlterTableWithAddColumn();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsAlterTableWithAddColumn()", __start, __finish);
		return __ret;
	}

	public boolean supportsAlterTableWithDropColumn() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsAlterTableWithDropColumn();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsAlterTableWithDropColumn()", __start, __finish);
		return __ret;
	}

	public boolean supportsBatchUpdates() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsBatchUpdates();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsBatchUpdates()", __start, __finish);
		return __ret;
	}

	public boolean supportsCatalogsInDataManipulation() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsCatalogsInDataManipulation();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsCatalogsInDataManipulation()", __start, __finish);
		return __ret;
	}

	public boolean supportsCatalogsInIndexDefinitions() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsCatalogsInIndexDefinitions();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsCatalogsInIndexDefinitions()", __start, __finish);
		return __ret;
	}

	public boolean supportsCatalogsInPrivilegeDefinitions() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsCatalogsInPrivilegeDefinitions();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsCatalogsInPrivilegeDefinitions()", __start, __finish);
		return __ret;
	}

	public boolean supportsCatalogsInProcedureCalls() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsCatalogsInProcedureCalls();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsCatalogsInProcedureCalls()", __start, __finish);
		return __ret;
	}

	public boolean supportsCatalogsInTableDefinitions() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsCatalogsInTableDefinitions();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsCatalogsInTableDefinitions()", __start, __finish);
		return __ret;
	}

	public boolean supportsColumnAliasing() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsColumnAliasing();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsColumnAliasing()", __start, __finish);
		return __ret;
	}

	public boolean supportsConvert(int p0,int p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsConvert( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsConvert(int,int)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public boolean supportsConvert() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsConvert();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsConvert()", __start, __finish);
		return __ret;
	}

	public boolean supportsCoreSQLGrammar() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsCoreSQLGrammar();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsCoreSQLGrammar()", __start, __finish);
		return __ret;
	}

	public boolean supportsCorrelatedSubqueries() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsCorrelatedSubqueries();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsCorrelatedSubqueries()", __start, __finish);
		return __ret;
	}

	public boolean supportsDataDefinitionAndDataManipulationTransactions() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsDataDefinitionAndDataManipulationTransactions();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsDataDefinitionAndDataManipulationTransactions()", __start, __finish);
		return __ret;
	}

	public boolean supportsDataManipulationTransactionsOnly() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsDataManipulationTransactionsOnly();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsDataManipulationTransactionsOnly()", __start, __finish);
		return __ret;
	}

	public boolean supportsDifferentTableCorrelationNames() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsDifferentTableCorrelationNames();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsDifferentTableCorrelationNames()", __start, __finish);
		return __ret;
	}

	public boolean supportsExpressionsInOrderBy() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsExpressionsInOrderBy();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsExpressionsInOrderBy()", __start, __finish);
		return __ret;
	}

	public boolean supportsExtendedSQLGrammar() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsExtendedSQLGrammar();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsExtendedSQLGrammar()", __start, __finish);
		return __ret;
	}

	public boolean supportsFullOuterJoins() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsFullOuterJoins();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsFullOuterJoins()", __start, __finish);
		return __ret;
	}

	public boolean supportsGetGeneratedKeys() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsGetGeneratedKeys();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsGetGeneratedKeys()", __start, __finish);
		return __ret;
	}

	public boolean supportsGroupBy() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsGroupBy();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsGroupBy()", __start, __finish);
		return __ret;
	}

	public boolean supportsGroupByBeyondSelect() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsGroupByBeyondSelect();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsGroupByBeyondSelect()", __start, __finish);
		return __ret;
	}

	public boolean supportsGroupByUnrelated() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsGroupByUnrelated();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsGroupByUnrelated()", __start, __finish);
		return __ret;
	}

	public boolean supportsIntegrityEnhancementFacility() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsIntegrityEnhancementFacility();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsIntegrityEnhancementFacility()", __start, __finish);
		return __ret;
	}

	public boolean supportsLikeEscapeClause() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsLikeEscapeClause();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsLikeEscapeClause()", __start, __finish);
		return __ret;
	}

	public boolean supportsLimitedOuterJoins() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsLimitedOuterJoins();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsLimitedOuterJoins()", __start, __finish);
		return __ret;
	}

	public boolean supportsMinimumSQLGrammar() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsMinimumSQLGrammar();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsMinimumSQLGrammar()", __start, __finish);
		return __ret;
	}

	public boolean supportsMixedCaseIdentifiers() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsMixedCaseIdentifiers();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsMixedCaseIdentifiers()", __start, __finish);
		return __ret;
	}

	public boolean supportsMixedCaseQuotedIdentifiers() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsMixedCaseQuotedIdentifiers();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsMixedCaseQuotedIdentifiers()", __start, __finish);
		return __ret;
	}

	public boolean supportsMultipleOpenResults() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsMultipleOpenResults();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsMultipleOpenResults()", __start, __finish);
		return __ret;
	}

	public boolean supportsMultipleResultSets() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsMultipleResultSets();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsMultipleResultSets()", __start, __finish);
		return __ret;
	}

	public boolean supportsMultipleTransactions() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsMultipleTransactions();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsMultipleTransactions()", __start, __finish);
		return __ret;
	}

	public boolean supportsNamedParameters() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsNamedParameters();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsNamedParameters()", __start, __finish);
		return __ret;
	}

	public boolean supportsNonNullableColumns() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsNonNullableColumns();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsNonNullableColumns()", __start, __finish);
		return __ret;
	}

	public boolean supportsOpenCursorsAcrossCommit() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsOpenCursorsAcrossCommit();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsOpenCursorsAcrossCommit()", __start, __finish);
		return __ret;
	}

	public boolean supportsOpenCursorsAcrossRollback() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsOpenCursorsAcrossRollback();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsOpenCursorsAcrossRollback()", __start, __finish);
		return __ret;
	}

	public boolean supportsOpenStatementsAcrossCommit() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsOpenStatementsAcrossCommit();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsOpenStatementsAcrossCommit()", __start, __finish);
		return __ret;
	}

	public boolean supportsOpenStatementsAcrossRollback() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsOpenStatementsAcrossRollback();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsOpenStatementsAcrossRollback()", __start, __finish);
		return __ret;
	}

	public boolean supportsOrderByUnrelated() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsOrderByUnrelated();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsOrderByUnrelated()", __start, __finish);
		return __ret;
	}

	public boolean supportsOuterJoins() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsOuterJoins();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsOuterJoins()", __start, __finish);
		return __ret;
	}

	public boolean supportsPositionedDelete() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsPositionedDelete();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsPositionedDelete()", __start, __finish);
		return __ret;
	}

	public boolean supportsPositionedUpdate() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsPositionedUpdate();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsPositionedUpdate()", __start, __finish);
		return __ret;
	}

	public boolean supportsResultSetConcurrency(int p0,int p1) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsResultSetConcurrency( p0, p1);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsResultSetConcurrency(int,int)",new Object[] {p0,p1}, __start, __finish);
		return __ret;
	}

	public boolean supportsResultSetHoldability(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsResultSetHoldability( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsResultSetHoldability(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean supportsResultSetType(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsResultSetType( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsResultSetType(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean supportsSavepoints() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsSavepoints();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsSavepoints()", __start, __finish);
		return __ret;
	}

	public boolean supportsSchemasInDataManipulation() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsSchemasInDataManipulation();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsSchemasInDataManipulation()", __start, __finish);
		return __ret;
	}

	public boolean supportsSchemasInIndexDefinitions() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsSchemasInIndexDefinitions();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsSchemasInIndexDefinitions()", __start, __finish);
		return __ret;
	}

	public boolean supportsSchemasInPrivilegeDefinitions() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsSchemasInPrivilegeDefinitions();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsSchemasInPrivilegeDefinitions()", __start, __finish);
		return __ret;
	}

	public boolean supportsSchemasInProcedureCalls() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsSchemasInProcedureCalls();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsSchemasInProcedureCalls()", __start, __finish);
		return __ret;
	}

	public boolean supportsSchemasInTableDefinitions() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsSchemasInTableDefinitions();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsSchemasInTableDefinitions()", __start, __finish);
		return __ret;
	}

	public boolean supportsSelectForUpdate() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsSelectForUpdate();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsSelectForUpdate()", __start, __finish);
		return __ret;
	}

	public boolean supportsStatementPooling() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsStatementPooling();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsStatementPooling()", __start, __finish);
		return __ret;
	}

	public boolean supportsStoredProcedures() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsStoredProcedures();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsStoredProcedures()", __start, __finish);
		return __ret;
	}

	public boolean supportsSubqueriesInComparisons() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsSubqueriesInComparisons();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsSubqueriesInComparisons()", __start, __finish);
		return __ret;
	}

	public boolean supportsSubqueriesInExists() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsSubqueriesInExists();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsSubqueriesInExists()", __start, __finish);
		return __ret;
	}

	public boolean supportsSubqueriesInIns() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsSubqueriesInIns();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsSubqueriesInIns()", __start, __finish);
		return __ret;
	}

	public boolean supportsSubqueriesInQuantifieds() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsSubqueriesInQuantifieds();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsSubqueriesInQuantifieds()", __start, __finish);
		return __ret;
	}

	public boolean supportsTableCorrelationNames() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsTableCorrelationNames();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsTableCorrelationNames()", __start, __finish);
		return __ret;
	}

	public boolean supportsTransactionIsolationLevel(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsTransactionIsolationLevel( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsTransactionIsolationLevel(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean supportsTransactions() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsTransactions();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsTransactions()", __start, __finish);
		return __ret;
	}

	public boolean supportsUnion() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsUnion();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsUnion()", __start, __finish);
		return __ret;
	}

	public boolean supportsUnionAll() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.supportsUnionAll();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","supportsUnionAll()", __start, __finish);
		return __ret;
	}

	public boolean updatesAreDetected(int p0) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.updatesAreDetected( p0);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","updatesAreDetected(int)",new Object[] {p0}, __start, __finish);
		return __ret;
	}

	public boolean usesLocalFilePerTable() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.usesLocalFilePerTable();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","usesLocalFilePerTable()", __start, __finish);
		return __ret;
	}

	public boolean usesLocalFiles() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.usesLocalFiles();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","usesLocalFiles()", __start, __finish);
		return __ret;
	}

	public java.lang.String getURL() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getURL();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getURL()", __start, __finish);
		return __ret;
	}

	public boolean isReadOnly() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final boolean __ret = delegate.isReadOnly();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","isReadOnly()", __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getAttributes(java.lang.String p0,java.lang.String p1,java.lang.String p2,java.lang.String p3) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getAttributes( p0, p1, p2, p3);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getAttributes(java.lang.String,java.lang.String,java.lang.String,java.lang.String)",new Object[] {p0,p1,p2,p3}, __start, __finish);
		return __ret;
	}

	public java.lang.String getUserName() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.lang.String __ret = delegate.getUserName();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getUserName()", __start, __finish);
		return __ret;
	}

	public java.sql.Connection getConnection() throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.Connection __ret = delegate.getConnection();
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getConnection()", __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getColumns(java.lang.String p0,java.lang.String p1,java.lang.String p2,java.lang.String p3) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getColumns( p0, p1, p2, p3);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getColumns(java.lang.String,java.lang.String,java.lang.String,java.lang.String)",new Object[] {p0,p1,p2,p3}, __start, __finish);
		return __ret;
	}

	public java.sql.ResultSet getTables(java.lang.String p0,java.lang.String p1,java.lang.String p2,java.lang.String[] p3) throws java.sql.SQLException {
		final long __start = System.currentTimeMillis();
		final java.sql.ResultSet __ret = delegate.getTables( p0, p1, p2, p3);
		final long __finish = System.currentTimeMillis();
		callLogger.log(connectionID, 0, "DatabaseMetaData","getTables(java.lang.String,java.lang.String,java.lang.String,java.lang.String[])",new Object[] {p0,p1,p2,p3}, __start, __finish);
		return __ret;
	}

}
