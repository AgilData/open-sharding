package org.opensharding.tpcc;

import java.sql.Statement;
import java.sql.Connection;
import java.sql.SQLException;
import java.sql.Timestamp;
import java.util.Calendar;
import java.util.Date;

public class Load implements TpccConstants {
    private static boolean optionDebug = false;

    /*
      * ==================================================================+ |
      * ROUTINE NAME |      LoadItems | DESCRIPTION |      Loads the Item table |
      * ARGUMENTS |      none
      * +==================================================================
      */
    public static void loadItems(Connection conn, int shardCount, boolean option_debug) {
        optionDebug = option_debug;
        int i_id = 0;
        int i_im_id = 0;
        String i_name = null;
        float i_price = 0;
        String i_data = null;

        int idatasize = 0;
        int[] orig = new int[MAXITEMS + 1];
        int pos = 0;
        int i = 0;
        int retried = 0;
        Statement stmt;
        try {
            stmt = conn.createStatement();
        } catch (SQLException e) {
            throw new RuntimeException("Items statement creation error", e);
        }


        /* EXEC SQL WHENEVER SQLERROR GOTO sqlerr; */

        System.out.printf("Loading Item \n");

        for (i = 0; i < MAXITEMS / 10; i++)
            orig[i] = 0;
        for (i = 0; i < MAXITEMS / 10; i++) {
            do {
                pos = Util.randomNumber(0, MAXITEMS);
            } while (orig[pos] != 0);
            orig[pos] = 1;
        }

        final String itemStub = "INSERT INTO item (i_id, i_im_id, i_name, i_price, i_data) VALUES ";
        final StringBuilder itemSQL = new StringBuilder();
        int itemBatchSize = 0;
        int itemMaxBatchSize = 100;

        for (i_id = 1; i_id <= MAXITEMS; i_id++) {

            /* Generate Item Data */
            i_im_id = Util.randomNumber(1, 10000);

            i_name = Util.makeAlphaString(14, 24);
            if (i_name == null) {
                System.out.println("I_name null.");
                System.exit(1);
            }

            i_price = (float) ((int) (Util.randomNumber(100, 10000)) / 100.0);

            i_data = Util.makeAlphaString(26, 50);
            if (orig[i_id] != 0) {
                pos = Util.randomNumber(0, i_data.length() - 8);
                i_data = i_data.substring(0, pos) + "original" + i_data.substring(pos + 8);
            }

            /*System.out.printf("IID = %d, Name= %s, Price = %f\n",
                          i_id, i_name, i_price); *///DEBUG

            /* EXEC SQL INSERT INTO
                               item
                               values(:i_id,:i_im_id,:i_name,:i_price,:i_data); */
            try {

                if (itemBatchSize == 0) {
                    itemSQL.append(itemStub);
                } else {
                    itemSQL.append(',');
                }

                itemSQL.append("(");
                itemSQL.append(i_id).append(",");
                itemSQL.append(i_im_id).append(",");
                itemSQL.append("'").append(i_name).append("',");
                itemSQL.append(i_price).append(',');
                itemSQL.append("'").append(i_data).append("'");
                itemSQL.append(")");

                if (++itemBatchSize == itemMaxBatchSize) {
                    stmt.execute(itemSQL.toString());
                    itemSQL.setLength(0);
                    itemBatchSize = 0;
                }

            } catch (SQLException e) {
                throw new RuntimeException("Item insert error", e);
            }

            if ((i_id % 100) == 0) {
                System.out.printf(".");
                if ((i_id % 5000) == 0)
                    System.out.printf(" %d\n", i_id);
            }
        }

        /* EXEC SQL COMMIT WORK; */

        try {
            if (itemBatchSize>0) {
                stmt.execute(itemSQL.toString());
            }
            stmt.close();
        } catch (SQLException e) {
            throw new RuntimeException("Item batch error", e);
        }

        System.out.printf("Item Done. \n");
    }

