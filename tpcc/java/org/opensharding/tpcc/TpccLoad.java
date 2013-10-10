package org.opensharding.tpcc;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.text.DecimalFormat;
import java.util.Properties;

import org.slf4j.LoggerFactory;
import org.slf4j.Logger;


public class TpccLoad implements TpccConstants {
    private String connectString;
    private String dbString;
    private String dbUser = null;
    private String dbPassword = null;
    private int port = 3306;
    private int shardCount = 0;
    private String jdbcUrl = null;
    private String javaDriver = null;
    private int shardId = -1;


    /* Global SQL Variables */
    static int num_ware = 0;
    static int fd = 0;
    static int seed = 0;


    int particle_flg = 0; /* "1" means particle mode */
    int part_no = 0; /* 1:items 2:warehouse 3:customer 4:orders */
    long min_ware = 1;
    long max_ware;

    /* Global Variables */
    static int i = 0;
    static int is_local = 1;           /* "1" mean local */
    static int DB_STRING_MAX = 51;
    static boolean option_debug = false;	/* 1 if generating debug output    */

    private static final Logger logger = LoggerFactory.getLogger(Tpcc.class);
    private static final boolean DEBUG = logger.isDebugEnabled();

    private static final String DRIVER = "DRIVER";
    private static final String WAREHOUSECOUNT = "WAREHOUSECOUNT";
    private static final String HOST = "HOST";
    private static final String DATABASE = "DATABASE";
    private static final String USER = "USER";
    private static final String PASSWORD = "PASSWORD";
    private static final String SHARDCOUNT = "SHARDCOUNT";
    private static final String JDBCURL = "JDBCURL";
    private static final String SHARDID = "SHARDID";

    private Properties properties;
    private InputStream inputStream;

    private static final String PROPERTIESFILE = "tpcc.properties";

    public TpccLoad() {
        // Empty.
    }

    private void init() {

        logger.info("Loading properties from: " + PROPERTIESFILE);

        try {
            properties = new Properties();
            inputStream = new FileInputStream(PROPERTIESFILE);
            properties.load(inputStream);
        } catch (IOException e) {
            throw new RuntimeException("Error loading properties file", e);
        }

    }


