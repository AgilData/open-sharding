tpcc
====

This project is a Java implementation of tpcc using JDBC and prepared statements.

Configuration settings include database connection details are in tpcc.properties.

To create the tpcc schema in MySQL:

cd database
mysql -u root
> source create_tables.sql
> source add_fkey_idx.sql

To load the initial warehouse data, run "ant tpccload".

To run the tpcc benchmarks, run "ant tpccrun".

Bugs can be reported to support@codefutures.com.

(c) 2013 CodeFutures Corporation.