    /*
      * ==================================================================+ |
      * ROUTINE NAME |      LoadWare | DESCRIPTION |      Loads the Warehouse
      * table |      Loads Stock, District as Warehouses are created | ARGUMENTS |
      * none +==================================================================
      */
    public static void loadWare(Connection conn, int shardCount, int min_ware, int max_ware, boolean option_debug, int shardId) {

        int w_id;
        String w_name = null;
        String w_street_1 = null;
        String w_street_2 = null;
        String w_city = null;
        String w_state = null;
        String w_zip = null;
        double w_tax = 0;
        double w_ytd = 0;

        int tmp = 0;
        boolean retried = false;
        int currentShard = 0;
        Statement stmt;


        try {
            stmt = conn.createStatement();
        } catch (SQLException e) {
            throw new RuntimeException("Creation of statement failed", e);
        }

        System.out.printf("Loading Warehouse \n");

        final String warehouseStub = "INSERT INTO warehouse (w_id, w_name, w_street_1, w_street_2, w_city, w_state, w_zip, w_tax, w_ytd) VALUES ";
        final StringBuilder warehouseSQL = new StringBuilder();
        final Record warehouseRecord = new Record(9);
        int warehouseBatchSize = 0;
        int warehouseMaxBatchSize = 100;


        retry:
        if (retried)
            System.out.printf("Retrying ....\n");
        retried = true;
        for (w_id = 1; w_id <= max_ware; w_id++) {

            if (shardCount > 0) {
                currentShard = (w_id % shardCount);
                if (currentShard == 0) {
                    currentShard = shardCount;
                }
            }

            if ((currentShard == shardId) || (shardId == 0)) {
                System.out.println("Current Shard: " + currentShard);
                /* Generate Warehouse Data */

                w_name = Util.makeAlphaString(6, 10);
                w_street_1 = Util.makeAlphaString(10, 20);
                w_street_2 = Util.makeAlphaString(10, 20);
                w_city = Util.makeAlphaString(10, 20);
                w_state = Util.makeAlphaString(2, 2);
                w_zip = Util.makeAlphaString(9, 9);

                w_tax = ((double) Util.randomNumber(10, 20) / 100.0);
                w_ytd = 3000000.00;

                //if (option_debug)
                System.out.printf("WID = %d, Name= %s, Tax = %f\n",
                        w_id, w_name, w_tax);
                /*EXEC SQL INSERT INTO
                                        warehouse
                                        values(:w_id,:w_name,
                                       :w_street_1,:w_street_2,:w_city,:w_state,
                                       :w_zip,:w_tax,:w_ytd);*/
                //   /*DBS_HINT: dbs_shard_action=shard_read, dbs_pshard=2 */
                try {

                    warehouseRecord.reset();
                    warehouseRecord.add(w_id);
                    warehouseRecord.add(w_name);
                    warehouseRecord.add(w_street_1);
                    warehouseRecord.add(w_street_2);
                    warehouseRecord.add(w_city);
                    warehouseRecord.add(w_state);
                    warehouseRecord.add(w_zip);
                    warehouseRecord.add(w_tax);
                    warehouseRecord.add(w_ytd);

                    if (warehouseBatchSize==0) {
                        warehouseSQL.append(warehouseStub);
                    }
                    else {
                        warehouseSQL.append(",");
                    }

                    warehouseSQL.append("(");
                    warehouseRecord.append(warehouseSQL, ",");
                    warehouseSQL.append(")");

                    if (++warehouseBatchSize == warehouseMaxBatchSize) {
                        stmt.execute(warehouseSQL.toString());
                        warehouseSQL.setLength(0);
                        warehouseBatchSize = 0;
                    }


                } catch (SQLException e) {
                    throw new RuntimeException("Warehouse insert error", e);
                }

                /** Make Rows associated with Warehouse **/
                stock(w_id, conn, shardCount, currentShard);
                district(w_id, conn, shardCount, currentShard);

            }

        }
        /* EXEC SQL COMMIT WORK; */
        //TODO: Throw an exception here

        try {
            if (warehouseBatchSize > 0) {
                stmt.execute(warehouseSQL.toString());
            }
            stmt.close();
        } catch (SQLException e) {
            throw new RuntimeException("Warehouse batch error", e);
        }
    }

    /*
      * ==================================================================+ |
      * ROUTINE NAME |      LoadCust | DESCRIPTION |      Loads the Customer Table
      * | ARGUMENTS |      none
      * +==================================================================
      */
    public static void loadCust(Connection conn, int shardCount, int min_ware, int max_ware, int shardId) {

        int w_id = 1;
        int d_id = 1;

        /* EXEC SQL WHENEVER SQLERROR GOTO sqlerr; */

        for (; w_id <= max_ware; w_id++)
            for (d_id = 1; d_id <= DIST_PER_WARE; d_id++)
                customer(d_id, w_id, conn, shardCount, shardId);

        return;
    }

    /*
      * ==================================================================+ |
      * ROUTINE NAME |      LoadOrd | DESCRIPTION |      Loads the Orders and
      * Order_Line Tables | ARGUMENTS |      none
      * +==================================================================
      */
    public static void loadOrd(Connection conn, int shardCount, int max_ware, int shardId) {
        // for each warehouse
        for (int w_id = 1; w_id <= max_ware; w_id++) {
            // for each district
            for (int d_id = 1; d_id <= DIST_PER_WARE; d_id++) {
                // generate orders
                orders(d_id, w_id, conn, shardCount, shardId);
            }
        }
    }

