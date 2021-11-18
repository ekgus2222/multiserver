#include "/usr/include/mysql/mysql.h"
#include <stdio.h>
#include <string.h>


void finish_with_error(MYSQL *con);
int run(char* filename, MYSQL *con);