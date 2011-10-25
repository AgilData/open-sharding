#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>

int main() {
  printf("TEST\n");
  MYSQL my;
  mysql_init(&my);
  if (!mysql_real_connect(&my, "localhost", "baduser", "badpass", "test", 3306, NULL, 0)) {
    printf("mysql_real_connect() FAILED: %d %s\n", mysql_errno(&my), mysql_error(&my));
    return -1;
  }
}