    /*
      * ==================================================================+ |
      * ROUTINE NAME |      Stock | DESCRIPTION |      Loads the Stock table |
      * ARGUMENTS |      w_id - warehouse id
      * +==================================================================
      */
    public static boolean stock(int w_id, Connection conn, int shardCount, int currentShard) {
        int s_i_id = 0;
        int s_w_id = 0;
        int s_quantity = 0;

        String s_dist_01 = null;
        String s_dist_02 = null;
        String s_dist_03 = null;
        String s_dist_04 = null;
        String s_dist_05 = null;
        String s_dist_06 = null;
        String s_dist_07 = null;
        String s_dist_08 = null;
        String s_dist_09 = null;
        String s_dist_10 = null;
        String s_data = null;

        int sdatasize = 0;
        int[] orig = new int[MAXITEMS + 1];
        int pos = 0;
        int i = 0;
        boolean error = false;
        Statement stmt;
        try {
            stmt = conn.createStatement();
        } catch (SQLException e) {
            throw new RuntimeException("Stament creation error", e);
        }

        final String stockStub = "INSERT INTO stock (s_i_id, s_w_id, s_quantity, " +
                "s_dist_01, s_dist_02, s_dist_03, s_dist_04, s_dist_05, s_dist_06, " +
                "s_dist_07, s_dist_08, s_dist_09, s_dist_10, s_ytd, s_order_cnt, " +
                "s_remote_cnt, s_data) VALUES ";
        final StringBuilder stockSQL = new StringBuilder();
        int stockBatchSize = 0;
        int stockMaxBatchSize = 100;

        /* EXEC SQL WHENEVER SQLERROR GOTO sqlerr;*/
        System.out.printf("Loading Stock Wid=%d\n", w_id);
        s_w_id = w_id;

        for (i = 0; i < MAXITEMS / 10; i++)
            orig[i] = 0;
        for (i = 0; i < MAXITEMS / 10; i++) {
            do {
                pos = Util.randomNumber(0, MAXITEMS);
            } while (orig[pos] != 0); //TODO: FIx later
            orig[pos] = 1;
        }

        retry:
        for (s_i_id = 1; s_i_id <= MAXITEMS; s_i_id++) {

            /* Generate Stock Data */
            s_quantity = Util.randomNumber(10, 100);

            s_dist_01 = Util.makeAlphaString(24, 24);
            s_dist_02 = Util.makeAlphaString(24, 24);
            s_dist_03 = Util.makeAlphaString(24, 24);
            s_dist_04 = Util.makeAlphaString(24, 24);
            s_dist_05 = Util.makeAlphaString(24, 24);
            s_dist_06 = Util.makeAlphaString(24, 24);
            s_dist_07 = Util.makeAlphaString(24, 24);
            s_dist_08 = Util.makeAlphaString(24, 24);
            s_dist_09 = Util.makeAlphaString(24, 24);
            s_dist_10 = Util.makeAlphaString(24, 24);


            s_data = Util.makeAlphaString(26, 50);
            sdatasize = s_data.length();
            if (orig[s_i_id] != 0) {//TODO:Change this later
                pos = Util.randomNumber(0, sdatasize - 8);
                s_data = "original";
            }
            /*EXEC SQL INSERT INTO
                               stock
                               values(:s_i_id,:s_w_id,:s_quantity,
                              :s_dist_01,:s_dist_02,:s_dist_03,:s_dist_04,:s_dist_05,
                              :s_dist_06,:s_dist_07,:s_dist_08,:s_dist_09,:s_dist_10,
                              0, 0, 0,:s_data);*/

            try {

                if (stockBatchSize==0) {
                    stockSQL.append(stockStub);
                }
                else {
                    stockSQL.append(",");
                }

                stockSQL.append("(");
                stockSQL.append(s_i_id).append(",");
                stockSQL.append(s_w_id).append(",");
                stockSQL.append(s_quantity).append(",");
                stockSQL.append("'").append(s_dist_01).append("',");
                stockSQL.append("'").append(s_dist_02).append("',");
                stockSQL.append("'").append(s_dist_03).append("',");
                stockSQL.append("'").append(s_dist_04).append("',");
                stockSQL.append("'").append(s_dist_05).append("',");
                stockSQL.append("'").append(s_dist_06).append("',");
                stockSQL.append("'").append(s_dist_07).append("',");
                stockSQL.append("'").append(s_dist_08).append("',");
                stockSQL.append("'").append(s_dist_09).append("',");
                stockSQL.append("'").append(s_dist_10).append("',");
                stockSQL.append(0).append(",");
                stockSQL.append(0).append(",");
                stockSQL.append(0).append(",");
                stockSQL.append("'").append(s_data).append("'");
                stockSQL.append(")");

                if (++stockBatchSize == stockMaxBatchSize) {
                    stmt.execute(stockSQL.toString());
                    stockSQL.setLength(0);
                    stockBatchSize = 0;
                }

            } catch (SQLException e) {
                throw new RuntimeException("Stock insert error", e);
            }

            if (optionDebug)
                System.out.printf("SID = %d, WID = %d, Quan = %d\n",
                        s_i_id, s_w_id, s_quantity);

            if ((s_i_id % 100) == 0) {
                System.out.printf(".");
                if ((s_i_id % 5000) == 0)
                    System.out.printf(" %d\n", s_i_id);
            }
        }

        try {
            if (stockBatchSize>0) {
                stmt.execute(stockSQL.toString());
            }
            stmt.close();
        } catch (SQLException e) {
            throw new RuntimeException("Stock batch error", e);
        }


        System.out.printf(" Stock Done.\n");
        return error;

    }

