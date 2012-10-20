package org.opensharding.tpcc;

import java.io.PrintWriter;
import java.sql.Connection;
import java.sql.Statement;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;


public class Driver implements TpccConstants {
	
	private static final Logger logger = LogManager.getLogger(Driver.class);
	private static final boolean DEBUG = logger.isDebugEnabled();
	
	//CHECK: The following variables are externs??
	public int activate_transaction = 1;
	public int counting_on;
	public int num_ware;
	public int num_conn;

	public int num_node;
	public int time_count;
	public PrintWriter freport_file;

	public int[] success;
	public int[] late;
	public int[] retry;
	public int[] failure;

	public int[][] success2;
	public int[][] late2;
	public int[][] retry2;
	public int[][] failure2;

	public double[] max_rt = new double[5];

	public long[] clk_tck;

	//Private variables
	private int MAX_RETRY = 2000;
	private int RTIME_NEWORD = 5;
	private int RTIME_PAYMENT = 5;
	private int RTIME_ORDSTAT = 5;
	private int RTIME_DELIVERY = 80;
	private int RTIME_SLEV = 20;
	
	private Connection conn;
	private TpccStatements pStmts;
	
	// Transaction instances.
	private NewOrder newOrder;
	private Payment payment;
	private OrderStat orderStat;
	private Slev slev;
	private Delivery delivery;
	
	/**
	 * Constructor.
	 * @param conn
	 */
	public Driver(Connection conn) {
		try {
			this.conn = conn;
			
			pStmts = new TpccStatements(conn);
			
			// Initialize the transactions.
			newOrder = new NewOrder(pStmts);
			payment = new Payment(pStmts);
			orderStat = new OrderStat(pStmts);
			slev = new Slev(pStmts);
			delivery = new Delivery(pStmts);
			
			
		} catch(Throwable th) {
			throw new RuntimeException("Error initializing Driver", th);
		}
	}
	
	public int runTransaction(int t_num, int numWare, int numConn)
	{
		
		num_ware = numWare;
		num_conn = numConn;
	    int i = 0;
	    int j = 0;

	    /* Actually, WaitTimes are needed... */
	    //CHECK: Is activate_transaction handled correctly?
	    int sequence = Util.seqGet();
	    while( activate_transaction >= 1 ){
	    	if(DEBUG) logger.debug("runTransaction: sequence: " + sequence);
	    	
	    	if(sequence == 0){
	    		doNeword(t_num, conn, pStmts);
	    	}else if( sequence == 1){
	    		doPayment(t_num, conn, pStmts);
	    	}else if(sequence == 2){
	    		doOrdstat(t_num, conn, pStmts);
	    	}else if(sequence == 3){
	    		doDelivery(t_num, conn, pStmts);
	    	}else if(sequence == 4){
	    		doSlev(t_num, conn, pStmts);
	    	}else{
	    		System.out.printf("Error - Unknown sequence: %d.\n", Util.seqGet());
				System.exit(1);
	    	}
	    	sequence = Util.seqGet();
//	      switch(Util.seqGet()){
//	      case 0:
//			doNeword(t_num, conn, pStmts);
//			break;
//		  case 1:
//			  System.out.println("Executing do payment");
//			doPayment(t_num, conn, pStmts);
//			break;
//		  case 2:
//			  System.out.println("Executing do ord stat");
//			doOrdstat(t_num, conn, pStmts);
//			break;
//		  case 3:
//			  System.out.println("Executing do delivery");
//			doDelivery(t_num, conn, pStmts);
//			break;
//		  case 4:
//			  System.out.println("Executing do slev");
//			doSlev(t_num, conn, pStmts);
//			break;
//		  default:
//			System.out.printf("Error - Unknown sequence: %d.\n", Util.seqGet());
//			System.exit(1);
		  }

	    

	    return(0);

	}
	
