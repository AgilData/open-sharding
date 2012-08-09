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
#define PORT 3306

bool show_results = 0;

/*Global Variables for Thread Test*/
std::string table="test1";
unsigned int numThreads=10;
unsigned int loopCount=10;
std::string limit="1";

using namespace std;

void * run(void* data){

    int thread_no = *((int *)data);

    cout<<"Thread " << thread_no << " started."<<endl;
    clock_t start = clock();

    MYSQL* mysql=mysql_init(NULL);
    if(!mysql){
        cout<<"Thread " << thread_no << " terminated - mysql_init() failed"<<endl;
        return NULL;
    }

    mysql = mysql_real_connect(mysql,SERVER,USER,PASSWORD,DATABASE,PORT,NULL,0);
    if(!mysql){
        cout<<"Thread " << thread_no << " mysql_real_connect() FAILED"<<endl;
        cout<<mysql_errno(mysql)<<" "<<mysql_error(mysql)<<endl;
        return NULL;
    }

    MYSQL_RES *res_set;
    MYSQL_ROW row;
    unsigned int    numFields;
            
    for (int j=0; j<loopCount; j++){

        cout<<"Thread " << thread_no <<" BEFORE query #" << j <<endl;

        //string sql = "SELECT * FROM item LIMIT " + limit;
        string sql = "SELECT * FROM " +  table + " LIMIT " + limit;
        if (mysql_query(mysql,sql.c_str())) {
            cout<<"Thread " << thread_no << " terminated - mysql_query() failed"<<endl;
            cout<<mysql_errno(mysql)<<" "<<mysql_error(mysql)<<endl;
            return NULL;
        }

        res_set = mysql_store_result(mysql);
        if (!res_set) {
            cout<<"Thread " << thread_no << " terminated - mysql_store_result() failed"<<endl;
            cout<<mysql_errno(mysql)<<" "<<mysql_error(mysql)<<endl;
            return NULL;
        }
            
        numFields = mysql_num_fields(res_set);
        while ((row = mysql_fetch_row(res_set)) != NULL){
            if (show_results) {
                for(int columnIndex=0; columnIndex < numFields; columnIndex++){
                    cout<<"["<<row[columnIndex]<<"] ";
                }
                cout << endl;
            }
        }

        mysql_free_result(res_set);

        cout<<"Thread " << thread_no <<" AFTER query #" << j <<endl;
   }
            
    mysql_close(mysql);   /* Close and shutdown connection*/

    cout<<"Thread " << thread_no <<" finished"<<endl;

    clock_t end = clock();
    clock_t total = end - start;
    cout<<"Thread " << thread_no <<" execution time: "<<total<<endl;

    return NULL;
}

int main(int argc, const char * argv[])
{
    if (numThreads==1) {

        int thread_no = 1;
        run(&thread_no);

    }
    else {

        pthread_t thread[numThreads];

        // create threads
        for (int i=0; i<numThreads; ++i){
            int *threadNo = new int[1];
             *threadNo = i+1;
            if(pthread_create(&thread[i], NULL, &run, (void*)threadNo)){
                cout<<"Failed to create thread"<<endl;
                return -1;
            }
        }

        // join threads (wait for threads to terminate)
        cout << "Waiting for threads to terminate"<<endl;
        for (int i=0; i<numThreads; ++i){
            cout << "Waiting to join thread " << (i+1) << endl;
            pthread_join(thread[i], NULL);
        }
        cout << "All threads have terminated"<<endl;
    }

    return 0;
}

