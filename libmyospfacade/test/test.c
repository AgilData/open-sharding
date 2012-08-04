#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

void run_query(MYSQL *my) {

  const char *sql = "SELECT * FROM country";
  
  if (mysql_real_query(my, sql, strlen(sql))) {
    printf("mysql_real_query() FAILED: %d %s\n", mysql_errno(my), mysql_error(my));
    return;
  }

  printf("Query ran OK\n");
}

int main() {
  printf("TEST\n");
  MYSQL my;
  mysql_init(&my);

  if (!mysql_real_connect(&my, "localhost", "root", "", "dbs_junit_1a", 3306, NULL, 0)) {
    printf("mysql_real_connect() FAILED: %d %s\n", mysql_errno(&my), mysql_error(&my));
    return -1;
  }

  run_query(&my);

  if (mysql_select_db(&my, "dbs_junit_1b")) {
    printf("mysql_select_db() FAILED: %d %s\n", mysql_errno(&my), mysql_error(&my));
    return -1;
  }

  run_query(&my);

  printf("TEST END\n");
}