	/*
	 * prepare data and execute the new order transaction for one order
	 * officially, this is supposed to be simulated terminal I/O
	 */
	private int doNeword (int t_num, Connection conn, TpccStatements pStmts)
	{
	    int c_num = 0;
	    int i = 0;
	    int ret = 0;
	    double rt = 0.0;
	    //CHECK: What needs to be done with these variables
	   // clock_t clk1,clk2;
	   // struct timespec tbuf1;
	   // struct timespec tbuf2;
	    long beginTime = 0;
	    long endTime = 0;
	    
	    int  w_id = 0;
	    int d_id = 0;
	    int c_id = 0;
	    int ol_cnt = 0;
	    int  all_local = 1;
	    int  notfound = MAXITEMS+1;  /* valid item ids are numbered consecutively
					    [1..MAXITEMS] */
	    int rbk = 0;
	    int[] itemid = new int[MAX_NUM_ITEMS];
	    int[] supware = new int[MAX_NUM_ITEMS];
	    int[] qty = new int[MAX_NUM_ITEMS];

	    if(num_node==0){
		w_id = Util.randomNumber(1, num_ware);
	    }else{
		c_num = ((num_node * t_num)/num_conn); /* drop moduls */
		w_id =Util.randomNumber(1 + (num_ware * c_num)/num_node,
				    (num_ware * (c_num + 1))/num_node);
	    }
	    
	    
	    d_id = Util.randomNumber(1, DIST_PER_WARE);
	    c_id = Util.nuRand(1023, 1, CUST_PER_DIST);

	    ol_cnt = Util.randomNumber(5, 15);
	    rbk = Util.randomNumber(1, 100);

	    for (i = 0; i < ol_cnt; i++) {
		itemid[i] = Util.nuRand(8191, 1, MAXITEMS);
		if ((i == ol_cnt - 1) && (rbk == 1)) {
		    itemid[i] = notfound;
		}
		if (Util.randomNumber(1, 100) != 1) {
		    supware[i] = w_id;
		}
		else {
		    supware[i] = otherWare(w_id);
		    all_local = 0;
		}
		qty[i] = Util.randomNumber(1, 10);
	    }
	    
	    beginTime = System.nanoTime();
	    for (i = 0; i < MAX_RETRY; i++) {
//	    	System.out.printf("t_num: %d w_id: %d, c_id: %d, ol_cnt: %d, all_local: %d  qty: %d\n", t_num, w_id, d_id, c_id, ol_cnt, all_local,  qty);
	      ret = newOrder.neword(t_num, w_id, d_id, c_id, ol_cnt, all_local, itemid, supware, qty, conn);
	      endTime =  System.nanoTime();

	      if(ret >= 1){
	    	  rt = (double)(endTime - beginTime);
	    	  //printf("NOT : %.3f\n", rt);
	        if (freport_file != null) {
	        	freport_file.printf("%d %.3f\n", time_count, rt);
	        }

		if(rt > max_rt[0])
		  max_rt[0]=rt;
		RtHist.histInc(0, rt);
		if(counting_on >= 1){
		  if( rt < RTIME_NEWORD ){
		    success[0]++;
		    success2[0][t_num]++;
		  }else{
		    late[0]++;
		    late2[0][t_num]++;
		  }
		}

		return (1); /* end */
	      }else{

			if(counting_on >= 1){
			  retry[0]++;
			  retry2[0][t_num]++;
			}

	      }
	    }

	    if(counting_on >= 1){
	      retry[0]--;
	      retry2[0][t_num]--;
	      failure[0]++;
	      failure2[0][t_num]++;
	    }

	    return (0);
	}
	
	/*
	 * produce the id of a valid warehouse other than home_ware
	 * (assuming there is one)
	 */
	private int otherWare (int home_ware)
	{
	    int tmp;

	    if (num_ware == 1) return home_ware;
	    while ((tmp = Util.randomNumber(1, num_ware)) == home_ware);
	    return tmp;
	}

