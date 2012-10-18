package org.opensharding.tpcc;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;

public class Slev implements TpccConstants{
	
	//private static TpccStatements pStmts;
	
	public Slev(TpccStatements pStms){
		//this.pStmts = pStms;
	}
	
	
	public static int slev( int t_num,
			  int w_id_arg,		/* warehouse id */
			  int d_id_arg,		/* district id */
			  int level_arg,		/* stock level */	
			  Connection conn, 
			  TpccStatements pStmts
		)
		{
			try{
				int w_id = w_id_arg;
				int d_id = d_id_arg;
				int level = level_arg;
				int d_next_o_id = 0;
				int i_count = 0;
				int ol_i_id = 0;
			
				//Get prepared statement
				//"SELECT d_next_o_id FROM district WHERE d_id = ? AND d_w_id = ?"
				
				try {
					pStmts.getStatement(32).setInt(1, d_id);
					pStmts.getStatement(32).setInt(2, w_id);
					ResultSet rs = pStmts.getStatement(32).executeQuery();
					if(rs.next()){
						d_next_o_id = rs.getInt(1);
						}
					rs.close();
				} catch (SQLException e) {
					throw new RuntimeException("Slev select transaction error", e);
				}
				
				//Get prepared statement
				//"SELECT DISTINCT ol_i_id FROM order_line WHERE ol_w_id = ? AND ol_d_id = ? AND ol_o_id < ? AND ol_o_id >= (? - 20)"
				try {
					pStmts.getStatement(33).setInt(1, w_id);
					pStmts.getStatement(33).setInt(2, d_id);
					pStmts.getStatement(33).setInt(3, d_next_o_id);
					pStmts.getStatement(33).setInt(4, d_next_o_id);
					ResultSet rs = pStmts.getStatement(32).executeQuery();
					while(rs.next()){
						ol_i_id = rs.getInt(1);
					}
					rs.close();
				} catch (SQLException e) {
					throw new RuntimeException("Slev select transaction error", e);
				}
				
				//Get prepared statement
				//"SELECT count(*) FROM stock WHERE s_w_id = ? AND s_i_id = ? AND s_quantity < ?"
				
				try {
					pStmts.getStatement(34).setInt(1, w_id);
					pStmts.getStatement(34).setInt(2, ol_i_id);
					pStmts.getStatement(34).setInt(3, level);
					ResultSet rs = pStmts.getStatement(34).executeQuery();
					if(rs.next()){
						i_count = rs.getInt(1);
					}
					rs.close();
				} catch (SQLException e) {
					throw new RuntimeException("Slev select transaction error", e);
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
