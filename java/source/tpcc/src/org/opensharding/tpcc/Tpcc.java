package org.opensharding.tpcc;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Tpcc{
	
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

	static int activate_transaction;
	static int counting_on;

	long clk_tck;

	static int is_local = 0; /* "1" mean local */
	static int valuable_flg = 0; /* "1" mean valuable ratio */
	static int[] ctx;
	
	public static void main( String[] argv)
	{
	  int i, k, t_num, arg_offset, c;
	  long m;
	  float f;
	  //pthread_t t;
	  //thread_arg thd_arg;
	 // timer_t timer;
	 // struct itimerval itval;
	 // struct sigaction  sigact;
	  int port= 3306;
	  int fd, seed;

	  System.out.printf("***************************************\n");
	  System.out.printf("*** ###easy### TPC-C Load Generator ***\n");
	  System.out.printf("***************************************\n");

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
	  
	 // clk_tck = sysconf(_SC_CLK_TCK);

	  /* Parse args */
	  if (argv.length == 0){
		  System.out.println("Arguments need to be provided.");
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
	    		num_ware = Integer.parseInt(argv[j+1]);
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
	    	}else if(argv[j].equals("-?")){
	    		System.out.printf("Usage: tpcc_start -h server_host -P port -d database_name -u mysql_user -p mysql_password -w warehouses -c connections -r warmup_time -l running_time -i report_interval -f report_file -t trx_file\n");
	            System.exit(0);
	    	}else{
	    		System.out.printf ("?? returned character code 0 %s ??\n", argv[j]);
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
	        		System.out.printf("\n expecting positive number of ratio parameters\n");
	        		System.exit(1);
	        }
	      }
	      
	      if( num_node > 0 ){
	    	    if( num_ware % num_node != 0 ){
	    	      System.out.printf("\n [warehouse] value must be devided by [num_node].\n");
	    	      System.exit(1);
	    	    }
	    	    if( num_conn % num_node != 0 ){
	    	      System.out.printf("\n [connection] value must be devided by [num_node].\n");
	    	      System.exit(1);
	    	    }
	    	  }
	      
	      if ( report_file.length() > 0 ) {
	    	    try {
					BufferedWriter freport_file = new BufferedWriter(new FileWriter(report_file));
				} catch (IOException e) {
					throw new RuntimeException("Could not open report_file", e);
				}
	      }
	      if (trx_file.length() > 0 ) {
	    	    try {
					BufferedWriter ftrx_file = new BufferedWriter(new FileWriter(trx_file));
				} catch (IOException e) {
					throw new RuntimeException("Could not open trx_file", e);
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
    	  
//    	  /* alarm initialize */
//    	  time_count = 0;
//    	  itval.it_interval.tv_sec = PRINT_INTERVAL;
//    	  itval.it_interval.tv_usec = 0;
//    	  itval.it_value.tv_sec = PRINT_INTERVAL;
//    	  itval.it_value.tv_usec = 0;
//    	  sigact.sa_handler = alarm_handler;
//    	  sigact.sa_flags = 0;
//    	  sigemptyset(&sigact.sa_mask);
//
//    	  /* setup handler&timer */
//    	  if( sigaction( SIGALRM, &sigact, NULL ) == -1 ) {
//    	    fprintf(stderr, "error in sigaction()\n");
//    	    exit(1);
//    	  }
//
//    	  fd = open("/dev/urandom", O_RDONLY);
//    	  if (fd == -1) {
//    	    fd = open("/dev/random", O_RDONLY);
//    	    if (fd == -1) {
//    	      struct timeval  tv;
//    	      gettimeofday(&tv, NULL);
//    	      seed = (tv.tv_sec ^ tv.tv_usec) * tv.tv_sec * tv.tv_usec ^ tv.tv_sec;
//    	    }else{
//    	      read(fd, &seed, sizeof(seed));
//    	      close(fd);
//    	    }
//    	  }else{
//    	    read(fd, &seed, sizeof(seed));
//    	    close(fd);
//    	  }
//    	  SetSeed(seed);
		

		  if(valuable_flg==0){
		    Util.seqInit(10,10,1,1,1); /* normal ratio */
		  }else{
		    Util.seqInit( Integer.parseInt(argv[9 + arg_offset]), Integer.parseInt(argv[10 + arg_offset]), Integer.parseInt(argv[11 + arg_offset]),
			     Integer.parseInt(argv[12 + arg_offset]), Integer.parseInt(argv[13 + arg_offset]) );
		  }
		  success2 = new int[5][num_conn];
		  late2 = new int[5][num_conn];
		  retry2 = new int[5][num_conn];
		  failure2 = new int[5][num_conn];
		  /* set up each counter */
		  for ( i=0; i<5; i++ ){
		      for ( k=0; k<num_conn; k++ ){
				  success2[i][k] = 0;
				  late2[i][k] = 0;
				  retry2[i][k] = 0;
				  failure2[i][k] = 0;
		      }
		  }

		  /* set up threads */

//		  t = malloc( sizeof(pthread_t) * num_conn );
//		  if ( t == NULL ){
//		    fprintf(stderr, "error at malloc(pthread_t)\n");
//		    exit(1);
//		  }
//		  thd_arg = malloc( sizeof(thread_arg) * num_conn );
//		  if( thd_arg == NULL ){
//		    fprintf(stderr, "error at malloc(thread_arg)\n");
//		    exit(1);
//		  }
		  
		ExecutorService executor = Executors.newFixedThreadPool(1);

		// Start each server.
		// TODO: From configuration.
		Runnable worker = new TpccThread(1, port, 1, connect_string, db_user, db_password, db_string, num_ware, num_conn);
		executor.execute(worker);
		
		  ctx = new int[num_conn];
//		  stmt = malloc( sizeof(MYSQL_STMT **) * num_conn );
//		  for( i=0; i < num_conn; i++ ){
//		      stmt[i] = malloc( sizeof(MYSQL_STMT *) * 40 );
//		  }

		  if ( ctx.length == 0 ){
		    System.out.printf("error at ctx size\n");
		    System.exit(1);
		  }
		  
		  /* EXEC SQL WHENEVER SQLERROR GOTO sqlerr; */

//		  for( i=0; i < num_conn; i++ ){
//		    ctx[i] = mysql_init(NULL);
//		    if(!ctx[i]) goto sqlerr;
//		  }

//		  for( t_num=0; t_num < num_conn; t_num++ ){
//		    thd_arg[t_num].port= port;
//		    thd_arg[t_num].number= t_num;
//		    pthread_create( &t[t_num], NULL, (void *)thread_main, (void *)&(thd_arg[t_num]) );
//		  }

		  System.out.printf("\nMEASURING START.\n\n");

		  /* sleep(measure_time); */
		  /* start timer */

//		#ifndef _SLEEP_ONLY_
//		  if( setitimer(ITIMER_REAL, &itval, NULL) == -1 ) {
//		    fprintf(stderr, "error in setitimer()\n");
//		  }
//		#endif

		  counting_on = 1;
		  /* wait signal */
//		  for(i = 0; i < (measure_time / PRINT_INTERVAL); i++ ) {
//		#ifndef _SLEEP_ONLY_
//		    pause();
//		#else
//		    sleep(PRINT_INTERVAL);
//		    alarm_dummy();
//		#endif
//		  }
		  counting_on = 0;

//		#ifndef _SLEEP_ONLY_
//		  /* stop timer */
//		  itval.it_interval.tv_sec = 0;
//		  itval.it_interval.tv_usec = 0;
//		  itval.it_value.tv_sec = 0;
//		  itval.it_value.tv_usec = 0;
//		  if( setitimer(ITIMER_REAL, &itval, NULL) == -1 ) {
//		    fprintf(stderr, "error in setitimer()\n");
//		  }
//		#endif

		  System.out.printf("\nSTOPPING THREADS");
		  activate_transaction = 0;

//		  /* wait threads' ending and close connections*/
//		  for( i=0; i < num_conn; i++ ){
//		    pthread_join( t[i], NULL );
//		  }
		  
		  

//		  free(ctx);
//		  for( i=0; i < num_conn; i++ ){
//		      free(stmt[i]);
//		  }
//		  free(stmt);
//
//		  free(t);
//		  free(thd_arg);

		  //hist_report();
		  if (freport_file != null)
			try {
				freport_file.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				throw new RuntimeException("Could not close file", e);
			}

		  if (ftrx_file != null)
			try {
				ftrx_file.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				throw new RuntimeException("Could not close file", e);
			}

		  System.out.printf("\n<Raw Results>\n");
		  for ( i=0; i<5; i++ ){
		    System.out.printf("  [%d] sc:%d  lt:%d  rt:%d  fl:%d \n", i, success[i], late[i], retry[i], failure[i]);
		  }
		  System.out.printf(" in %d sec.\n", (measure_time / PRINT_INTERVAL) * PRINT_INTERVAL);

		  System.out.printf("\n<Raw Results2(sum ver.)>\n");
		  for( i=0; i<5; i++ ){
		      success2_sum[i] = 0;
		      late2_sum[i] = 0;
		      retry2_sum[i] = 0;
		      failure2_sum[i] = 0;
		      for( k=0; k<num_conn; k++ ){
			  success2_sum[i] += success2[i][k];
			  late2_sum[i] += late2[i][k];
			  retry2_sum[i] += retry2[i][k];
			  failure2_sum[i] += failure2[i][k];
		      }
		  }
		  for ( i=0; i<5; i++ ){
		      System.out.printf("  [%d] sc:%d  lt:%d  rt:%d  fl:%d \n", i, success2_sum[i], late2_sum[i], retry2_sum[i], failure2_sum[i]);
		  }

		  System.out.printf("\n<Constraint Check> (all must be [OK])\n [transaction percentage]\n");
		  for ( i=0, m=0; i<5; i++ ){
		    m += (success[i] + late[i]);
		  }

		  f = (float) (100.0 * (float)(success[1] + late[1])/(float)m);
		  System.out.printf("        Payment: %f%% (>=43.0%%)",f);
		  if ( f >= 43.0 ){
		    System.out.printf(" [OK]\n");
		  }else{
		    System.out.printf(" [NG] *\n");
		  }
		  f = (float) (100.0 * (float)(success[2] + late[2])/(float)m);
		  System.out.printf("   Order-Status: %f%% (>= 4.0%%)",f);
		  if ( f >= 4.0 ){
		    System.out.printf(" [OK]\n");
		  }else{
		    System.out.printf(" [NG] *\n");
		  }
		  f = (float) (100.0 * (float)(success[3] + late[3])/(float)m);
		  System.out.printf("       Delivery: %f%% (>= 4.0%%)",f);
		  if ( f >= 4.0 ){
		    System.out.printf(" [OK]\n");
		  }else{
		    System.out.printf(" [NG] *\n");
		  }
		  f = (float) (100.0 * (float)(success[4] + late[4])/(float)m);
		  System.out.printf("    Stock-Level: %f%% (>= 4.0%%)",f);
		  if ( f >= 4.0 ){
		    System.out.printf(" [OK]\n");
		  }else{
		    System.out.printf(" [NG] *\n");
		  }

		  System.out.printf(" [response time (at least 90%% passed)]\n");
		  f = (float) (100.0 * (float)success[0]/(float)(success[0] + late[0]));
		  System.out.printf("      New-Order: %f%% ",f);
		  if ( f >= 90.0 ){
		    System.out.printf(" [OK]\n");
		  }else{
		    System.out.printf(" [NG] *\n");
		  }
		  f = (float) (100.0 * (float)success[1]/(float)(success[1] + late[1]));
		   System.out.printf("        Payment: %f%% ",f);
		  if ( f >= 90.0 ){
		    System.out.printf(" [OK]\n");
		  }else{
		    System.out.printf(" [NG] *\n");
		  }
		  f = (float) (100.0 * (float)success[2]/(float)(success[2] + late[2]));
		  System.out.printf("   Order-Status: %f%% ",f);
		  if ( f >= 90.0 ){
		    System.out.printf(" [OK]\n");
		  }else{
		    System.out.printf(" [NG] *\n");
		  }
		  f = (float) (100.0 * (float)success[3]/(float)(success[3] + late[3]));
		  System.out.printf("       Delivery: %f%% ",f);
		  if ( f >= 90.0 ){
		    System.out.printf(" [OK]\n");
		  }else{
		    System.out.printf(" [NG] *\n");
		  }
		  f = (float) (100.0 * (float)success[4]/(float)(success[4] + late[4]));
		  	System.out.printf("    Stock-Level: %f%% ",f);
		  if ( f >= 90.0 ){
		    System.out.printf(" [OK]\n");
		  }else{
		    System.out.printf(" [NG] *\n");
		  }

		 System.out.println("\n<TpmC>\n");
		  f = (float) ((float)(success[0] + late[0]) * 60.0
		    / (float)((measure_time / PRINT_INTERVAL) * PRINT_INTERVAL));
		  System.out.printf("                 %f TpmC\n",f);
		  System.exit(0);
	}
	
	void alarm_handler(int signum)
	{
	  int i;
	  int[] s = new int[5];
	  int[] l = new int[5];
	  double[] rt90 = new double[5];

	  for( i=0; i<5; i++ ){
	    s[i] = success[i];
	    l[i] = late[i];
	    rt90[i] = RtHist.histCkp(i);
	  }
	  
	  time_count += PRINT_INTERVAL;
	  System.out.printf("%d, %d(%d):%f|%f, %d(%d):%f|%f, %d(%d):%f|%f, %d(%d):%f|%f, %d(%d):%f|%f\n",
		 time_count,
		 ( s[0] + l[0] - prev_s[0] - prev_l[0] ),
		 ( l[0] - prev_l[0] ),
		 rt90[0],(double)cur_max_rt[0],
		 ( s[1] + l[1] - prev_s[1] - prev_l[1] ),
		 ( l[1] - prev_l[1] ),
		 rt90[1],(double)cur_max_rt[1],
		 ( s[2] + l[2] - prev_s[2] - prev_l[2] ),
		 ( l[2] - prev_l[2] ),
		 rt90[2],(double)cur_max_rt[2],
		 ( s[3] + l[3] - prev_s[3] - prev_l[3] ),
		 ( l[3] - prev_l[3] ),
		 rt90[3],(double)cur_max_rt[3],
		 ( s[4] + l[4] - prev_s[4] - prev_l[4] ),
		 ( l[4] - prev_l[4] ),
		 rt90[4],(double)cur_max_rt[4]
		 );

	  for( i=0; i<5; i++ ){
	    prev_s[i] = s[i];
	    prev_l[i] = l[i];
	    cur_max_rt[i]=0.0;
	  }
	}
	
	void alarm_dummy()
	{
	  int i;
	  int[] s = new int[5];
	  int[] l = new int[5];
	  float[] rt90 = new float[5];

	  for( i=0; i<5; i++ ){
	    s[i] = success[i];
	    l[i] = late[i];
	    rt90[i] = (float) RtHist.histCkp(i);
	  }

	  time_count += PRINT_INTERVAL;
	 System.out.printf("%d, %d(%d):%f, %d(%d):%f, %d(%d):%f, %d(%d):%f, %d(%d):%f\n",
		 time_count,
		 ( s[0] + l[0] - prev_s[0] - prev_l[0] ),
		 ( l[0] - prev_l[0] ),
		 rt90[0],
		 ( s[1] + l[1] - prev_s[1] - prev_l[1] ),
		 ( l[1] - prev_l[1] ),
		 rt90[1],
		 ( s[2] + l[2] - prev_s[2] - prev_l[2] ),
		 ( l[2] - prev_l[2] ),
		 rt90[2],
		 ( s[3] + l[3] - prev_s[3] - prev_l[3] ),
		 ( l[3] - prev_l[3] ),
		 rt90[3],
		 ( s[4] + l[4] - prev_s[4] - prev_l[4] ),
		 ( l[4] - prev_l[4] ),
		 rt90[4]
		 );

	  for( i=0; i<5; i++ ){
	    prev_s[i] = s[i];
	    prev_l[i] = l[i];
	  }
	}
			
			

}