	/*
	 * prepare data and execute payment transaction
	 */
	private int doPayment (int t_num, Connection conn, TpccStatements pStmts)
	{
	    int c_num = 0;
	    int byname = 0;
	    int i = 0;
	    int ret = 0;
	    double rt = 0.0;

	    //clock_t clk1,clk2;
	    //struct timespec tbuf1;
	    //struct timespec tbuf2;
	    
	    long beginTime = 0;
	    long endTime = 0;
	    int  w_id = 0;
	    int d_id = 0;
	    int c_w_id = 0;
	    int c_d_id = 0;
	    int c_id = 0;
	    int h_amount = 0;
	    String c_last = null;
	    
//	    System.out.println("DO PAYMENT TRANSACTIONS");
	    if(num_node==0){
		w_id = Util.randomNumber(1, num_ware);
	    }else{
		c_num = ((num_node * t_num)/num_conn); /* drop moduls */
		w_id = Util.randomNumber(1 + (num_ware * c_num)/num_node,
				    (num_ware * (c_num + 1))/num_node);
	    }
	    d_id = Util.randomNumber(1, DIST_PER_WARE);
	    c_id = Util.nuRand(1023, 1, CUST_PER_DIST);
	    c_last = Util.lastName(Util.nuRand(255,0,999)); 
	    h_amount = Util.randomNumber(1,5000);
	    if (Util.randomNumber(1, 100) <= 60) {
	        byname = 1; /* select by last name */
	    }else{
	        byname = 0; /* select by customer id */
	    }
	    if (Util.randomNumber(1, 100) <= 85) {
	        c_w_id = w_id;
	        c_d_id = d_id;
	    }else{
	        c_w_id = otherWare(w_id);
	        c_d_id = Util.randomNumber(1, DIST_PER_WARE);
	    }
//	    System.out.printf("Payment| cnum: %d  w_id: %d d_id: %d c_id: %d c_last: %s h_amount: %d\n byname: %d c_w_id: %d c_d_id: %d", 
//	    		c_num, w_id, d_id, c_id, c_last, h_amount, byname, c_w_id, c_d_id );

	    //clk1 = clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tbuf1 );
	    beginTime = System.nanoTime();
	    for (i = 0; i < MAX_RETRY; i++) {
	      ret = payment.payment(t_num, w_id, d_id, byname, c_w_id, c_d_id, c_id, c_last, h_amount, conn);
	     // clk2 = clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tbuf2 );
	      endTime = System.nanoTime();

	      if(ret >= 1){

			//rt = (double)(tbuf2.tv_sec * 1000.0 + tbuf2.tv_nsec/1000000.0-tbuf1.tv_sec * 1000.0 - tbuf1.tv_nsec/1000000.0);
			rt = (double)(endTime - beginTime);
	    	  if(rt > max_rt[1])
	    		max_rt[1]=rt;
	    	RtHist.histInc(1, rt);
			if(counting_on >= 1){
			  if( rt < RTIME_PAYMENT ){
			    success[1]++;
			    success2[1][t_num]++;
			  }else{
			    late[1]++;
			    late2[1][t_num]++;
			  }
			}

			return (1); /* end */
	      }else{

			if(counting_on >= 1){
			  retry[1]++;
			  retry2[1][t_num]++;
			}

	      }
	    }

	    if(counting_on >= 1){
	      retry[1]--;
	      retry2[1][t_num]--;
	      failure[1]++;
	      failure2[1][t_num]++;
	    }

	    return (0);
	}
	
	/*
	 * prepare data and execute order status transaction
	 */
	private int doOrdstat (int t_num, Connection conn, TpccStatements pStmts)
	{
	    int c_num = 0;
	    int byname = 0;
	    int i = 0;
	    int ret = 0;
	    double rt = 0.0;
	    /*clock_t clk1,clk2;
	    struct timespec tbuf1;
	    struct timespec tbuf2;*/
	    long beginTime = 0;
	    long endTime = 0;
	    
	    int  w_id = 0;
	    int d_id = 0; 
	    int c_id = 0;
	    String c_last = null;

	    if(num_node==0){
		w_id = Util.randomNumber(1, num_ware);
	    }else{
		c_num = ((num_node * t_num)/num_conn); /* drop moduls */
		w_id = Util.randomNumber(1 + (num_ware * c_num)/num_node,
				    (num_ware * (c_num + 1))/num_node);
	    }
	    d_id = Util.randomNumber(1, DIST_PER_WARE);
	    c_id = Util.nuRand(1023, 1, CUST_PER_DIST); 
	    c_last = Util.lastName(Util.nuRand(255,0,999)); 
	    if (Util.randomNumber(1, 100) <= 60) {
	        byname = 1; /* select by last name */
	    }else{
	        byname = 0; /* select by customer id */
	    }

	      //clk1 = clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tbuf1 );
	    beginTime = System.nanoTime();
	    for (i = 0; i < MAX_RETRY; i++) {
	      ret = orderStat.ordStat(t_num, w_id, d_id, byname, c_id, c_last, conn);
	     // clk2 = clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tbuf2 );
	      endTime = System.nanoTime();

	      if(ret >= 1){

		//rt = (double)(tbuf2.tv_sec * 1000.0 + tbuf2.tv_nsec/1000000.0-tbuf1.tv_sec * 1000.0 - tbuf1.tv_nsec/1000000.0)
	    	rt = (double)(endTime - beginTime);  
	    	  if(rt > max_rt[2])
	    		  max_rt[2]=rt;
			RtHist.histInc(2, rt);
			if(counting_on >= 1){
			  if( rt < RTIME_ORDSTAT ){
			    success[2]++;
			    success2[2][t_num]++;
			  }else{
			    late[2]++;
			    late2[2][t_num]++;
			  }
			}
	
			return (1); /* end */
	      }else{

			if(counting_on >= 1){
			  retry[2]++;
			  retry2[2][t_num]++;
			}

	      }
	    }

	    if(counting_on >= 1){
	      retry[2]--;
	      retry2[2][t_num]--;
	      failure[2]++;
	      failure2[2][t_num]++;
	    }

	    return (0);

	}
	
