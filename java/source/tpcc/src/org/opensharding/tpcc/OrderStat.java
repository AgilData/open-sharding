package org.opensharding.tpcc;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;

public class OrderStat implements TpccConstants{
	
	int ordStat( int t_num,
		     int w_id_arg,		/* warehouse id */
		     int d_id_arg,		/* district id */
		     int byname,		/* select by c_id or c_last? */
		     int c_id_arg,		/* customer id */
		     char c_last_arg[], /* customer last name, format? */
		     Connection conn,
		     TpccStatements pStmts
	){
		int w_id = w_id_arg;
		int d_id = d_id_arg;
		int c_id = c_id_arg;
		int c_d_id = d_id;
		int c_w_id = w_id;
		char[] c_first = new char[17];
		char[] c_middle = new char[3];
		char[] c_last = new char[17];
		float c_balance = 0;
		int o_id = 0;
		char[] o_entry_d = new char[25];
		int o_carrier_id = 0;
		int ol_i_id = 0;
		int ol_supply_w_id = 0;
		int ol_quantity = 0;
		float ol_amount = 0;
		char[] ol_delivery_d = new char[25];
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
				pStmts.getStatement(20).setString(3, c_last.toString());
				ResultSet rs = pStmts.getStatement(20).executeQuery();
				while(rs.next()){
					namecnt = rs.getInt(1);
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
				pStmts.getStatement(21).setString(3, c_last.toString());
				ResultSet rs = pStmts.getStatement(21).executeQuery();
				while(rs.next()){
					c_balance = rs.getFloat(1);
					c_first = rs.getString(2).toCharArray();
					c_middle = rs.getString(3).toCharArray();
					c_last = rs.getString(4).toCharArray();
				}
				
				rs.close();
			} catch (SQLException e) {
				throw new RuntimeException("OrderStat Select transaction error", e);
			}
			


			if (namecnt % 2 == 1){ //?? Check
				namecnt++;	
			} /* Locate midpoint customer; */
			
			//Not For sure about the for loop here:
				
			} else {		/* by number */
				proceed = 6;
				
				//Get Transaction Number 
				//"SELECT c_balance, c_first, c_middle, c_last FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_id = ?"
				try{
					pStmts.getStatement(22).setInt(1, c_w_id);
					pStmts.getStatement(22).setInt(2, c_d_id);
					pStmts.getStatement(22).setInt(3, c_id);
					ResultSet rs = pStmts.getStatement(22).executeQuery();
					while(rs.next()){
						c_balance = rs.getFloat(1);
						c_first = rs.getString(2).toCharArray();
						c_middle = rs.getString(3).toCharArray();
						c_last = rs.getString(4).toCharArray();
						
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
				ResultSet rs = pStmts.getStatement(23).executeQuery();
				while (rs.next()){
					o_id = rs.getInt(1);
					o_entry_d = rs.getString(2).toCharArray();
					o_carrier_id = rs.getInt(3);
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
				ResultSet rs = pStmts.getStatement(24).executeQuery();
				while(rs.next()){
					ol_i_id = rs.getInt(1);
					ol_supply_w_id = rs.getInt(2);
					ol_quantity = rs.getInt(3);
					ol_amount = rs.getFloat(4);
					ol_delivery_d = rs.getString(5).toCharArray();
				}
				
				rs.close();
			} catch (SQLException e){
				throw new RuntimeException("OrderStat select transaction error", e);
			}
			
		return(1);

	}

}