    /*
      * ==================================================================+ |
      * ROUTINE NAME |      District | DESCRIPTION |      Loads the District table
      * | ARGUMENTS |      w_id - warehouse id
      * +==================================================================
      */
    public static boolean district(int w_id, Connection conn, int shardCount, int currentShard) {
        int d_id;
        int d_w_id;
        String d_name;
        String d_street_1;
        String d_street_2;
        String d_city;
        String d_state;
        String d_zip;
        float d_tax;
        float d_ytd;
        int d_next_o_id;
        boolean error = false;
        Statement stmt;
        try {
            stmt = conn.createStatement();
        } catch (SQLException e1) {
            throw new RuntimeException("District statemet creation error", e1);
        }

        System.out.printf("Loading District\n");
        d_w_id = w_id;
        d_ytd = (float) 30000.0;
        d_next_o_id = 3001;
        
        final String districtStub = "INSERT INTO district (d_id, d_w_id, d_name, d_street_1, d_street_2, " +
                "d_city, d_state, d_zip, d_tax, d_ytd, d_next_o_id) VALUES ";
        final StringBuilder districtSQL = new StringBuilder();
        int districtBatchSize = 0;
        int districtMaxBatchSize = 100;

        retry:
        for (d_id = 1; d_id <= DIST_PER_WARE; d_id++) {

            /* Generate District Data */

            d_name = Util.makeAlphaString(6, 10);
            d_street_1 = Util.makeAlphaString(10, 20);
            d_street_2 = Util.makeAlphaString(10, 20);
            d_city = Util.makeAlphaString(10, 20);
            d_state = Util.makeAlphaString(2, 2);
            d_zip = Util.makeAlphaString(9, 9);

            d_tax = (float) (((float) Util.randomNumber(10, 20)) / 100.0);

            /*EXEC SQL INSERT INTO
                               district
                               values(:d_id,:d_w_id,:d_name,
                              :d_street_1,:d_street_2,:d_city,:d_state,:d_zip,
                              :d_tax,:d_ytd,:d_next_o_id);*/
            try {
                
                if (districtBatchSize==0) {
                    districtSQL.append(districtStub);
                }
                else {
                    districtSQL.append(",");
                }
                
                districtSQL.append("(");
                districtSQL.append(d_id).append(",");
                districtSQL.append(d_w_id).append(",");
                districtSQL.append("'").append(d_name).append("',");
                districtSQL.append("'").append(d_street_1).append("',");
                districtSQL.append("'").append(d_street_2).append("',");
                districtSQL.append("'").append(d_city).append("',");
                districtSQL.append("'").append(d_state).append("',");
                districtSQL.append("'").append(d_zip).append("',");
                districtSQL.append(d_tax).append(",");
                districtSQL.append(d_ytd).append(",");
                districtSQL.append(d_next_o_id);
                districtSQL.append(")");
                
                if (++districtBatchSize == districtMaxBatchSize) {
                    stmt.execute(districtSQL.toString());
                    districtSQL.setLength(0);
                    districtBatchSize = 0;
                }

            } catch (SQLException e) {
                throw new RuntimeException("District insert batch error", e);
            }
            if (optionDebug)
                System.out.printf("DID = %d, WID = %d, Name = %s, Tax = %f\n",
                        d_id, d_w_id, d_name, d_tax);

        }

        try {
            if (districtBatchSize > 0) {
                stmt.execute(districtSQL.toString());
            }
            stmt.close();
        } catch (SQLException e) {
            throw new RuntimeException("District execute batach error", e);
        }

        return error;

    }