	/*
	 * execute delivery transaction
	 */
	private int doDelivery (int t_num, Connection conn, TpccStatements pStmts)
	{
	    int c_num = 0;
	    int i = 0;
	    int ret = 0;
	    double rt = 0.0;
	  /*clock_t clk1,clk2;
	    struct timespec tbuf1;
	    struct timespec tbuf2;*/
	    long beginTime = 0;
	    long endTime = 0;
	    int  w_id = 0;
	    int o_carrier_id = 0;

	    if(num_node==0){
	    	w_id = Util.randomNumber(1, num_ware);
	    }else{
		c_num = ((num_node * t_num)/num_conn); /* drop moduls */
		w_id = Util.randomNumber(1 + (num_ware * c_num)/num_node,
				    (num_ware * (c_num + 1))/num_node);
	    }
	    o_carrier_id = Util.randomNumber(1, 10);

	      //clk1 = clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tbuf1 );
	    beginTime = System.nanoTime();
	    for (i = 0; i < MAX_RETRY; i++) {
	      ret = delivery.delivery(t_num, w_id, o_carrier_id, conn);
	     // clk2 = clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tbuf2 );
	      endTime = System.nanoTime();
	      if(ret >=1 ){

	    	  //rt = (double)(tbuf2.tv_sec * 1000.0 + tbuf2.tv_nsec/1000000.0-tbuf1.tv_sec * 1000.0 - tbuf1.tv_nsec/1000000.0);
			rt = (double)(endTime - beginTime);
	    	if(rt > max_rt[3])
			  max_rt[3]=rt;
			RtHist.histInc(3, rt );
			if(counting_on >= 1){
			  if( rt < RTIME_DELIVERY ){
			    success[3]++;
			    success2[3][t_num]++;
			  }else{
			    late[3]++;
			    late2[3][t_num]++;
			  }
			}
	
			return (1); /* end */
	      }else{

			if(counting_on >= 1){
			  retry[3]++;
			  retry2[3][t_num]++;
			}

	      }
	    }

	    if(counting_on >= 1){
	      retry[3]--;
	      retry2[3][t_num]--;
	      failure[3]++;
	      failure2[3][t_num]++;
	    }

	    return (0);

	}
	
	/* 
	 * prepare data and execute the stock level transaction
	 */
	private int doSlev (int t_num, Connection conn, TpccStatements pStmts)
	{
	    int c_num = 0;
	    int i = 0;
	    int ret = 0;
	    double rt = 0.0;
	    long beginTime = 0;
	    long endTime = 0;
	    int  w_id = 0;
	    int d_id = 0;
	    int level = 0;

	    if(num_node==0){
	    	w_id = Util.randomNumber(1, num_ware);
	    }else{
	    	c_num = ((num_node * t_num)/num_conn); /* drop moduls */
			w_id = Util.randomNumber(1 + (num_ware * c_num)/num_node,
				    (num_ware * (c_num + 1))/num_node);
	    }
	    d_id = Util.randomNumber(1, DIST_PER_WARE); 
	    level = Util.randomNumber(10, 20); 

	     // clk1 = clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tbuf1 );
	    beginTime = System.nanoTime();
	    for (i = 0; i < MAX_RETRY; i++) {
	      ret = slev.slev(t_num, w_id, d_id, level, conn);
	      //clk2 = clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tbuf2 );
	      endTime = System.nanoTime();

	      if(ret >= 1){

	    	  //rt = (double)(tbuf2.tv_sec * 1000.0 + tbuf2.tv_nsec/1000000.0-tbuf1.tv_sec * 1000.0 - tbuf1.tv_nsec/1000000.0);
			rt = (double)(endTime - beginTime);
	    	if(rt > max_rt[4])
			  max_rt[4]=rt;
			RtHist.histInc(4, rt );
			if(counting_on >= 1){
			  if( rt < RTIME_SLEV ){
			    success[4]++;
			    success2[4][t_num]++;
			  }else{
			    late[4]++;
			    late2[4][t_num]++;
			  }
			}

			return (1); /* end */
	      }else{

			if(counting_on >= 1){
			  retry[4]++;
			  retry2[4][t_num]++;
			}

	      }
	    }

	    if(counting_on >= 1){
	      retry[4]--;
	      retry2[4][t_num]--;
	      failure[4]++;
	      failure2[4][t_num]++;
	    }

	    return (0);

	}


}