    private int runLoad(boolean overridePropertiesFile, String[] argv) {

        if (overridePropertiesFile) {
            for (int i = 0; i < argv.length; i = i + 2) {
                if (argv[i].equals("-h")) {
                    connectString = argv[i + 1];
                } else if (argv[i].equals("-d")) {
                    dbString = argv[i + 1];
                } else if (argv[i].equals("-u")) {
                    dbUser = argv[i + 1];
                } else if (argv[i].equals("-p")) {
                    dbPassword = argv[i + 1];
                } else if (argv[i].equals("-j")) {
                    javaDriver = argv[i + 1];
                } else if (argv[i].equals("-l")) {
                    jdbcUrl = argv[i + 1];
                } else if (argv[i].equals("-s")) {
                    shardCount = Integer.parseInt(argv[i + 1]);
                } else if (argv[i].equals("-i")) {
                    shardId = Integer.parseInt(argv[i + 1]);
                } else {
                    System.out.println("Incorrect Argument: " + argv[i]);
                    System.out.println("The possible arguments are as follows: ");
                    System.out.println("-h [database host]");
                    System.out.println("-d [database name]");
                    System.out.println("-u [database username]");
                    System.out.println("-p [database password]");
                    System.out.println("-w [number of warehouses]");
                    System.out.println("-j [java driver]");
                    System.out.println("-l [jdbc url]");
                    System.out.println("-s [shard count]");
                    System.out.println("-i [shard id]");
                    System.exit(-1);

                }
            }
        } else {
            connectString = properties.getProperty(HOST);
            dbString = properties.getProperty(DATABASE);
            dbUser = properties.getProperty(USER);
            dbPassword = properties.getProperty(PASSWORD);
            num_ware = Integer.parseInt(properties.getProperty(WAREHOUSECOUNT));
            shardCount = Integer.parseInt(properties.getProperty(SHARDCOUNT));
            javaDriver = properties.getProperty(DRIVER);
            jdbcUrl = properties.getProperty(JDBCURL);
            shardId = Integer.parseInt(properties.getProperty(SHARDID));
        }

        System.out.printf("*************************************\n");
        System.out.printf("*** Java TPC-C Data Loader version " + Tpcc.VERSION + " ***\n");
        System.out.printf("*************************************\n");

        long start = System.currentTimeMillis();
        System.out.println("Execution time start: " + start);


        if (connectString == null) {
            throw new RuntimeException("Host is null.");
        }
        if (dbString == null) {
            throw new RuntimeException("Database name is null.");
        }
        if (dbUser == null) {
            throw new RuntimeException("User is null.");
        }
        if (dbPassword == null) {
            throw new RuntimeException("Password is null.");
        }
        if (num_ware < 1) {
            throw new RuntimeException("Warehouse count has to be greater than or equal to 1.");
        }
        if (javaDriver == null) {
            throw new RuntimeException("Java Driver is null.");
        }
        if (jdbcUrl == null) {
            throw new RuntimeException("JDBC Url is null.");
        }
        if (shardId == -1) {
            throw new RuntimeException("ShardId was not obtained");
        }


        System.out.printf("<Parameters>\n");
        if (is_local == 0) System.out.printf("     [server]: %s\n", connectString);
        if (is_local == 0) System.out.printf("     [port]: %d\n", port);
        System.out.printf("     [DBname]: %s\n", dbString);
        System.out.printf("       [user]: %s\n", dbUser);
        System.out.printf("       [pass]: %s\n", dbPassword);

        System.out.printf("  [warehouse]: %d\n", num_ware);
        System.out.printf(" [shardId]: %d\n", shardId);
        if (particle_flg == 1) {
            System.out.printf("  [part(1-4)]: %d\n", part_no);
            System.out.printf("     [MIN WH]: %d\n", min_ware);
            System.out.printf("     [MAX WH]: %d\n", max_ware);
        }

        //TODO: Pass the seed in as a variable.
        Util.setSeed(seed);


		/* EXEC SQL WHENEVER SQLERROR GOTO Error_SqlCall; */
        try {
            Class.forName(javaDriver);
        } catch (ClassNotFoundException e1) {
            throw new RuntimeException("Class for mysql error", e1);
        }

        Connection conn;

        try {
            conn = DriverManager.getConnection(jdbcUrl, dbUser, dbPassword);
        } catch (SQLException e) {
            throw new RuntimeException("Connection error", e);
        }


        Statement stmt;
        try {
            stmt = conn.createStatement();
        } catch (SQLException e) {
            throw new RuntimeException("Could not create statement", e);
        }
        try {
            stmt.execute("SET UNIQUE_CHECKS=0");
        } catch (SQLException e) {
            throw new RuntimeException("Could not set unique checks error", e);
        }
        try {
            stmt.execute("SET FOREIGN_KEY_CHECKS=0");
            stmt.close();
        } catch (SQLException e) {
            throw new RuntimeException("Could not set foreign key checks error", e);
        }


        System.out.printf("TPCC Data Load Started...\n");
        max_ware = num_ware;
        if (particle_flg == 0) {
            System.out.printf("Particle flag: %d\n", particle_flg);
            Load.loadItems(conn, shardCount, option_debug);
            Load.loadWare(conn, shardCount, (int) min_ware, (int) max_ware, option_debug, shardId);
            Load.loadCust(conn, shardCount, (int) min_ware, (int) max_ware, shardId);
            Load.loadOrd(conn, shardCount, (int) max_ware, shardId);
        } else if (particle_flg == 1) {
            switch (part_no) {
                case 1:
                    Load.loadItems(conn, shardCount, option_debug);
                    break;
                case 2:
                    Load.loadWare(conn, shardCount, (int) min_ware, (int) max_ware, option_debug, shardId);
                    break;
                case 3:
                    Load.loadCust(conn, shardCount, (int) min_ware, (int) max_ware, shardId);
                    break;
                case 4:
                    Load.loadOrd(conn, shardCount, (int) max_ware, shardId);
                    break;
                default:
                    System.out.printf("Unknown part_no\n");
                    System.out.printf("1:ITEMS 2:WAREHOUSE 3:CUSTOMER 4:ORDERS\n");
            }
        }

        System.out.printf("\n...DATA LOADING COMPLETED SUCCESSFULLY.\n");

        long end = System.currentTimeMillis();
        System.out.println("Total execution time: " + (double) ((((double) end - (double) start) / 1000.0)) / 60.0 + " Min");
        return 0;

    }

    public static void main(String[] argv) {

        // dump information about the environment we are running in
        String sysProp[] = {
                "os.name",
                "os.arch",
                "os.version",
                "java.runtime.name",
                "java.vm.version",
                "java.library.path"
        };

        for (String s : sysProp) {
            logger.info("System Property: " + s + " = " + System.getProperty(s));
        }

        DecimalFormat df = new DecimalFormat("#,##0.0");
        System.out.println("maxMemory = " + df.format(Runtime.getRuntime().totalMemory() / (1024.0 * 1024.0)) + " MB");
        TpccLoad tpccLoad = new TpccLoad();

        int ret = 0;
        if (argv.length == 0) {
            System.out.println("Using the tpcc.properties file for the load configuration.");
            tpccLoad.init();
            ret = tpccLoad.runLoad(false, argv);
        } else {

            if ((argv.length % 2) == 0) {
                System.out.println("Using the command line arguments for the load configuration.");
                ret = tpccLoad.runLoad(true, argv);
            } else {
                System.out.println("Invalid number of arguments.");
                System.out.println("Incorrect Argument: " + argv[i]);
                System.out.println("The possible arguments are as follows: ");
                System.out.println("-h [database host]");
                System.out.println("-d [database name]");
                System.out.println("-u [database username]");
                System.out.println("-p [database password]");
                System.out.println("-w [number of warehouses]");
                System.out.println("-j [java driver]");
                System.out.println("-l [jdbc url]");
                System.out.println("-s [shard count]");
                System.out.println("-i [shard id]");
                System.exit(-1);

            }
        }

        System.exit(ret);
    }


}
