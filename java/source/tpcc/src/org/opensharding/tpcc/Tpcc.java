package org.opensharding.tpcc;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Calendar;
import java.util.Properties;
import java.util.Timer;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class Tpcc {
	
	private static final Logger logger = LogManager.getLogger(Tpcc.class);
	private static final boolean DEBUG = logger.isDebugEnabled();
	
	private static final String DRIVER = "DRIVER";
	private static final String WAREHOUSECOUNT = "WAREHOUSECOUNT";
	private static final String HOST = "HOST";
	private static final String DATABASE = "DATABASE";
	private static final String USER = "USER";
	private static final String PASSWORD = "PASSWORD";
	private static final String CONNECTIONS = "CONNECTIONS";
	private static final String RAMPUPTIME = "RAMPUPTIME";
	private static final String DURATION = "DURATION";
	private static final String JDBCURL = "JDBCURL";
	
	private static final String PROPERTIESFILE = "tpcc.properties";
	
	
	/* Global SQL Variables */
	private int DB_STRING_MAX = 128;
	private int MAX_CLUSTER_SIZE = 128;

	private String connect_string;

	private String db_string;
	private String db_host;
	private String db_user;
	private String db_password;


	private int num_ware;
	private int num_conn;
	private int rampup_time;
	private int measure_time;
	private String javaDriver;
	private String jdbcUrl;

	private int num_node; /* number of servers that consists of cluster i.e. RAC (0:normal mode)*/
	private int  NUM_NODE_MAX = 8;
	private String node_string;

	private int time_count;
	private int PRINT_INTERVAL=10;

	private int[] success = new int[5];
	private int[] late = new int[5];
	private int[] retry = new int[5];
	private int[] failure = new int[5];

	private int[] prev_s = new int[5];
	private int[] prev_l = new int[5];

	private double[] max_rt = new double[5];
	private int port= 3306;
	private Counter count = new Counter();

	private Properties properties;
	private InputStream inputStream;

	public static int activate_transaction = 0;

	public Tpcc() {
		// Empty.
	}
	
	private void init() {
		
		  
		  
		  logger.info("Loading properties from: " + PROPERTIESFILE);
		  
		  properties = new Properties();
		  inputStream = getClass().getClassLoader().getResourceAsStream(PROPERTIESFILE);
		  if(inputStream == null){
			  throw new RuntimeException("Failed to access properties.");
		  }
		  try{
			  properties.load(inputStream);
		  }catch (IOException e){
			  throw new RuntimeException("Error loading properties file", e);
		  }
		
	}
	
	private void stopTransactions(){
		activate_transaction = 0;
	}
	
	private int runBenchmark() {
		
		logger.info("***************************************");
		logger.info("****** Java TPC-C Load Generator ******");
		logger.info("***************************************");
		
		/* initialize */
		RtHist.histInit();
		activate_transaction = 1;
		Timer timer1 = new Timer();  
		
		for (int i=0; i<5; i++ ){
		    success[i]=0;
		    late[i]=0;
		    retry[i]=0;
		    failure[i]=0;
		
		    prev_s[i]=0;
		    prev_l[i]=0;
		
		    max_rt[i]=0.0;
		}
		
		
		/* number of node (default 0) */
		num_node = 0;
		  
		connect_string = properties.getProperty(HOST);
		db_string = properties.getProperty(DATABASE);
		db_user = properties.getProperty(USER);
		db_password = properties.getProperty(PASSWORD);
		num_ware = Integer.parseInt(properties.getProperty(WAREHOUSECOUNT));
		num_conn = Integer.parseInt(properties.getProperty(CONNECTIONS));
		rampup_time = Integer.parseInt(properties.getProperty(RAMPUPTIME));
		measure_time = Integer.parseInt(properties.getProperty(DURATION));
		javaDriver = properties.getProperty(DRIVER);
		jdbcUrl = properties.getProperty(JDBCURL);
		  
		if(connect_string == null){
			throw new RuntimeException("Host is null.");
		}
		if(db_string == null){
			  throw new RuntimeException("Database name is null.");
		}
		if(db_user == null){
			  throw new RuntimeException("User is null.");
		}
		if(db_password == null){
			  throw new RuntimeException("Password is null.");
		}
		if(num_ware < 1){
			  throw new RuntimeException("Warehouse count has to be greater than or equal to 1.");
		}
		if(num_conn < 1){
			  throw new RuntimeException("Connections has to be greater than or equal to 1.");
		}
		if(rampup_time < 1){
			  throw new RuntimeException("Rampup time has to be greater than or equal to 1.");
		}
		if(measure_time < 1){
			throw new RuntimeException("Duration has to be greater than or equal to 1.");
		}
		if(javaDriver == null){
			  throw new RuntimeException("Java Driver is null.");
		}
		if(jdbcUrl == null){
			  throw new RuntimeException("JDBC Url is null.");
		}
		
		if( num_node > 0 ){
			  if( num_ware % num_node != 0 ){
				  logger.error(" [warehouse] value must be devided by [num_node].");
			  return 1;
			  }
			if( num_conn % num_node != 0 ) {
				  logger.error("[connection] value must be devided by [num_node].");
					  return 1;
			}
		}
		
		      
		long delay1 = measure_time*1000;
		
		System.out.printf("<Parameters>\n");
		 
		System.out.printf("     [server]: ");
		System.out.printf("%s",  connect_string);
		
		System.out.printf("     [DBname]: %s\n", db_string);
		System.out.printf("       [user]: %s\n", db_user);
		System.out.printf("       [pass]: %s\n", db_password);
		
		System.out. printf("  [warehouse]: %d\n", num_ware);
		System.out.printf("  [connection]: %d\n", num_conn);
		System.out. printf("     [rampup]: %d (sec.)\n", rampup_time);
		System.out. printf("    [measure]: %d (sec.)\n", measure_time);
		System.out.printf("	   [driver]: %s\n", javaDriver);
		System.out.printf("		  [URL]: %s\n", jdbcUrl);
		
		Util.seqInit(10,10,1,1,1);
		
		
		/* set up threads */
		  
		if(DEBUG) logger.debug("Creating TpccThread");
		ExecutorService executor = Executors.newFixedThreadPool(num_conn);
		
		// Start each server.
		
		for(int i =0; i<num_conn; i++) {
			Runnable worker = new TpccThread(i, port, 1, connect_string, db_user, db_password, db_string, num_ware, num_conn, count, javaDriver, jdbcUrl);
			executor.execute(worker);
		}
		
		
		long initialtime = System.currentTimeMillis();
		
		System.out.printf("\nMEASURING START.\n\n");
		
		long currentTime = 0;
		
		while(measure_time > ((currentTime - initialtime) / 1000.0) ){
			currentTime = System.currentTimeMillis();
			if( ((currentTime - initialtime) / 1000.0) % 1 == 0 ){
				System.out.println("Current execution time lapse: " + ((currentTime - initialtime) / 1000.0) % 1 );
			}
		}
		
		System.out.printf("\nSTOPPING THREADS\n");
		
		
		executor.shutdown();
		
		while(!executor.isTerminated()) {
		}
		
		
		System.out.printf("TOTAL TRANSACTIONS: %d\n", count.getTotal());
		System.out.printf("TOTAL TPMS: %f\n", (float)count.getTotal()/(float)measure_time);
		
		
		//TODO: To be implemented better later.
		//RtHist.histReport();
		return 0;

	}
	
	public static void main( String[] argv)
	{
		Tpcc tpcc = new Tpcc();
		tpcc.init();
		int ret = tpcc.runBenchmark();
		System.exit(ret);
	}
			

}

