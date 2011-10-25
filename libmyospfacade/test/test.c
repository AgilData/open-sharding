#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>

int main() {
  printf("TEST\n");
  MYSQL my;
  mysql_init(&my);

  if (!mysql_real_connect(&my, "localhost", "root", "", "dbs_junit_1a", 3306, NULL, 0)) {
    printf("mysql_real_connect() FAILED: %d %s\n", mysql_errno(&my), mysql_error(&my));
    return -1;
  }

  if (mysql_select_db(&my, "dbs_junit_1b")) {
    printf("mysql_select_db() FAILED: %d %s\n", mysql_errno(&my), mysql_error(&my));
    return -1;
  }

  printf("TEST END\n");
}
