package org.opensharding.tpcc;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Timestamp;
import java.util.Calendar;
import java.util.Date;

public class NewOrder implements TpccConstants {
	
	private TpccStatements pStmts;
	
	private String s_dist_01 = null;
	private String s_dist_02 = null;
	private String s_dist_03 = null;
	private String s_dist_04 = null;
	private String s_dist_05 = null;
	private String s_dist_06 = null;
	private String s_dist_07 = null;
	private String s_dist_08 = null;
	private String s_dist_09 = null;
	private String s_dist_10 = null;
	
	
	/**
	 * Constructor.
	 * @param pStmts
	 */
	public NewOrder(TpccStatements pStmts) {
		this.pStmts = pStmts;
	}
	
	private String pickDistInfo(String ol_dist_info, int ol_supply_w_id){
		switch(ol_supply_w_id){
		case 1:
			ol_dist_info = s_dist_01;
			break;
		case 2:
			ol_dist_info = s_dist_02;
			break;
		case 3:
			ol_dist_info = s_dist_03;
			break;
		case 4:
			ol_dist_info = s_dist_04;
			break;
		case 5:
			ol_dist_info = s_dist_05;
			break;
		case 6:
			ol_dist_info = s_dist_06;
			break;
		case 7:
			ol_dist_info = s_dist_07;
			break;
		case 8:
			ol_dist_info = s_dist_08;
			break;
		case 9:
			ol_dist_info = s_dist_09;
			break;
		case 10:
			ol_dist_info = s_dist_10;
			break;
		}
		
		return ol_dist_info;
	}
	
