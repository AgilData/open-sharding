package org.opensharding.tpcc;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;


public class TpccLoad implements TpccConstants {
	/* Global SQL Variables */
	static long count_ware = 0;
	static int fd = 0;
	static int seed = 0;

	static int particle_flg = 0; /* "1" means particle mode */
	static int part_no = 0; /* 1:items 2:warehouse 3:customer 4:orders */
	static long min_ware = 1;
	static long max_ware;

	/* Global Variables */
	static int i = 0;
	static int is_local = 1;           /* "1" mean local */
	static int DB_STRING_MAX = 51;
	static boolean option_debug = false;	/* 1 if generating debug output    */

	
	private static void parseHost(String host, String from)
	{
	  if( !from.contains(":") ){
		  host = from;
	  }
	  else{
		  String[] tempSplit;
		  tempSplit = from.split(":");
		  host = tempSplit[1];
	  }
	 host = host + '\0';
	}
	
	private static int parsePort(String from)
	{
	  if(!from.contains(":"))
	    return 3306;
	  else
	  {
		String[] tempPort;
		tempPort = from.split(":");
		int portValue = Integer.parseInt(tempPort[1]);
		if( portValue <= 0xFFFF ){
		    return portValue;
	    } else {
	      System.out.printf("Incorrect port value: %s\n");
	      return -1;
	    }
	    
	  }
	  
	}
	
	/*
	 * ==================================================================+ |
	 * main() | ARGUMENTS |      Warehouses n [Debug] [Help]
	 * +==================================================================
	 */
	@SuppressWarnings("null")
	public static void main(String[] argv){
		String connect_string = null;
		String db_string = null;
		String db_user = null;
		String db_password = null;
	    int port= 3306;
	    int shardCount = 0;

		System.out.printf("*************************************\n");
		System.out.printf("*** ###easy### TPC-C Data Loader  ***\n");
		System.out.printf("*************************************\n");

		/* Parse args */
		if (argv.length != 11) {
		    if (argv.length != 8) {
			System.out.printf("\n usage: tpcc_load [server] [DB] [user] [pass] [warehouse] [shardCount] [Class}\n"
								+ "      OR\n"
								+ "        tpcc_load [server] [DB] [user] [pass] [warehouse] [shardCount] [Class] [part] [min_wh] [max_wh]\n\n"
								+ "           * [part]: 1=ITEMS 2=WAREHOUSE 3=CUSTOMER 4=ORDERS\n");
			System.exit(1);
		    }
		}else{
		    particle_flg = 1;
		}

		if ( argv[1].length() >= DB_STRING_MAX ) {
		    System.out.printf("\n server phrase is too long\n");
		    System.exit(1);
		}
		if (argv[2].length() >= DB_STRING_MAX ) {
		    System.out.printf("\n DBname phrase is too long\n");
		    System.exit(1);
		}
		if ( argv[3].length() >= DB_STRING_MAX ) {
		    System.out.printf("\n user phrase is too long\n");
		    System.exit(1);
		}
		if ( argv[4].length() >= DB_STRING_MAX ) {
		    System.out.printf("\n pass phrase is too long\n");
		    System.exit(1);
		}
		if ((count_ware = Integer.parseInt(argv[5])) <= 0) {
		    System.out.printf("\n expecting positive number of warehouses\n");
		    System.exit(1);
		}
	    //strcpy(connect_string, argv[1]);
	    /*parseHost(connect_string, argv[1]);
	    if(connect_string == null){
	    	System.out.println("Failed to parse the host.");
	    	System.exit(1);
	    }*/
		connect_string = argv[1];
	    port= parsePort(argv[1]);
		db_string = argv[2];
		db_user = argv[3];
		db_password = argv[4];
		shardCount = Integer.parseInt(argv[6]);
		String connectionClass = argv[7];

		if(connect_string.equals("1")){
		  is_local = 1;
		}else{
		  is_local = 0;
		}

		if(particle_flg==1){
		    part_no = Integer.parseInt(argv[8]);
		    min_ware = Integer.parseInt(argv[9]);
		    max_ware = Integer.parseInt(argv[10]);
		}else{
		    min_ware = 1;
		    max_ware = count_ware;
		}

		System.out.printf("<Parameters>\n");
		if(is_local==0) System.out.printf("     [server]: %s\n", connect_string);
		if(is_local==0) System.out.printf("     [port]: %d\n", port);
		System.out.printf("     [DBname]: %s\n", db_string);
		System.out.printf("       [user]: %s\n", db_user);
		System.out.printf("       [pass]: %s\n", db_password);

		System.out.printf("  [warehouse]: %d\n", count_ware);

		if(particle_flg==1){
		    System.out.printf("  [part(1-4)]: %d\n", part_no);
		    System.out.printf("     [MIN WH]: %d\n", min_ware);
		    System.out.printf("     [MAX WH]: %d\n", max_ware);
		}

		//TODO: Pass the seed in as a variable.
		Util.setSeed(seed);


		/* EXEC SQL WHENEVER SQLERROR GOTO Error_SqlCall; */
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
				throw new RuntimeException("Connection to local host error", e);
			}
		
		
		Statement stmt;
		try {
			stmt = conn.createStatement();
		} catch (SQLException e) {
			throw new RuntimeException("Could not create statement", e);
		}
		try {
			stmt.execute("SET UNIQUE_CHECKS=0");
		} catch (SQLException e) {
			throw new RuntimeException("Could not set unique checks error", e);
		}
		try {
			stmt.execute("SET FOREIGN_KEY_CHECKS=0");
			stmt.close();
		} catch (SQLException e) {
			throw new RuntimeException("Could not set foreign key checks error", e);
		}
	

		System.out.printf("TPCC Data Load Started...\n");

		if(particle_flg==0){
			System.out.printf("Particle flag: %d", particle_flg);
			//Load.loadItems(conn, shardCount, option_debug);
		   // Load.loadWare(conn, shardCount, (int)min_ware, (int)max_ware, option_debug);
		   Load.loadCust(conn, shardCount, (int)min_ware, (int)max_ware);
		   Load.loadOrd(conn, shardCount, (int)min_ware, (int)max_ware);
		}else if(particle_flg==1){
		    switch(part_no){
			case 1:
				 Load.loadItems(conn, shardCount, option_debug);
				 break;
			case 2:
			    Load.loadWare(conn, shardCount, (int)min_ware, (int)max_ware, option_debug);
			    break;
			case 3:
			    Load.loadCust(conn, shardCount, (int)min_ware, (int)max_ware);
			    break;
			case 4:
			    Load.loadOrd(conn, shardCount, (int)min_ware, (int)max_ware);;
			    break;
			default:
			    System.out.printf("Unknown part_no\n");
			    System.out.printf("1:ITEMS 2:WAREHOUSE 3:CUSTOMER 4:ORDERS\n");
		    }
		}

		System.out.printf("\n...DATA LOADING COMPLETED SUCCESSFULLY.\n");
		System.exit(0);

	}


}
