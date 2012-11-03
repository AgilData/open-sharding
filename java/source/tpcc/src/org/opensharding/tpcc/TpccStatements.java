package org.opensharding.tpcc;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.PrintStream;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.SQLException;

public class TpccStatements {

    private static final Logger logger = LogManager.getLogger(TpccStatements.class);
	
	public static int STMT_COUNT = 35;
	
	private Connection conn;
	private PreparedStatement[] pStmts = new PreparedStatement[STMT_COUNT];
	
	
	public TpccStatements(Connection conn) {
		this.conn = conn;
		init();
	}
	
	public void init() {
		try {
			pStmts[0] = conn.prepareStatement("SELECT c_discount, c_last, c_credit, w_tax FROM customer, warehouse WHERE w_id = ? AND c_w_id = ? AND c_d_id = ? AND c_id = ?");
			pStmts[1] = conn.prepareStatement("SELECT d_next_o_id, d_tax FROM district WHERE d_id = ? AND d_w_id = ? FOR UPDATE");
			pStmts[2] = conn.prepareStatement("UPDATE district SET d_next_o_id = ? + 1 WHERE d_id = ? AND d_w_id = ?");
			pStmts[3] = conn.prepareStatement("INSERT INTO orders (o_id, o_d_id, o_w_id, o_c_id, o_entry_d, o_ol_cnt, o_all_local) VALUES(?, ?, ?, ?, ?, ?, ?)");
			pStmts[4] = conn.prepareStatement("INSERT INTO new_orders (no_o_id, no_d_id, no_w_id) VALUES (?,?,?)");
			pStmts[5] = conn.prepareStatement("SELECT i_price, i_name, i_data FROM item WHERE i_id = ?");
			pStmts[6] = conn.prepareStatement("SELECT s_quantity, s_data, s_dist_01, s_dist_02, s_dist_03, s_dist_04, s_dist_05, s_dist_06, s_dist_07, s_dist_08, s_dist_09, s_dist_10 FROM stock WHERE s_i_id = ? AND s_w_id = ? FOR UPDATE");
			pStmts[7] = conn.prepareStatement("UPDATE stock SET s_quantity = ? WHERE s_i_id = ? AND s_w_id = ?");
			pStmts[8] = conn.prepareStatement("INSERT INTO order_line (ol_o_id, ol_d_id, ol_w_id, ol_number, ol_i_id, ol_supply_w_id, ol_quantity, ol_amount, ol_dist_info) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
			pStmts[9] = conn.prepareStatement("UPDATE warehouse SET w_ytd = w_ytd + ? WHERE w_id = ?");
			pStmts[10] = conn.prepareStatement("SELECT w_street_1, w_street_2, w_city, w_state, w_zip, w_name FROM warehouse WHERE w_id = ?");
			pStmts[11] = conn.prepareStatement("UPDATE district SET d_ytd = d_ytd + ? WHERE d_w_id = ? AND d_id = ?");
			pStmts[12] = conn.prepareStatement("SELECT d_street_1, d_street_2, d_city, d_state, d_zip, d_name FROM district WHERE d_w_id = ? AND d_id = ?");
			pStmts[13] = conn.prepareStatement("SELECT count(c_id) FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_last = ?");
			pStmts[14] = conn.prepareStatement("SELECT c_id FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_last = ? ORDER BY c_first");
			pStmts[15] = conn.prepareStatement("SELECT c_first, c_middle, c_last, c_street_1, c_street_2, c_city, c_state, c_zip, c_phone, c_credit, c_credit_lim, c_discount, c_balance, c_since FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_id = ? FOR UPDATE");
			pStmts[16] = conn.prepareStatement("SELECT c_data FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_id = ?");
			pStmts[17] = conn.prepareStatement("UPDATE customer SET c_balance = ?, c_data = ? WHERE c_w_id = ? AND c_d_id = ? AND c_id = ?");
			pStmts[18] = conn.prepareStatement("UPDATE customer SET c_balance = ? WHERE c_w_id = ? AND c_d_id = ? AND c_id = ?");
			pStmts[19] = conn.prepareStatement("INSERT INTO history(h_c_d_id, h_c_w_id, h_c_id, h_d_id, h_w_id, h_date, h_amount, h_data) VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
			pStmts[20] = conn.prepareStatement("SELECT count(c_id) FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_last = ?");
			pStmts[21] = conn.prepareStatement("SELECT c_balance, c_first, c_middle, c_last FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_last = ? ORDER BY c_first");
			pStmts[22] = conn.prepareStatement("SELECT c_balance, c_first, c_middle, c_last FROM customer WHERE c_w_id = ? AND c_d_id = ? AND c_id = ?");
			pStmts[23] = conn.prepareStatement( "SELECT o_id, o_entry_d, COALESCE(o_carrier_id,0) FROM orders WHERE o_w_id = ? AND o_d_id = ? AND o_c_id = ? AND o_id = (SELECT MAX(o_id) FROM orders WHERE o_w_id = ? AND o_d_id = ? AND o_c_id = ?)");
			pStmts[24] = conn.prepareStatement("SELECT ol_i_id, ol_supply_w_id, ol_quantity, ol_amount, ol_delivery_d FROM order_line WHERE ol_w_id = ? AND ol_d_id = ? AND ol_o_id = ?");
			pStmts[25] = conn.prepareStatement("SELECT COALESCE(MIN(no_o_id),0) FROM new_orders WHERE no_d_id = ? AND no_w_id = ?");
			pStmts[26] = conn.prepareStatement("DELETE FROM new_orders WHERE no_o_id = ? AND no_d_id = ? AND no_w_id = ?");
			pStmts[27] = conn.prepareStatement("SELECT o_c_id FROM orders WHERE o_id = ? AND o_d_id = ? AND o_w_id = ?");
			pStmts[28] = conn.prepareStatement("UPDATE orders SET o_carrier_id = ? WHERE o_id = ? AND o_d_id = ? AND o_w_id = ?");
			pStmts[29] = conn.prepareStatement("UPDATE order_line SET ol_delivery_d = ? WHERE ol_o_id = ? AND ol_d_id = ? AND ol_w_id = ?");
			pStmts[30] = conn.prepareStatement("SELECT SUM(ol_amount) FROM order_line WHERE ol_o_id = ? AND ol_d_id = ? AND ol_w_id = ?");
			pStmts[31] = conn.prepareStatement("UPDATE customer SET c_balance = c_balance + ? , c_delivery_cnt = c_delivery_cnt + 1 WHERE c_id = ? AND c_d_id = ? AND c_w_id = ?");
			pStmts[32] = conn.prepareStatement("SELECT d_next_o_id FROM district WHERE d_id = ? AND d_w_id = ?");
			pStmts[33] = conn.prepareStatement("SELECT DISTINCT ol_i_id FROM order_line WHERE ol_w_id = ? AND ol_d_id = ? AND ol_o_id < ? AND ol_o_id >= (? - 20)");
			pStmts[34] = conn.prepareStatement("SELECT count(*) FROM stock WHERE s_w_id = ? AND s_i_id = ? AND s_quantity < ?");
			
		} catch(Exception e) {
			throw new RuntimeException("init error", e);
		}
	}

	public PreparedStatement getStatement(int idx) {
		if(idx < 0 || idx > pStmts.length) {
			throw new RuntimeException("Index is out of bounds. idx: " + idx);
		}
		return pStmts[idx];
	}
	
    public void setAutoCommit(boolean b) throws SQLException {
        conn.setAutoCommit(b);
    }
	
	/**
	 * Commit a transaction.
	 */
	public void commit() {
        logger.trace("commit()");
        try {
            conn.commit();
        } catch (SQLException e) {
            logger.error("COMMIT FAILED", e);
        }
    }

	/**
	 * Rollback a transaction.
	 */
	public void rollback() {
        logger.trace("rollback()");
        try {
            conn.rollback();
        } catch (SQLException e) {
            logger.error("ROLLBACK FAILED", e);
        }
    }
}
