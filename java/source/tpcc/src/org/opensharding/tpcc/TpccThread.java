package org.opensharding.tpcc;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class TpccThread extends Thread {

	private static final Logger logger = LogManager.getLogger(TpccThread.class);
	private static final boolean DEBUG = logger.isDebugEnabled();
	
	int number;
	int port;
	int is_local;
	int num_ware;
	int num_conn;
	String connect_string;
	String db_user;
	String db_password;
	String db_string;
	String connectionClass;
	
	TpccStatements pStmts;
	Counter count;
	
	public TpccThread(int number, int port, int is_local, String connect_string, String db_user, String db_password, String db_string, int num_ware, int num_conn, Counter count, String connectionClass) {
		 this.number = number;
		 this.port = port;
		 this.connect_string = connect_string;
		 this.db_password = db_password;
		 this.db_string = db_string;
		 this.db_user = db_user;
		 this.is_local = is_local;
		 this.num_conn = num_conn;
		 this.num_ware -= num_ware;
		 this.count = count;
		 this.connectionClass = connectionClass;
	}
	
	public void run() {
		 logger.info("Connection to DBSHARDS.");
		  try {
				Class.forName(connectionClass); 
			} catch (ClassNotFoundException e1) {
				throw new RuntimeException("Class for mysql error", e1);
			}
			String dbUrl = null;
			Connection conn;
			
	
			dbUrl = "jdbc:dbshards:" + db_string;
			try {
				conn = DriverManager.getConnection (dbUrl, db_user, db_password);
			} catch (SQLException e) {
				// TODO Auto-generated catch block
				throw new RuntimeException("Connection to specific host error", e);
			}

	  

			try {
				conn.setAutoCommit(false);
			} catch (SQLException e) {
				throw new RuntimeException("Set AutoCommit error", e);
			}
			
			// Create a driver instance.
			Driver driver = new Driver(conn);
		 
			if(DEBUG) logger.debug("Starting driver with: number: " + number + " num_ware: " + num_ware + " num_conn: " + num_conn);
			driver.runTransaction(number, count, num_ware, num_conn);
	

	
		  System.out.printf(".");

	
		}
}

