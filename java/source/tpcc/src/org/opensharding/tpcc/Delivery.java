package org.opensharding.tpcc;

import java.sql.*;
import java.util.Calendar;
import java.util.Date;

public class Delivery implements TpccConstants {
	
	private static TpccStatements pStmts;
	
	public Delivery(TpccStatements pStmts) {
		this.pStmts = pStmts;
	}
	
	public static int delivery( int t_num, int w_id_arg, int o_carrier_id_arg){
		try{
			// Start a transaction.
			pStmts.getConnection().setAutoCommit(false);
			
			int w_id = w_id_arg;
			int o_carrier_id = o_carrier_id_arg;
			int d_id = 0;
			int c_id = 0;
			int no_o_id = 0;
			float ol_total = 0;
	
			int proceed = 0;
			
			
		
			//gettimestamp(datetime, STRFTIME_FORMAT, TIMESTAMP_LEN); Java Equivalent below?
			Calendar calendar = Calendar.getInstance();
			Date now = calendar.getTime();
			Timestamp currentTimeStamp = new Timestamp(now.getTime());
			
			for (d_id = 1; d_id <= DIST_PER_WARE; d_id++){
				
				proceed = 1;
				
				
				// Get the prepared statement. 
				//"SELECT COALESCE(MIN(no_o_id),0) FROM new_orders WHERE no_d_id = ? AND no_w_id = ?"
				try {
					pStmts.getStatement(25).setInt(1, d_id);
					pStmts.getStatement(25).setInt(2, w_id);
					ResultSet rs = pStmts.getStatement(25).executeQuery();
					
					if(rs.next()) {
						no_o_id = rs.getInt(1);
					}
					rs.close();
				} catch (SQLException e) {
					throw new RuntimeException("Delivery Select transaction error", e);
				}
				
				if(no_o_id == 0) continue;
				proceed = 2;
	
				//Get the prepared statement
				//"DELETE FROM new_orders WHERE no_o_id = ? AND no_d_id = ? AND no_w_id = ?"
				try {
					pStmts.getStatement(26).setInt(1, no_o_id);
					pStmts.getStatement(26).setInt(2, d_id);
					pStmts.getStatement(26).setInt(3, w_id);
					pStmts.getStatement(26).executeQuery();
				} catch (SQLException e) {
					throw new RuntimeException(" Delivery Delete transaction error", e);
				}
				
				proceed = 3;
				
				//Get the prepared statement
				//"SELECT o_c_id FROM orders WHERE o_id = ? AND o_d_id = ? AND o_w_id = ?"
				
				try {
					pStmts.getStatement(27).setInt(1, no_o_id);
					pStmts.getStatement(27).setInt(2, d_id);
					pStmts.getStatement(27).setInt(3, w_id);
					ResultSet rs = pStmts.getStatement(27).executeQuery();
					
					if(rs.next()){
						c_id = rs.getInt(1);
					}
					rs.close();
				} catch (SQLException e) {
					throw new RuntimeException(" Delivery Select transaction error", e);
				}
				
				proceed = 4;
				
				//Get the prepared Statement 
				//"UPDATE orders SET o_carrier_id = ? WHERE o_id = ? AND o_d_id = ? AND o_w_id = ?"
				try {
					pStmts.getStatement(28).setInt(1, o_carrier_id);
					pStmts.getStatement(28).setInt(2, no_o_id);
					pStmts.getStatement(28).setInt(3, d_id);
					pStmts.getStatement(28).setInt(4, w_id);
					pStmts.getStatement(28).executeUpdate();
				} catch (SQLException e) {
					throw new RuntimeException("Delivery Update transcation error", e);
				}
				
				proceed = 5;
				
				//Get the prepared Statement
				//"UPDATE order_line SET ol_delivery_d = ? WHERE ol_o_id = ? AND ol_d_id = ? AND ol_w_id = ?"
				try {
					pStmts.getStatement(29).setString(1, currentTimeStamp.toString());
					pStmts.getStatement(29).setInt(2, no_o_id);
					pStmts.getStatement(29).setInt(3, d_id);
					pStmts.getStatement(29).setInt(4, w_id);
					pStmts.getStatement(29).executeUpdate();
				} catch (SQLException e) {
					throw new RuntimeException("Delivery Update transaction error", e);
				}
				
				proceed = 6;
				
				//Get the prepared Statement
				//"SELECT SUM(ol_amount) FROM order_line WHERE ol_o_id = ? AND ol_d_id = ? AND ol_w_id = ?"
				try {
					pStmts.getStatement(30).setInt(1, no_o_id);
					pStmts.getStatement(30).setInt(2, d_id);
					pStmts.getStatement(30).setInt(3, w_id);
					ResultSet rs = pStmts.getStatement(30).executeQuery();
					if(rs.next()){
						ol_total = rs.getFloat(1);
					}
					
					rs.close();
				} catch (SQLException e) {
					throw new RuntimeException("Delivery Select transaction error", e);
				}
				
				proceed = 7;
				
				//Get the prepared statement
				//"UPDATE customer SET c_balance = c_balance + ? , c_delivery_cnt = c_delivery_cnt + 1 WHERE c_id = ? AND c_d_id = ? AND c_w_id = ?"
				
				try {
					pStmts.getStatement(31).setFloat(1, ol_total);
					pStmts.getStatement(31).setInt(2, c_id);
					pStmts.getStatement(31).setInt(2, d_id);
					pStmts.getStatement(31).setInt(3, w_id);
					pStmts.getStatement(31).executeUpdate();
				} catch (SQLException e) {
					throw new RuntimeException("Delivery Update transaction error", e);
				}
			}
		
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
		
		return 1;
		
	}

}