	public int neword( int t_num,
		    int w_id_arg,		/* warehouse id */
		    int d_id_arg,		/* district id */
		    int c_id_arg,		/* customer id */
		    int o_ol_cnt_arg,	        /* number of items */
		    int o_all_local_arg,	/* are all order lines local */
		    int itemid[],		/* ids of items to be ordered */
		    int supware[],		/* warehouses supplying items */
		    int qty[],
		    Connection conn
	)
	{

		try {
			
			// Start a transaction.
			pStmts.getConnection().setAutoCommit(false);
			
			int w_id = w_id_arg;
			int d_id = d_id_arg;
			int c_id = c_id_arg;
			int o_ol_cnt = o_ol_cnt_arg;
			int o_all_local = o_all_local_arg;
			float c_discount = 0;
			String c_last = null;
			String c_credit = null;
			float w_tax = 0;
			int d_next_o_id = 0;
			float d_tax = 0;
			int o_id = 0;
			String i_name = null;
			float i_price = 0;
			String i_data = null;
			int ol_i_id = 0;
			int s_quantity = 0;
			String s_data = null;
			
			String ol_dist_info = null;
			int ol_supply_w_id = 0;
			float ol_amount = 0;
			int ol_number = 0;
			int ol_quantity = 0;

			String[] iname = new String[MAX_NUM_ITEMS];
			String[] bg = new String[MAX_NUM_ITEMS];
			float[] amt = new float[MAX_NUM_ITEMS];
			float[] price = new float[MAX_NUM_ITEMS];
			int[] stock = new int[MAX_NUM_ITEMS];
			float total = 0;

			int min_num = 0;
			int i = 0, j = 0, tmp = 0, swp = 0;
			int[] ol_num_seq = new int[MAX_NUM_ITEMS];

			int proceed = 0;
			//struct timespec tbuf1,tbuf_start;
			//clock_t clk1,clk_start;
			long beginTime = 0;
			long endTime = 0;
			
			//Timestamp 
			Calendar calendar = Calendar.getInstance();
			Date now = calendar.getTime();
			Timestamp currentTimeStamp = new Timestamp(now.getTime());
			
			proceed = 1;
			
			//Get prepared statement
			//"SELECT c_discount, c_last, c_credit, w_tax FROM customer, warehouse WHERE w_id = ? AND c_w_id = w_id AND c_d_id = ? AND c_id = ?"
			try {
				pStmts.getStatement(0).setInt(1, w_id);
				pStmts.getStatement(0).setInt(2, w_id);
				pStmts.getStatement(0).setInt(3, d_id);
				pStmts.getStatement(0).setInt(4, c_id);
				System.out.println("SELECT c_discount, c_last, c_credit, w_tax FROM customer, warehouse WHERE w_id = " + w_id + " AND c_w_id = " + w_id + " AND c_d_id = " + d_id + " AND c_id = " + c_id);
				ResultSet rs = pStmts.getStatement(0).executeQuery();
				if(rs.next()){
					c_discount = rs.getFloat(1);
					c_last = rs.getString(2);
					c_credit = rs.getString(3);
					w_tax = rs.getFloat(4);
				}
				System.out.printf("c_discount: %f, c_last: %s, c_credit: %s, w_tax: %f\n\n", c_discount, c_last, c_credit, w_tax);
				rs.close();
			} catch (SQLException e) {
				throw new RuntimeException("NewOrder select transaction error", e);
			}
			
			proceed = 2;
			//Get prepared statement
			//"SELECT d_next_o_id, d_tax FROM district WHERE d_id = ? AND d_w_id = ? FOR UPDATE"
			
			try {
				pStmts.getStatement(1).setInt(1, d_id);
				pStmts.getStatement(1).setInt(2, w_id);
				System.out.printf("SELECT d_next_o_id, d_tax FROM district WHERE d_id = %d AND d_w_id = %d FOR UPDATE\n", d_id, w_id);
				ResultSet rs = pStmts.getStatement(1).executeQuery();
				if(rs.next()){
					d_next_o_id = rs.getInt(1);
					d_tax = rs.getFloat(2);
					System.out.printf("d_next: %d , d_tax %f \n\n", d_next_o_id, d_tax);
				}
				rs.close();
				
			} catch (SQLException e) {
				throw new RuntimeException("Neworder select transaction error", e);
			}
			
			proceed = 3;
			//Get prepared statement
			//"UPDATE district SET d_next_o_id = ? + 1 WHERE d_id = ? AND d_w_id = ?"
			
			try {
				pStmts.getStatement(2).setInt(1, d_next_o_id);
				pStmts.getStatement(2).setInt(2, d_id);
				pStmts.getStatement(2).setInt(3, w_id);
				pStmts.getStatement(2).executeUpdate();
			} catch (SQLException e) {
				throw new RuntimeException("NewOrder update transaction error", e);
			}
			
			o_id = d_next_o_id;
			
			proceed = 4;
			//Get prepared statement
			//"INSERT INTO orders (o_id, o_d_id, o_w_id, o_c_id, o_entry_d, o_ol_cnt, o_all_local) VALUES(?, ?, ?, ?, ?, ?, ?)"
			
			try {
				pStmts.getStatement(3).setInt(1, o_id);
				pStmts.getStatement(3).setInt(2, d_id);
				pStmts.getStatement(3).setInt(3, w_id);
				pStmts.getStatement(3).setInt(4, c_id);
				pStmts.getStatement(3).setString(5, currentTimeStamp.toString());
				pStmts.getStatement(3).setInt(6, o_ol_cnt);
				pStmts.getStatement(3).setInt(7, o_all_local);
				
				pStmts.getStatement(3).executeUpdate();
			} catch (SQLException e) {
				throw new RuntimeException("NewOrder insert transaction error",e );
			}
			
			proceed = 5;
			//Get prepared statement
			//"INSERT INTO new_orders (no_o_id, no_d_id, no_w_id) VALUES (?,?,?)
			try {
				pStmts.getStatement(4).setInt(1, o_id);
				pStmts.getStatement(4).setInt(2, d_id);
				pStmts.getStatement(4).setInt(3, w_id);
				
				pStmts.getStatement(4).executeUpdate();
			} catch (SQLException e) {
				throw new RuntimeException("NewOrder insert transaction error", e);
			}
			
			/* sort orders to avoid DeadLock */
			for (i = 0; i < o_ol_cnt; i++) {
				ol_num_seq[i]=i;
			}
			
			for (i = 0; i < (o_ol_cnt - 1); i++) {
				tmp = (MAXITEMS + 1) * supware[ol_num_seq[i]] + itemid[ol_num_seq[i]];
				min_num = i;
				for ( j = i+1; j < o_ol_cnt; j++) {
				  if ( (MAXITEMS + 1) * supware[ol_num_seq[j]] + itemid[ol_num_seq[j]] < tmp ){
				    tmp = (MAXITEMS + 1) * supware[ol_num_seq[j]] + itemid[ol_num_seq[j]];
				    min_num = j;
				  }
				}
				if ( min_num != i ){
				  swp = ol_num_seq[min_num];
				  ol_num_seq[min_num] = ol_num_seq[i];
				  ol_num_seq[i] = swp;
				}
			}
			
			for (ol_number = 1; ol_number <= o_ol_cnt; ol_number++) {
				ol_supply_w_id = supware[ol_num_seq[ol_number - 1]];
				ol_i_id = itemid[ol_num_seq[ol_number - 1]];
				ol_quantity = qty[ol_num_seq[ol_number - 1]];

				/* EXEC SQL WHENEVER NOT FOUND GOTO invaliditem; */
				proceed = 6;
				//Get prepared statement
				//"SELECT i_price, i_name, i_data FROM item WHERE i_id = ?"
				
				try {
					pStmts.getStatement(5).setInt(1, ol_i_id);
					System.out.println("SELECT i_price, i_name, i_data FROM item WHERE i_id =" + ol_i_id);
					ResultSet rs = pStmts.getStatement(5).executeQuery();
					if(rs.next()){
						i_price = rs.getFloat(1);
						i_name = rs.getString(2);
						i_data = rs.getString(3);
						System.out.println("i_price: " + i_price + " i_name " + i_name + " i_data " + i_data + "\n");
					} else {
						rs.close();
						throw new AbortedTransactionException();
					}
					rs.close();
				} catch (SQLException e) {
					throw new RuntimeException("NewOrder select transaction error", e);
				}
				
				price[ol_num_seq[ol_number - 1]] = i_price;
				iname[ol_num_seq[ol_number -1]] = i_name;
				
				proceed = 7;
				//Get prepared statement
				//"SELECT s_quantity, s_data, s_dist_01, s_dist_02, s_dist_03, s_dist_04, s_dist_05, s_dist_06, s_dist_07, s_dist_08, s_dist_09, s_dist_10 FROM stock WHERE s_i_id = ? AND s_w_id = ? FOR UPDATE"
				try {
					pStmts.getStatement(6).setInt(1, ol_i_id);
					pStmts.getStatement(6).setInt(2, ol_supply_w_id);
					System.out.println(pStmts.toString());
					ResultSet rs = pStmts.getStatement(6).executeQuery();
					if(rs.next()){
						s_quantity = rs.getInt(1);
						s_data = rs.getString(2);
						s_dist_01 = rs.getString(3);
						s_dist_02 = rs.getString(4);
						s_dist_03 = rs.getString(5);
						s_dist_04 = rs.getString(6);
						s_dist_05 = rs.getString(7);
						s_dist_06 = rs.getString(8);
						s_dist_07 = rs.getString(9);
						s_dist_08 = rs.getString(10);
						s_dist_09 = rs.getString(11);
						s_dist_10 = rs.getString(12);
						System.out.printf("s_quantity: %d s_data: %s dist_1: %s dist_2: %s dist_3: %s dist4: %s dist_5: %s dist_6: %s dist_7 %s dist_8 %s dist_9: %s dist10: %s \n\n", s_quantity, s_data, s_dist_01,
								s_dist_02, s_dist_03, s_dist_04, s_dist_05, s_dist_06, s_dist_07, s_dist_08, s_dist_09, s_dist_10);
					}

					rs.close();
				} catch (SQLException e) {
					throw new RuntimeException("NewOrder select transaction error", e);
				}
			
				ol_dist_info = pickDistInfo(ol_dist_info, d_id);	/* pick correct * s_dist_xx */

				stock[ol_num_seq[ol_number - 1]] = s_quantity;
				
				if ( (i_data.toString().contains("original")) &&  ( s_data.toString().contains("original")) ){
					bg[ol_num_seq[ol_number - 1]] = "B";

				} else {
					bg[ol_num_seq[ol_number - 1]] = "G";

				}
				
				if (s_quantity > ol_quantity){
					s_quantity = s_quantity - ol_quantity;
				} else {
					s_quantity = s_quantity - ol_quantity + 91;
				}
				
				proceed = 8;
				//Get the prepared statement
				//"UPDATE stock SET s_quantity = ? WHERE s_i_id = ? AND s_w_id = ?"
				try {
					pStmts.getStatement(7).setInt(1, s_quantity);
					pStmts.getStatement(7).setInt(2, ol_i_id);
					pStmts.getStatement(7).setInt(3, ol_supply_w_id);
					
					pStmts.getStatement(7).executeUpdate();
				} catch (SQLException e) {
					throw new RuntimeException("NewOrder update transaction error", e);
				}

				ol_amount = ol_quantity * i_price * (1 + w_tax + d_tax) * (1 - c_discount);
				amt[ol_num_seq[ol_number - 1]] = ol_amount;
				total += ol_amount;
				
				proceed = 9;
				//Get prepared statement
				//"INSERT INTO order_line (ol_o_id, ol_d_id, ol_w_id, ol_number, ol_i_id, ol_supply_w_id, ol_quantity, ol_amount, ol_dist_info) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"
				
				try {
					pStmts.getStatement(8).setInt(1, o_id);
					pStmts.getStatement(8).setInt(2, d_id);
					pStmts.getStatement(8).setInt(3, w_id);
					pStmts.getStatement(8).setInt(4, ol_number);
					pStmts.getStatement(8).setInt(5, ol_i_id);
					pStmts.getStatement(8).setInt(6, ol_supply_w_id);
					pStmts.getStatement(8).setInt(7, ol_quantity);
					pStmts.getStatement(8).setFloat(8, ol_amount);
					pStmts.getStatement(8).setString(9, ol_dist_info.toString());
					
					pStmts.getStatement(8).executeUpdate();
				} catch (SQLException e) {
					throw new RuntimeException("NewOrder insert transaction error", e);
				}
			
			}
			
			/* Needs to be finished. Refers to line 108 and 109
			clk1 = clock_gettime(CLOCK_REALTIME, &tbuf1 );
			if (ftrx_file) {
				fprintf(ftrx_file,"t_num: %d finish: %lu %lu start: %lu %lu\n",t_num, tbuf1.tv_sec, tbuf1.tv_nsec,
					tbuf_start.tv_sec, tbuf_start.tv_nsec);
			} */
		} catch (AbortedTransactionException ate) {
			// Rollback if an aborted transaction, they are intentional in some percentage of cases.
			pStmts.rollback();
			
		} catch (Exception e) {
			try {
				// Rollback if an aborted transaction, they are intentional in some percentage of cases.
				pStmts.getConnection().rollback();
			} catch(Throwable th) {
				throw new RuntimeException("New Order error", th);
			}
		}
		
		// Commit.
		pStmts.commit();
		
		System.out.println("Finished new order");
		
		return 1;
	}
}
