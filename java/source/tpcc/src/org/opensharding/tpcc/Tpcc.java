package org.opensharding.tpcc;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Calendar;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class Tpcc {
	
	private static final Logger logger = LogManager.getLogger(Tpcc.class);
	private static final boolean DEBUG = logger.isDebugEnabled();
	
	/* Global SQL Variables */
	static int DB_STRING_MAX = 128;
	static int MAX_CLUSTER_SIZE = 128;

	static String connect_string;

	static String db_string;
	static String db_host;
	static String db_user;
	static String db_password;
	static String report_file = null;
	static String trx_file = null;
	static BufferedWriter freport_file;
	static BufferedWriter ftrx_file;

	static int num_ware;
	static int num_conn;
	static int lampup_time;
	static int measure_time;

	static int num_node; /* number of servers that consists of cluster i.e. RAC (0:normal mode)*/
	static int  NUM_NODE_MAX = 8;
	static String node_string;

	static int time_count;
	static int PRINT_INTERVAL=10;

	static int[] success = new int[5];
	static int[] late = new int[5];
	static int[] retry = new int[5];
	static int[] failure = new int[5];

	static int[][] success2;
	static int[][] late2;
	static int[][] retry2;
	static int[][] failure2;

	static int[] success2_sum = new int[5];
	static int[] late2_sum = new int[5];
	static int[] retry2_sum = new int[5];
	static int[] failure2_sum = new int[5];

	static int[] prev_s = new int[5];
	static int[] prev_l = new int[5];

	static double[] max_rt = new double[5];
	static double[] cur_max_rt = new double[5];

	static int counting_on;

	long clk_tck;

	static int is_local = 0; /* "1" mean local */
	static int valuable_flg = 0; /* "1" mean valuable ratio */
	static int[] ctx;
	public static int activate_transaction = 0;;
	
	public static void main( String[] argv)
	{
	  int i, k, t_num, arg_offset, c;
	  long m;
	  float f;
	
	  int port= 3306;
	  int fd, seed;
	  
	  String connectionClass = null;
	  
	  Counter count = new Counter();

	  logger.info("***************************************");
	  logger.info("****** Java TPC-C Load Generator ******");
	  logger.info("***************************************");

	  /* initialize */
	  RtHist.histInit();
	  activate_transaction = 1;
	  counting_on = 0;

	  for ( i=0; i<5; i++ ){
	    success[i]=0;
	    late[i]=0;
	    retry[i]=0;
	    failure[i]=0;

	    prev_s[i]=0;
	    prev_l[i]=0;

	    max_rt[i]=0.0;
	  }
	  /* dummy initialize*/
	  num_ware = 1;
	  num_conn = 10;
	  lampup_time = 10;
	  measure_time = 20;
	  db_string = "tpcc";

	  /* number of node (default 0) */
	  num_node = 0;
	  arg_offset = 0;


	  /* Parse args */
	  if (argv.length == 0){
		  logger.error("Arguments need to be provided.");
		  System.exit(0);
	  }

	
		
		for(int j = 0; j < argv.length; j= j+2) {
	    	if (argv[j].equals("-h")){
	    		System.out.printf("Option h with value %s \n", argv[j+1]);
	    		connect_string = argv[j+1];
	    	}else if(argv[j].equals("-d")){
	    		System.out.printf("Option d with value %s \n", argv[j+1]);
	    		db_string = argv[j+1];
	    	}else if(argv[j].equals("-u")){
	    		System.out.printf("Option u with value %s \n", argv[j+1]);
	    		db_user = argv[j+1];
	    	}else if(argv[j].equals("-p")){
	    		System.out.printf("Option p with value %s \n", argv[j+1]);
	    		db_password = argv[j+1];
	    	}else if(argv[j].equals("-f")){
	    		System.out.printf("Option f with value %s \n", argv[j+1]);
	    		report_file = argv[j+1];
	    	}else if(argv[j].equals("-t")){
	    		System.out.printf("Option t with value %s \n", argv[j+1]);
	    		trx_file = argv[j+1];
	    	}else if(argv[j].equals("-w")){
	    		System.out.printf("Option w with value %s \n", argv[j+1]);
	    		num_ware = Integer.parseInt(argv[j+1]) * -1;
	    	}else if(argv[j].equals("-c")){
	    		System.out.printf("Option c with value %s \n", argv[j+1]);
	    		num_conn = Integer.parseInt(argv[j+1]);
	    	}else if(argv[j].equals("-r")){
	    		System.out.printf("Option r with value %s \n", argv[j+1]);
	    		lampup_time = Integer.parseInt(argv[j+1]);
	    	}else if(argv[j].equals("-l")){
	    		System.out.printf("Option l with value %s \n", argv[j+1]);
	    		measure_time = Integer.parseInt(argv[j+1]);
	    	}else if(argv[j].equals("-i")){
	    		System.out.printf("Option i with value %s \n", argv[j+1]);
	    		PRINT_INTERVAL = Integer.parseInt(argv[j+1]);
	    	}else if(argv[j].equals("-P")){
	    		System.out.printf("Option P with value %s \n", argv[j+1]);
	    		port = Integer.parseInt(argv[j+1]);
	    	}else if(argv[j].equals("-U")){
	    		System.out.printf("Option U with value %s \n", argv[j+1]);
	    		connectionClass = argv[j+1];
	    	}else if(argv[j].equals("-?")){
	    		System.out.printf("Usage: tpcc_start -h server_host -P port -d database_name -u mysql_user -p mysql_password -w warehouses -c connections -r warmup_time -l running_time -i report_interval -f report_file -t trx_file\n");
	            System.exit(0);
	    	}else{
	    		logger.error("?? returned character code 0 %s ??\n", argv[j]);
	    	}
	    }
	    
	    if(!db_string.equals("l")){
	        is_local = 1;
	      }else{
	        is_local = 0;
	      }

	      if(valuable_flg==1){
	        if( (Integer.parseInt(argv[9 + arg_offset]) < 0) || 
	        	(Integer.parseInt(argv[10 + arg_offset]) < 0)||
	        	(Integer.parseInt(argv[11 + arg_offset]) < 0)||
	        	(Integer.parseInt(argv[12 + arg_offset]) < 0)||
	        	(Integer.parseInt(argv[13 + arg_offset]) < 0) ) {
	        		logger.error(" expecting positive number of ratio parameters");
	        		System.exit(1);
	        }
	      }
	      
	      if( num_node > 0 ){
	    	    if( num_ware % num_node != 0 ){
	    	      logger.error(" [warehouse] value must be devided by [num_node].");
	    	      System.exit(1);
	    	    }
	    	    if( num_conn % num_node != 0 ){
	    	      logger.error("[connection] value must be devided by [num_node].");
	    	      System.exit(1);
	    	    }
	    	  }
	


    	  System.out.printf("<Parameters>\n");
    	  if(is_local==0) {
    	    System.out.printf("     [server]: ");
    	    System.out.printf("%s",  connect_string);
    	    System.out.printf("\n");
    	  }
    	  
    	  if(is_local==0){
    		  System.out.printf("     [port]: %d\n", port);
    	  }

    	  System.out.printf("     [DBname]: %s\n", db_string);
    	  System.out.printf("       [user]: %s\n", db_user);
    	  System.out.printf("       [pass]: %s\n", db_password);

    	  System.out. printf("  [warehouse]: %d\n", num_ware);
    	  System.out.printf(" [connection]: %d\n", num_conn);
    	  System.out. printf("     [rampup]: %d (sec.)\n", lampup_time);
    	  System.out. printf("    [measure]: %d (sec.)\n", measure_time);

    	  if(valuable_flg==1){
    	    System.out.printf("      [ratio]: %d:%d:%d:%d:%d\n", Integer.parseInt(argv[9 + arg_offset]), Integer.parseInt(argv[10 + arg_offset]),
    		   Integer.parseInt(argv[11 + arg_offset]), Integer.parseInt(argv[12 + arg_offset]), Integer.parseInt(argv[13 + arg_offset]) );
    	  }
    	  
		

		  if(valuable_flg==0){
		    Util.seqInit(10,10,1,1,1); /* normal ratio */
		  }else{
		    Util.seqInit( Integer.parseInt(argv[9 + arg_offset]), Integer.parseInt(argv[10 + arg_offset]), Integer.parseInt(argv[11 + arg_offset]),
			     Integer.parseInt(argv[12 + arg_offset]), Integer.parseInt(argv[13 + arg_offset]) );
		  }

		  /* set up threads */
		  
		if(DEBUG) logger.debug("Creating TpccThread");
		ExecutorService executor = Executors.newFixedThreadPool(num_conn);
	
		// Start each server.

		for(i =0; i<num_conn; i++){
			Runnable worker = new TpccThread(i, port, 1, connect_string, db_user, db_password, db_string, num_ware, num_conn, count, connectionClass);
			executor.execute(worker);
		}
		
		
		Calendar calendar = Calendar.getInstance();
		int initialtime = calendar.get(Calendar.SECOND);
		int time = 0;
		System.out.printf("\nMEASURING START.\n\n");

		if(DEBUG) logger.debug("main: measure_time: " + measure_time + " intial time: " + initialtime);
		
		do{
			calendar = Calendar.getInstance();
			time = calendar.get(Calendar.SECOND);
			
		}while( measure_time  > (time - initialtime) );
		
		System.out.printf("\nSTOPPING THREADS\n");
		
		activate_transaction = 0;
		
		executor.shutdown();
		
		while(!executor.isTerminated()) {
		}
		
	
		System.out.printf("TOTAL TRANSACTIONS: %d\n", count.getTotal());
		System.out.printf("TOTAL TPMS: %f\n", (float)count.getTotal()/(float)measure_time);
		
		

		  RtHist.histReport();
		  System.exit(0);
	}
			

}

