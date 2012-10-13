package org.opensharding.tpcc;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Timestamp;
import java.util.Calendar;
import java.util.Date;

public class Payment implements TpccConstants{
	
	int payment( int t_num,
		     int w_id_arg,		/* warehouse id */
		     int d_id_arg,		/* district id */
		     int byname,		/* select by c_id or c_last? */
		     int c_w_id_arg,
		     int c_d_id_arg,
		     int c_id_arg,		/* customer id */
		     char c_last_arg[],	        /* customer last name */
		     float h_amount_arg,	        /* payment amount */
		     Connection conn,
		     TpccStatements pStmts
	)
	{
		int w_id = w_id_arg;
		int d_id = d_id_arg;
		int c_id = c_id_arg;
		char[] w_name = new char[11];
		char[] w_street_1 = new char[21];
		char[] w_street_2 = new char[21];
		char[] w_city = new char[21];
		char[] w_state = new char[3];
		char[] w_zip = new char[10];
		int c_d_id = c_d_id_arg;
		int c_w_id = c_w_id_arg;
		char[] c_first = new char[17];
		char[] c_middle = new char[3];
		char[] c_last = new char[17];
		char[] c_street_1 = new char[21];
		char[] c_street_2 = new char[21];
		char[] c_city = new char[21];
		char[] c_state = new char[3];
		char[] c_zip = new char[10];
		char[] c_phone =  new char[17];
		char[] c_since = new char[20];
		char[] c_credit = new char[4];
		int c_credit_lim = 0;
		float c_discount = 0;
		float c_balance = 0;
		char[] c_data = new char[502];
		char[] c_new_data = new char[502];
		float h_amount = h_amount_arg;
		char[] h_data = new char[26];
		char[] d_name = new char[11];
		char[] d_street_1 = new char[21];
		char[] d_street_2 = new char[21];
		char[] d_city = new char[21];
		char[] d_state = new char[3];
		char[] d_zip = new char[10];
		int namecnt = 0;
		char[] datetime = new char[81];

		int n;
		int proceed = 0;
        
        //Time Stamp
        Calendar calendar = Calendar.getInstance();
		Date now = calendar.getTime();
		Timestamp currentTimeStamp = new Timestamp(now.getTime());

		proceed = 1;
		//Get prepared statement
		//"UPDATE warehouse SET w_ytd = w_ytd + ? WHERE w_id = ?"
		try {
			pStmts.getStatement(9).setFloat(1, h_amount);
			pStmts.getStatement(9).setInt(2, w_id);
			pStmts.getStatement(9).executeQuery();
		} catch (SQLException e) {
			throw new RuntimeException("Payment Update transaction error", e);
		}
		
		proceed = 2;
		//Get prepared statement
		//"SELECT w_street_1, w_street_2, w_city, w_state, w_zip, w_name FROM warehouse WHERE w_id = ?"
		
		try {
			pStmts.getStatement(10).setInt(1, w_id);
			ResultSet rs = pStmts.getStatement(10).executeQuery();
			while(rs.next()){
				w_street_1 = rs.getString(1).toCharArray();
				w_street_2 = rs.getString(2).toCharArray();
				w_city = rs.getString(3).toCharArray();
				w_state = rs.getString(4).toCharArray();
				w_zip = rs.getString(5).toCharArray();
				w_name = rs.getString(6).toCharArray();
			}
			rs.close();
			
		} catch (SQLException e) {
			throw new RuntimeException("Payment select transaction error", e);
		}
		
		proceed = 3;
		//Get prepared statement
		//"UPDATE district SET d_ytd = d_ytd + ? WHERE d_w_id = ? AND d_id = ?"
		try {
			pStmts.getStatement(11).setFloat(1, h_amount);
			pStmts.getStatement(11).setInt(2, w_id);
			pStmts.getStatement(11).setInt(3, d_id);
			pStmts.getStatement(11).executeQuery();
		} catch (SQLException e) {
			throw new RuntimeException("Payment update transaction error", e);
		}
		
		proceed = 4;
		//Get prepared statement
		//"SELECT d_street_1, d_street_2, d_city, d_state, d_zip, d_name FROM district WHERE d_w_id = ? AND d_id = ?"
		
		try {
			pStmts.getStatement(12).setInt(1, w_id);
			pStmts.getStatement(12).setInt(2, d_id);
			ResultSet rs = pStmts.getStatement(12).executeQuery();
			while(rs.next()){
				d_street_1 = rs.getString(1).toCharArray();
				d_street_2 = rs.getString(2).toCharArray();
				d_city = rs.getString(3).toCharArray();
				d_state = rs.getString(4).toCharArray();
				d_zip = rs.getString(5).toCharArray();
				d_name = rs.getString(6).toCharArray();
				
			}
			
			rs.close();
		} catch (SQLException e) {
			throw new RuntimeException("Payment select transaction error", e);
		}
		
		if (byname == 1) {
			c_last = c_last_arg;
			proceed = 5;
			//Get prepared statement
			//"SELECT count(c_id) FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_last = ?"
			
			try {
				pStmts.getStatement(13).setInt(1, c_w_id);
				pStmts.getStatement(13).setInt(2, c_d_id);
				pStmts.getStatement(13).setString(3, c_last.toString());
				ResultSet rs = pStmts.getStatement(13).executeQuery();
				while(rs.next()){
					namecnt = rs.getInt(1);
				}
				
				rs.close();
			} catch (SQLException e) {
				throw new RuntimeException("Payment select transaction error", e);
			}
			
			//Get prepared Transaction
			//"SELECT c_id FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_last = ? ORDER BY c_first"
			
			try {
				pStmts.getStatement(14).setInt(1, c_w_id);
				pStmts.getStatement(14).setInt(2, c_d_id);
				pStmts.getStatement(14).setString(3, c_last.toString());
				ResultSet rs = pStmts.getStatement(14).executeQuery();
				while(rs.next()){
					c_id = rs.getInt(1);
				}
				rs.close();
			} catch (SQLException e) {
				throw new RuntimeException("Payment select transaction error", e);
			}
		
			if (namecnt % 2 == 1) 
				namecnt++;	/* Locate midpoint customer; */
			
		}
			
		proceed = 6;
		//Get the prepared statement
		//"SELECT c_first, c_middle, c_last, c_street_1, c_street_2, c_city, c_state, c_zip, c_phone, c_credit, c_credit_lim, c_discount, c_balance, c_since FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_id = ? FOR UPDATE"
		try {
			pStmts.getStatement(15).setInt(1, c_w_id);
			pStmts.getStatement(15).setInt(2, c_d_id);
			pStmts.getStatement(15).setInt(3, c_id);
			ResultSet rs = pStmts.getStatement(15).executeQuery();
			while(rs.next()){
				c_first = rs.getString(1).toCharArray();
				c_middle = rs.getString(2).toCharArray();
				c_last = rs.getString(3).toCharArray();
				c_street_1 = rs.getString(4).toCharArray();
				c_street_2 = rs.getString(5).toCharArray();
				c_city = rs.getString(6).toCharArray();
				c_state = rs.getString(7).toCharArray();
				c_zip = rs.getString(8).toCharArray();
				c_phone = rs.getString(9).toCharArray();
				c_credit = rs.getString(10).toCharArray();
				c_credit_lim = rs.getInt(11);
				c_discount = rs.getFloat(12);
				c_balance = rs.getFloat(13);
				c_since = rs.getString(14).toCharArray();
			}
			rs.close();
		} catch (SQLException e) {
			throw new RuntimeException("Payment select transaction error", e);
		}
		
		c_balance += h_amount;
		c_credit[2] = '\0';
		if (c_credit.toString().contains("BC")) {
			proceed = 7;
			//Get Prepared Statement
			//"SELECT c_data FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_id = ?"
			try {
				pStmts.getStatement(16).setInt(1, c_w_id);
				pStmts.getStatement(16).setInt(2, c_d_id);
				pStmts.getStatement(16).setInt(3, c_id);
				ResultSet rs = pStmts.getStatement(16).executeQuery();
				while(rs.next()){
					c_data = rs.getString(1).toCharArray();
				}
				rs.close();
			} catch (SQLException e) {
				throw new RuntimeException("Payment select transaction error", e);
			}
			
			c_new_data = String.format("| %4d %2d %4d %2d %4d $%7.2f %12c %24c", c_id, c_d_id, c_w_id, d_id, w_id, h_amount, datetime, c_data).toCharArray();
			
			c_new_data = ( c_new_data.toString() + c_data.toString().substring(0, (500 - c_new_data.length) ) ).toCharArray();

			c_new_data[500] = '\0';

			proceed = 8;
			//Get prepared statement
			//"UPDATE customer SET c_balance = ?, c_data = ? WHERE c_w_id = ? AND c_d_id = ? AND c_id = ?"
			try {
				pStmts.getStatement(17).setFloat(1, c_balance);
				pStmts.getStatement(17).setString(2, c_data.toString());
				pStmts.getStatement(17).setInt(3, c_w_id);
				pStmts.getStatement(17).setInt(4, c_d_id);
				pStmts.getStatement(17).setInt(5, c_id);
				pStmts.getStatement(17).executeQuery();
			} catch (SQLException e) {
				throw new RuntimeException("Payment update transaction error", e);
			}
		
			
			
		} else {
			proceed = 9;
			//Get prepared statement
			//"UPDATE customer SET c_balance = ? WHERE c_w_id = ? AND c_d_id = ? AND c_id = ?"
			
			try {
				pStmts.getStatement(18).setFloat(1, c_balance);
				pStmts.getStatement(18).setInt(2, c_w_id);
				pStmts.getStatement(18).setInt(3, c_d_id);
				pStmts.getStatement(18).setInt(4, c_id);
				pStmts.getStatement(18).executeQuery();
			} catch (SQLException e) {
				throw new RuntimeException("Payment update transaction error", e);
			}
			
		}
		
		h_data = String.copyValueOf(w_name, 0, 10).toCharArray();
		h_data[10] = '\0';
		h_data = String.copyValueOf(d_name, 0, 10).toCharArray();
		h_data[20] = ' ';
		h_data[21] = ' ';
		h_data[22] = ' ';
		h_data[23] = ' ';
		h_data[24] = '\0';

		proceed = 10;
		//Get prepared statement
		//"INSERT INTO history(h_c_d_id, h_c_w_id, h_c_id, h_d_id, h_w_id, h_date, h_amount, h_data) VALUES(?, ?, ?, ?, ?, ?, ?, ?)"
		try {
			pStmts.getStatement(19).setInt(1, c_d_id);
			pStmts.getStatement(19).setInt(2, c_w_id);
			pStmts.getStatement(19).setInt(3, c_id);
			pStmts.getStatement(19).setInt(4, d_id);
			pStmts.getStatement(19).setInt(5, w_id);
			pStmts.getStatement(19).setString(6, currentTimeStamp.toString());
			pStmts.getStatement(19).setFloat(7, h_amount);
			pStmts.getStatement(19).setString(8, h_data.toString());
			pStmts.getStatement(19).executeQuery();
		} catch (SQLException e) {
			throw new RuntimeException("Payment insert transaction error", e);
		}
		

		return (1);

		
		
	}

}
