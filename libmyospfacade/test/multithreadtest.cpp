//
//  multithreadtest.cpp
//  MyOSP-Benchmarking
//
//  Created by Jerry Ramey on 7/30/12.
//  Copyright (c) 2012 CodeFutures Corporation. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <mysql.h>
#include <string>
#include <time.h>
#include <sstream>
#include <pthread.h>

#include <iomanip>
#include <locale>

#define SERVER "localhost"
#define USER ""
#define PASSWORD ""
#define DATABASE "test"

/*Global Variables for Thread Test*/
std::string table="test1";
unsigned int numThreads=10;
unsigned int loopCount=10;
std::string limit="1";

using namespace std;

void * run(void*){

    cout<<"Thread started."<<endl;
    clock_t start = clock();

    MYSQL* connect=mysql_init(NULL);
    if(!connect){
        cout<<"Thread terminated - mysql_init() failed"<<endl;
        return;
    }

    connect = mysql_real_connect(connect,SERVER,USER,PASSWORD,DATABASE,0,NULL,0);
        
    if(!connect){
        cout<<"mysql_real_connect() FAILED"<<endl;
        return NULL;
    }

    MYSQL_RES *res_set;
    MYSQL_ROW row;
    unsigned int    numFields;
            
    for (int j=0; i<loopCount; j++){
        //string sql = "SELECT * FROM item LIMIT " + limit;
        string sql = "SELECT * FROM " +  table + " LIMIT " + limit;
        if (mysql_query(connect,sql.c_str())) {
            cout<<"Thread terminated - mysql_query() failed"<<endl;
            return NULL;
        }

        res_set = mysql_store_result(connect);
        if (!res_set) {
            cout<<"Thread terminated - mysql_store_result() failed"<<endl;
            return NULL;
        }
            
        numFields = mysql_num_fields(res_set);
        cout<<"result set has " << numField << "columns" << endl;
        while ((row = mysql_fetch_row(res_set)) != NULL){
            for(int columnIndex=0; columnIndex < numFields; columnIndex++){
                cout<<"["<<row[columnIndex]<<"] ";
            }
            cout << endl;
        }

        mysql_free_result(res_set);
    }
            
    mysql_close(connect);   /* Close and shutdown connection*/

    cout<<"Thread finished"<<endl;

    clock_t end = clock();
    clock_t total = end - start;
    cout<<"Execution time of this thread: "<<total<<endl;

    return NULL;
}

int main(int argc, const char * argv[])
{

    pthread_t thread[numThreads];
    cout<<"Check 1"<<endl;
  
    for(i=0; i<numThreads; ++i){
        cout<<"For loop iteration "<<i<<endl;
        if(pthread_create(&thread[i], NULL, &run, (void*)i)){
            cout<<"Could not create the thread."<<endl;
            return -1;
        }else {
            cout<<" Thread created"<<endl;
        }
    }

    return 0;
}