    /*
      * ==================================================================+ |
      * ROUTINE NAME |      Customer | DESCRIPTION |      Loads Customer Table |
      * Also inserts corresponding history record | ARGUMENTS |      id   -
      * customer id |      d_id - district id |      w_id - warehouse id
      * +==================================================================
      */
    public static void customer(int d_id, int w_id, Connection conn, int shardCount, int shardId) {
        int c_id = 0;
        int c_d_id = 0;
        int c_w_id = 0;
        String c_first = null;
        String c_middle = null;
        String c_last = null;
        String c_street_1 = null;
        String c_street_2 = null;
        String c_city = null;
        String c_state = null;
        String c_zip = null;
        String c_phone = null;
        String c_since = null;
        String c_credit = null;

        int c_credit_lim = 0;
        float c_discount = 0;
        float c_balance = 0;
        String c_data = null;

        double h_amount = 0.0;

        String h_data = null;
        boolean retried = false;
        Statement stmtCust;
        Statement stmtHist;
        try {
            stmtCust = conn.createStatement();
            stmtHist = conn.createStatement();
        } catch (SQLException e1) {
            throw new RuntimeException("Customer statemet creation error", e1);
        }


        System.out.printf("Loading Customer for DID=%d, WID=%d\n", d_id, w_id);
        int currentShard = 0;
        if (shardCount > 0) {
            currentShard = (w_id % shardCount);
            if (currentShard == 0) {
                currentShard = shardCount;
            }
        }

        final String customerStub = "INSERT INTO customer (c_id, c_d_id, c_w_id, c_first, c_middle, c_last, " +
                "c_street_1, c_street_2, c_city, c_state, c_zip, c_phone, c_since, c_credit, c_credit_lim, " +
                "c_discount, c_balance, c_ytd_payment, c_payment_cnt, c_delivery_cnt, c_data) VALUES ";
        final StringBuilder customerSQL = new StringBuilder();
        int customerBatchSize = 0;
        int customerMaxBatchSize = 100;
        final Record customerRecord = new Record(21);
        
        final String historyStub = "INSERT INTO history (h_c_id, h_c_d_id, h_c_w_id, h_d_id, h_w_id, h_date, h_amount, h_data) VALUES ";
        final StringBuilder historySQL = new StringBuilder();
        int historyBatchSize = 0;
        int historyMaxBatchSize = 100;
        final Record historyRecord = new Record(8);

        if ((currentShard == shardId) || (shardId == 0)) {
            retry:
            if (retried)
                System.out.printf("Retrying ...\n");
            retried = true;
            for (c_id = 1; c_id <= CUST_PER_DIST; c_id++) {

                /* Generate Customer Data */
                c_d_id = d_id;
                c_w_id = w_id;

                c_first = Util.makeAlphaString(8, 16);
                c_middle = "O" + "E";

                if (c_id <= 1000) {
                    c_last = Util.lastName(c_id - 1);
                } else {
                    c_last = Util.lastName(Util.nuRand(255, 0, 999));
                }

                c_street_1 = Util.makeAlphaString(10, 20);
                c_street_2 = Util.makeAlphaString(10, 20);
                c_city = Util.makeAlphaString(10, 20);
                c_state = Util.makeAlphaString(2, 2);
                c_zip = Util.makeAlphaString(9, 9);

                c_phone = Util.makeNumberString(16, 16);

                if (Util.randomNumber(0, 1) == 1)
                    c_credit = "G";
                else
                    c_credit = "B";
                c_credit += "C";

                c_credit_lim = 50000;
                c_discount = (float) (((float) Util.randomNumber(0, 50)) / 100.0);
                c_balance = (float) -10.0;

                c_data = Util.makeAlphaString(300, 500);
                //gettimestamp(datetime, STRFTIME_FORMAT, TIMESTAMP_LEN); Java Equivalent below?
                Calendar calendar = Calendar.getInstance();
                Date now = calendar.getTime();
                Timestamp currentTimeStamp = new Timestamp(now.getTime());
                Date date = new java.sql.Date(calendar.getTimeInMillis());
                /*EXEC SQL INSERT INTO
                                        customer
                                        values(:c_id,:c_d_id,:c_w_id,
                                  :c_first,:c_middle,:c_last,
                                  :c_street_1,:c_street_2,:c_city,:c_state,
                                  :c_zip,
                                      :c_phone, :timestamp,
                                  :c_credit,
                                  :c_credit_lim,:c_discount,:c_balance,
                                  10.0, 1, 0,:c_data);*/
                try {


                    customerRecord.reset();
                    customerRecord.add(c_id);
                    customerRecord.add(c_d_id);
                    customerRecord.add(c_w_id);
                    customerRecord.add(c_first);
                    customerRecord.add(c_middle);
                    customerRecord.add(c_last);
                    customerRecord.add(c_street_1);
                    customerRecord.add(c_street_2);
                    customerRecord.add(c_city);
                    customerRecord.add(c_state);
                    customerRecord.add(c_zip);
                    customerRecord.add(c_phone);
                    customerRecord.add(date);
                    customerRecord.add(c_credit);
                    customerRecord.add(c_credit_lim);
                    customerRecord.add(c_discount);
                    customerRecord.add(c_balance);
                    customerRecord.add(10.0);
                    customerRecord.add(1);
                    customerRecord.add(0);
                    customerRecord.add(c_data);
                    
                    //if SQL 

                        if (customerBatchSize==0) {
                            customerSQL.append(customerStub);
                        }
                        else {
                            customerSQL.append(",");
                        }
                        customerSQL.append("(");
                        customerRecord.append(customerSQL, ",");
                        customerSQL.append(")");
                        
                        if (++customerBatchSize == customerMaxBatchSize) {
                            stmtCust.execute(customerSQL.toString());
                            customerSQL.setLength(0);
                            customerBatchSize = 0;
                        }
                    
                    //else if FILE
                    
                        //output.write(customerRecord.toString('\t'));
                    
                    //


                } catch (SQLException e) {
                    throw new RuntimeException("Customer insert error", e);
                }

                h_amount = 10.0;

                h_data = Util.makeAlphaString(12, 24);

                /*EXEC SQL INSERT INTO
                                        history
                                        values(:c_id,:c_d_id,:c_w_id,
                                       :c_d_id,:c_w_id, :timestamp,
                                       :h_amount,:h_data);*/
                try {

                    historyRecord.reset();
                    historyRecord.add(c_id);
                    historyRecord.add(c_d_id);
                    historyRecord.add(c_w_id);
                    historyRecord.add(c_d_id);
                    historyRecord.add(c_w_id);
                    historyRecord.add(date);
                    historyRecord.add(h_amount);
                    historyRecord.add(h_data);

                    if (historyBatchSize==0) {
                        historySQL.append(historyStub);
                    }
                    else {
                        historySQL.append(",");
                    }

                    historySQL.append("(");
                    historyRecord.append(historySQL, ",");
                    historySQL.append(")");

                    if (++historyBatchSize == historyMaxBatchSize) {
                        stmtHist.execute(historySQL.toString());
                        historySQL.setLength(0);
                        historyBatchSize = 0;
                    }

                } catch (SQLException e) {
                    throw new RuntimeException("Insert into History error", e);
                }
                if (optionDebug)
                    System.out.printf("CID = %d, LST = %s, P# = %s\n",
                            c_id, c_last, c_phone);
                if ((c_id % 100) == 0) {
                    System.out.printf(".");
                    if ((c_id % 1000) == 0)
                        System.out.printf(" %d\n", c_id);
                }
            }

            try {
                if (customerBatchSize>0) {
                    stmtCust.execute(customerSQL.toString());
                }
                stmtCust.close();
            
            } catch (SQLException e) {
                throw new RuntimeException("Batch execution Customer error", e);
            }

            try {
                if (historyBatchSize > 0) {
                    stmtHist.execute(historySQL.toString());
                }
                stmtHist.close();
            } catch (SQLException e) {
                throw new RuntimeException("Batch execution History error", e);
            }

        }

        System.out.printf("Customer Done.\n");

        return;
    }

