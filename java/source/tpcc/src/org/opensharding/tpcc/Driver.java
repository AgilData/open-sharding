package org.opensharding.tpcc;

import java.io.File;
import java.io.PrintWriter;


public class Driver implements TpccConstants {
	
	//CHECK: The following variables are externs??
	public static int num_ware;
	public static int num_conn;
	public static int activate_transaction;
	public static int counting_on;

	public static int num_node;
	public static int time_count;
	public static PrintWriter freport_file;

	public static int[] success;
	public static int[] late;
	public static int[] retry;
	public static int[] failure;

	public static int[][] success2;
	public static int[][] late2;
	public static int[][] retry2;
	public static int[][] failure2;

	public static double[] max_rt;

	public static long[] clk_tck;

	//Private variables
	private static int MAX_RETRY = 2000;
	private static int RTIME_NEWORD = 5;
	private static int RTIME_PAYMENT = 5;
	private static int RTIME_ORDSTAT = 5;
	private static int RTIME_DELIVERY = 80;
	private static int RTIME_SLEV = 20;
	
	public int driver (int t_num)
	{
	    int i = 0;
	    int j = 0;

	    /* Actually, WaitTimes are needed... */
	    //CHECK: Is activate_transaction handled correctly?
	    
	    while( activate_transaction >= 1 ){
	      switch(Util.seqGet()){
	      case 0:
			doNeword(t_num);
			break;
		  case 1:
			doPayment(t_num);
			break;
		  case 2:
			doOrdstat(t_num);
			break;
		  case 3:
			doDelivery(t_num);
			break;
		  case 4:
			doSlev(t_num);
			break;
		  default:
			System.out.printf("Error - Unknown sequence.\n");
		  }

	    }

	    return(0);

	}
	
	/*
	 * prepare data and execute the new order transaction for one order
	 * officially, this is supposed to be simulated terminal I/O
	 */
	static int doNeword (int t_num)
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
	    
	   // clk1 = clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tbuf1 );
	    beginTime = System.nanoTime();
	    for (i = 0; i < MAX_RETRY; i++) {
	      ret = NewOrder.neword(t_num, w_id, d_id, c_id, ol_cnt, all_local, itemid, supware, qty);
	    //  clk2 = clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tbuf2 );
	      endTime =  System.nanoTime();

	      if(ret >= 1){
	    
	    	  //CHECK: Is this correct?
	    	  //rt = (double)(tbuf2.tv_sec * 1000.0 + tbuf2.tv_nsec/1000000.0-tbuf1.tv_sec * 1000.0 - tbuf1.tv_nsec/1000000.0);
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
	static int otherWare (int home_ware)
	{
	    int tmp;

	    if (num_ware == 1) return home_ware;
	    while ((tmp = Util.randomNumber(1, num_ware)) == home_ware);
	    return tmp;
	}

	/*
	 * prepare data and execute payment transaction
	 */
	static int doPayment (int t_num)
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

	    if(num_node==0){
		w_id = Util.randomNumber(1, num_ware);
	    }else{
		c_num = ((num_node * t_num)/num_conn); /* drop moduls */
		w_id = Util.randomNumber(1 + (num_ware * c_num)/num_node,
				    (num_ware * (c_num + 1))/num_node);
	    }
	    d_id = Util.randomNumber(1, DIST_PER_WARE);
	    c_id = Util.nuRand(1023, 1, CUST_PER_DIST); 
	    Util.lastName(Util.nuRand(255,0,999), c_last); 
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

	    //clk1 = clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tbuf1 );
	    beginTime = System.nanoTime();
	    for (i = 0; i < MAX_RETRY; i++) {
	      ret = Payment.payment(t_num, w_id, d_id, byname, c_w_id, c_d_id, c_id, c_last, h_amount);
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
	static int doOrdstat (int t_num)
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
	    Util.lastName(Util.nuRand(255,0,999), c_last); 
	    if (Util.randomNumber(1, 100) <= 60) {
	        byname = 1; /* select by last name */
	    }else{
	        byname = 0; /* select by customer id */
	    }

	      //clk1 = clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tbuf1 );
	    beginTime = System.nanoTime();
	    for (i = 0; i < MAX_RETRY; i++) {
	      ret = OrderStat.ordStat(t_num, w_id, d_id, byname, c_id, c_last);
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
	static int doDelivery (int t_num)
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
	      ret = Delivery.delivery(t_num, w_id, o_carrier_id);
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
	static int doSlev (int t_num)
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
	      ret = Slev.slev(t_num, w_id, d_id, level);
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
