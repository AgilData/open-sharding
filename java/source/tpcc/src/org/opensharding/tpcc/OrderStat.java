package org.opensharding.tpcc;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;

public class OrderStat implements TpccConstants{
	
	private  TpccStatements pStmts;
	
	public OrderStat(TpccStatements pStmts) {
		this.pStmts = pStmts;
	}

	public int ordStat( int t_num,
		     int w_id_arg,		/* warehouse id */
		     int d_id_arg,		/* district id */
		     int byname,		/* select by c_id or c_last? */
		     int c_id_arg,		/* customer id */
		     String c_last_arg,  /* customer last name, format? */
		     Connection conn
	)
	{
		
		try {
			
			pStmts.getConnection().setAutoCommit(false);
			System.out.println("================================================ORDER STAT==============================================");
			int w_id = w_id_arg;
			int d_id = d_id_arg;
			int c_id = c_id_arg;
			int c_d_id = d_id;
			int c_w_id = w_id;
			String c_first = null;
			String c_middle = null;
			String c_last = null;
			float c_balance = 0;
			int o_id = 0;
			String o_entry_d = null;
			int o_carrier_id = 0;
			int ol_i_id = 0;
			int ol_supply_w_id = 0;
			int ol_quantity = 0;
			float ol_amount = 0;
			String ol_delivery_d = null;
			int namecnt = 0;

			int  n = 0;
			int proceed = 0;
			
			if (byname > 0) { 
				
				c_last = c_last_arg;
				proceed = 1;
				
				//Get Prepared Statement
				//"SELECT count(c_id) FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_last = ?"
				try {
					pStmts.getStatement(20).setInt(1, c_w_id);
					pStmts.getStatement(20).setInt(2, c_d_id);
					pStmts.getStatement(20).setString(3, c_last);
//					System.out.printf("SELECT count(c_id) FROM customer WHERE c_w_id = %d AND c_d_id = %d AND c_last = %s\n", c_w_id, c_d_id, c_last);
					
					ResultSet rs = pStmts.getStatement(20).executeQuery();
					if(rs.next()){
						namecnt = rs.getInt(1);
//						System.out.printf("Namecnt: %d\n", namecnt);
					}
					
					rs.close();
				} catch (SQLException e) {
					throw new RuntimeException("OrderStat Select transaction error", e);
				}
				
				proceed = 2;
				
				//Get the prepared statement
				//"SELECT c_balance, c_first, c_middle, c_last FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_last = ? ORDER BY c_first"
				
				try {
					pStmts.getStatement(21).setInt(1, c_w_id);
					pStmts.getStatement(21).setInt(2, c_d_id);
					pStmts.getStatement(21).setString(3, c_last);
//					System.out.printf("SELECT c_balance, c_first, c_middle, c_last FROM customer WHERE c_w_id = %d AND c_d_id = %d AND c_last = %s ORDER BY c_first", c_w_id, c_d_id, c_last);
					
					ResultSet rs = pStmts.getStatement(21).executeQuery();


					if (namecnt % 2 == 1){ //?? Check
						namecnt++;	
					} /* Locate midpoint customer; */

					// Use a for loop to find midpoint customer based on namecnt.
					for (n = 0; n < namecnt / 2; n++) {
						rs.next();
						c_balance = rs.getFloat(1);
						c_first = rs.getString(2);
						c_middle = rs.getString(3);
						c_last = rs.getString(4);
//						System.out.printf("balance: %f first: %s, middle: %s last: %s\n", c_balance, c_first, c_middle, c_last);
					}
					
					rs.close();
				} catch (SQLException e) {
					throw new RuntimeException("OrderStat Select transaction error", e);
				}
				
			} else {		/* by number */
				proceed = 6;
				
				//Get Transaction Number 
				//"SELECT c_balance, c_first, c_middle, c_last FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_id = ?"
				try{
					pStmts.getStatement(22).setInt(1, c_w_id);
					pStmts.getStatement(22).setInt(2, c_d_id);
					pStmts.getStatement(22).setInt(3, c_id);
//					System.out.printf("SELECT c_balance, c_first, c_middle, c_last FROM customer WHERE c_w_id = %d AND c_d_id = %d AND c_id = %d\n", c_w_id, c_d_id, c_id);
					ResultSet rs = pStmts.getStatement(22).executeQuery();
					if(rs.next()){
						c_balance = rs.getFloat(1);
						c_first = rs.getString(2);
						c_middle = rs.getString(3);
						c_last = rs.getString(4);
//						System.out.printf("balance: %f, first: %s, middle: %s last: %s\n", c_balance, c_first, c_middle, c_last);
						
					}
				
					rs.close();
				} catch (SQLException e){
					throw new RuntimeException("OrderStat select transaction error", e);
				}
				
			}

			/* find the most recent order for this customer */

			proceed = 7;
			//Get prepared statement
			//"SELECT o_id, o_entry_d, COALESCE(o_carrier_id,0) FROM orders WHERE o_w_id = ? AND o_d_id = ? AND o_c_id = ? AND o_id = (SELECT MAX(o_id) FROM orders WHERE o_w_id = ? AND o_d_id = ? AND o_c_id = ?)"
			try{
				pStmts.getStatement(23).setInt(1, c_w_id);
				pStmts.getStatement(23).setInt(2, c_d_id);
				pStmts.getStatement(23).setInt(3, c_id);
				pStmts.getStatement(23).setInt(4, c_w_id);
				pStmts.getStatement(23).setInt(5, c_d_id);
				pStmts.getStatement(23).setInt(6, c_id);
//				System.out.printf("SELECT o_id, o_entry_d, COALESCE(o_carrier_id,0) FROM orders " +
//						"WHERE o_w_id = %d AND o_d_id = %d AND o_c_id = %d AND o_id = " +
//						"(SELECT MAX(o_id) FROM orders WHERE o_w_id = %d AND o_d_id = %d AND o_c_id = ?)\n",
//						c_w_id, c_d_id, c_id, c_w_id, c_d_id, c_id);
				ResultSet rs = pStmts.getStatement(23).executeQuery();
				if (rs.next()){
					o_id = rs.getInt(1);
					o_entry_d = rs.getString(2);
					o_carrier_id = rs.getInt(3);
//					System.out.printf("O_id: %d, entry: %s, carrier: %d\n", o_id, o_entry_d, o_carrier_id);
				}
				
				rs.close();
			} catch (SQLException e){
				throw new RuntimeException("OrderState select transaction error", e);
			}

			proceed = 8;
			
			//Get prepared statement 
			//"SELECT ol_i_id, ol_supply_w_id, ol_quantity, ol_amount, ol_delivery_d FROM order_line WHERE ol_w_id = ? AND ol_d_id = ? AND ol_o_id = ?"
			try{
				pStmts.getStatement(24).setInt(1, c_w_id);
				pStmts.getStatement(24).setInt(2, c_d_id);
				pStmts.getStatement(24).setInt(3, o_id);
//				System.out.printf("SELECT ol_i_id, ol_supply_w_id, ol_quantity, ol_amount, ol_delivery_d FROM order_line " +
//						"WHERE ol_w_id = %d AND ol_d_id = %d AND ol_o_id = %d\n", c_w_id, c_d_id, o_id);
				ResultSet rs = pStmts.getStatement(24).executeQuery();
				while(rs.next()){
					ol_i_id = rs.getInt(1);
					ol_supply_w_id = rs.getInt(2);
					ol_quantity = rs.getInt(3);
					ol_amount = rs.getFloat(4);
					ol_delivery_d = rs.getString(5);
//					System.out.printf("ol_I_id: %d supply: %d quantity: %d amout: %f delivery %s\n", ol_i_id, ol_supply_w_id, ol_quantity,
//							ol_amount, ol_delivery_d);
				}
				
				rs.close();
			} catch (SQLException e){
				throw new RuntimeException("OrderStat select transaction error", e);
			}
		} catch (Exception e) {
			pStmts.rollback();
			e.printStackTrace();
		}
		
		// Commit.
		pStmts.commit();
			
		return 1;

	}

}