    /*
      * ==================================================================+ |
      * ROUTINE NAME |      Orders | DESCRIPTION |      Loads the Orders table |
      * Also loads the Order_Line table on the fly | ARGUMENTS |      w_id -
      * warehouse id
      * +==================================================================
      */
    public static void orders(int d_id, int w_id, Connection conn, int shardCount, int shardId) {
        int o_id;
        int o_c_id;
        int o_d_id;
        int o_w_id;
        int o_carrier_id;
        int o_ol_cnt;
        int ol;
        int ol_i_id;
        int ol_supply_w_id;
        int ol_quantity;
        float ol_amount;
        String ol_dist_info;

        //TODO: shouldn't these variables be used?
//        float i_price;
//        float c_discount;

        float tmp_float;
        Statement stmt;
        try {
            stmt = conn.createStatement();
        } catch (SQLException e1) {
            throw new RuntimeException("District statement creation error", e1);
        }

        int currentShard = 0;
        if (shardCount > 0) {
            currentShard = (w_id % shardCount);
            if (currentShard == 0) {
                currentShard = shardCount;
            }
        }

        final String orderStub = "INSERT INTO orders (o_id, o_d_id, o_w_id, o_c_id, o_entry_d, o_carrier_id, o_ol_cnt, o_all_local) VALUES ";
        final StringBuilder orderSQL = new StringBuilder();
        final Record orderRecord = new Record(8);
        int orderBatchSize = 0;
        final int orderMaxBatchSize = 100;

        final String newOrderStub = "INSERT INTO new_orders (no_o_id, no_d_id, no_w_id) VALUES ";
        final StringBuilder newOrderSQL = new StringBuilder();
        final Record newOrderRecord = new Record(3);
        int newOrderBatchSize = 0;
        final int newOrderMaxBatchSize = 100;

        final String orderLineStub = "INSERT INTO order_line (ol_o_id, ol_d_id, ol_w_id, ol_number, ol_i_id, ol_supply_w_id, ol_delivery_d, ol_quantity, ol_amount, ol_dist_info) VAULES ";
        final Record orderLineRecord = new Record(10);
        final StringBuilder orderLineSQL = new StringBuilder();
        int orderLineBatchSize = 0;
        final int orderLineMaxBatchSize = 100;

        if ((currentShard == shardId) || (shardId == 0)) {
            System.out.printf("Loading Orders for D=%d, W=%d\n", d_id, w_id);
            o_d_id = d_id;
            o_w_id = w_id;
            Util.initPermutation();    /* initialize permutation of customer numbers */
            for (o_id = 1; o_id <= ORD_PER_DIST; o_id++) {

                /* Generate Order Data */
                o_c_id = Util.getPermutation();
                o_carrier_id = Util.randomNumber(1, 10);
                o_ol_cnt = Util.randomNumber(5, 15);

                //gettimestamp(datetime, STRFTIME_FORMAT, TIMESTAMP_LEN); Java Equivalent below?
                Date date = new java.sql.Date(System.currentTimeMillis());


                if (o_id > 2100) {    /* the last 900 orders have not been
							 * delivered) */
                    /*EXEC SQL INSERT INTO
                                         orders
                                         values(:o_id,:o_d_id,:o_w_id,:o_c_id,
                                        :timestamp,
                                        NULL,:o_ol_cnt, 1);*/
                    try {

                        orderRecord.reset();
                        orderRecord.add(o_id);
                        orderRecord.add(o_d_id);
                        orderRecord.add(o_w_id);
                        orderRecord.add(o_c_id);
                        orderRecord.add(date);
                        orderRecord.add("NULL");
                        orderRecord.add(o_ol_cnt);
                        orderRecord.add(1);

                        if (orderBatchSize == 0) {
                            orderSQL.append(orderStub);
                        } else {
                            orderSQL.append(",");
                        }

                        orderSQL.append("(");
                        orderRecord.append(orderSQL, ",");
                        orderSQL.append(")");

                        if (++orderBatchSize == orderMaxBatchSize) {
                            stmt.execute(orderSQL.toString());
                            orderSQL.setLength(0);
                            orderBatchSize = 0;
                        }

                    } catch (SQLException e) {
                        throw new RuntimeException("Orders insert error", e);
                    }

                    /*EXEC SQL INSERT INTO
                                         new_orders
                                         values(:o_id,:o_d_id,:o_w_id);*/
                    try {

                        newOrderRecord.reset();
                        newOrderRecord.add(o_id);
                        newOrderRecord.add(o_d_id);
                        newOrderRecord.add(o_w_id);

                        if (newOrderBatchSize == 0) {
                            newOrderSQL.append(newOrderStub);
                        } else {
                            newOrderSQL.append(',');
                        }

                        newOrderSQL.append('(');
                        newOrderRecord.append(newOrderSQL, ",");
                        newOrderSQL.append(')');

                        if (++newOrderBatchSize == newOrderMaxBatchSize) {
                            stmt.execute(newOrderSQL.toString());
                            newOrderSQL.setLength(0);
                            newOrderBatchSize = 0;
                        }

                    } catch (SQLException e) {
                        throw new RuntimeException("New Orders insert error", e);
                    }
                } else {
                    /*EXEC SQL INSERT INTO
                             orders
                             values(:o_id,:o_d_id,:o_w_id,:o_c_id,
                                :timestamp,
                                :o_carrier_id,:o_ol_cnt, 1);*/
                    try {

                        orderRecord.reset();
                        orderRecord.add(o_id);
                        orderRecord.add(o_d_id);
                        orderRecord.add(o_w_id);
                        orderRecord.add(o_c_id);
                        orderRecord.add(date);
                        orderRecord.add(o_carrier_id);
                        orderRecord.add(o_ol_cnt);
                        orderRecord.add(1);

                        if (orderBatchSize == 0) {
                            orderSQL.append(orderStub);
                        } else {
                            orderSQL.append(",");
                        }

                        orderSQL.append("(");
                        orderRecord.append(orderSQL, ",");
                        orderSQL.append(")");

                        if (++orderBatchSize == orderMaxBatchSize) {
                            stmt.execute(orderSQL.toString());
                            orderSQL.setLength(0);
                            orderBatchSize = 0;
                        }

                    } catch (SQLException e) {
                        throw new RuntimeException("Orders insert error", e);
                    }

                }


                if (optionDebug)
                    System.out.printf("OID = %d, CID = %d, DID = %d, WID = %d\n",
                            o_id, o_c_id, o_d_id, o_w_id);

                for (ol = 1; ol <= o_ol_cnt; ol++) {
                    /* Generate Order Line Data */
                    ol_i_id = Util.randomNumber(1, MAXITEMS);
                    ol_supply_w_id = o_w_id;
                    ol_quantity = 5;
                    ol_amount = (float) 0.0;

                    ol_dist_info = Util.makeAlphaString(24, 24);

                    tmp_float = (float) ((float) (Util.randomNumber(10, 10000)) / 100.0);

                    if (o_id > 2100) {
                        /*EXEC SQL INSERT INTO
                                              order_line
                                              values(:o_id,:o_d_id,:o_w_id,:ol,
                                             :ol_i_id,:ol_supply_w_id, NULL,
                                             :ol_quantity,:ol_amount,:ol_dist_info);*/
                        try {

                            orderLineRecord.reset();
                            orderLineRecord.add(o_id);
                            orderLineRecord.add(o_d_id);
                            orderLineRecord.add(o_w_id);
                            orderLineRecord.add(ol);
                            orderLineRecord.add(ol_i_id);
                            orderLineRecord.add(ol_supply_w_id);
                            orderLineRecord.add("NULL");
                            orderLineRecord.add(ol_quantity);
                            orderLineRecord.add(ol_amount);
                            orderLineRecord.add(ol_dist_info);

                            if (orderLineBatchSize == 0) {
                                orderLineSQL.append(orderLineStub);
                            } else {
                                orderLineSQL.append(",");
                            }

                            orderLineSQL.append("(");
                            orderLineRecord.append(orderLineSQL, ",");
                            orderLineSQL.append(")");

                            if (++orderLineBatchSize == orderLineMaxBatchSize) {
                                stmt.execute(orderLineSQL.toString());
                                orderLineSQL.setLength(0);
                                orderLineBatchSize = 0;
                            }


                        } catch (SQLException e) {
                            throw new RuntimeException("Order line insert error", e);
                        }
                    } else {
                        /*EXEC SQL INSERT INTO
                                  order_line
                                  values(:o_id,:o_d_id,:o_w_id,:ol,
                                     :ol_i_id,:ol_supply_w_id, 
                                     :timestamp,
                                     :ol_quantity,:tmp_float,:ol_dist_info);*/
                        try {

                            orderLineRecord.reset();
                            orderLineRecord.add(o_id);
                            orderLineRecord.add(o_d_id);
                            orderLineRecord.add(o_w_id);
                            orderLineRecord.add(ol);
                            orderLineRecord.add(ol_i_id);
                            orderLineRecord.add(ol_supply_w_id);
                            orderLineRecord.add(date);
                            orderLineRecord.add(ol_quantity);
                            orderLineRecord.add(tmp_float);
                            orderLineRecord.add(ol_dist_info);

                            if (orderLineBatchSize == 0) {
                                orderLineSQL.append(orderLineStub);
                            } else {
                                orderLineSQL.append(",");
                            }

                            orderLineSQL.append("(");
                            orderLineRecord.append(orderLineSQL, ",");
                            orderLineSQL.append(")");

                            if (++orderLineBatchSize == orderLineMaxBatchSize) {
                                stmt.execute(orderLineSQL.toString());
                                orderLineSQL.setLength(0);
                                orderLineBatchSize = 0;
                            }


                        } catch (SQLException e) {
                            throw new RuntimeException("Order line insert error", e);
                        }
                    }

                    if (optionDebug)
                        System.out.printf("OL = %d, IID = %d, QUAN = %d, AMT = %f\n",
                                ol, ol_i_id, ol_quantity, ol_amount);

                }
                if ((o_id % 100) == 0) {
                    System.out.printf(".");

                    if ((o_id % 1000) == 0)
                        System.out.printf(" %d\n", o_id);
                }
            }

            try {
                if (orderBatchSize > 0) {
                    stmt.execute(orderSQL.toString());
                }
                if (newOrderBatchSize > 0) {
                    stmt.execute(newOrderSQL.toString());
                }
                if (orderLineBatchSize > 0) {
                    stmt.execute(orderLineSQL.toString());
                }
                stmt.close();

            } catch (SQLException e) {
                throw new RuntimeException("Order batch execute error", e);
            }
        }


        System.out.printf("Orders Done.\n");
    }


}
