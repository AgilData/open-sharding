//
//  main.cpp
//  MyOSP-Benchmarking
//
//  Created by Jerry Ramey on 7/30/12.
//  Copyright (c) 2012 DBShards. All rights reserved.
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


/*Iterator*/
int i;

/*Mysql Connector*/
MYSQL *connect;

/*pthread mutex*/
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;


using namespace std;

void * run(void*){
    cout<<"Entering Function."<<endl;
    
    connect=mysql_init(NULL); 
    if(!connect){
        fprintf(stderr,"MySQL Initialization Failed");
        exit(1);
    }
    else{
        connect=mysql_real_connect(connect,SERVER,USER,PASSWORD,DATABASE,0,NULL,0);
        
        if(connect){
            printf("Connection Succeeded\n");
            MYSQL_RES *res_set; 
            MYSQL_ROW row;
            unsigned int    numFields;
            
            for (int j=0; i<loopCount; j++){
                //string sql = "SELECT * FROM item LIMIT " + limit;
                string sql = "SELECT * FROM " +  table + " LIMIT " + limit;
                mysql_query(connect,sql.c_str());
                //cout<<sql<<endl;
                
            }
            
            
            
            res_set = mysql_store_result(connect);
            
            while ((row = mysql_fetch_row(res_set)) != NULL){
                numFields = mysql_num_fields(res_set);
                //printf("%s\n",row[i] != NULL ?
                //row[i] : "NULL"); /* Print the row data */
                for(int ii=0; ii < numFields; ii++){
                    //printf("%s\t", mysqlRow[ii] ? mysqlRow[ii] : "NULL");  // Not NULL then print
                }
            }
            
            mysql_close(connect);   /* Close and shutdown connection*/
        }
        else{
            printf("Connection Failed!\n");
            exit(2);
        }

    }
    
}

int main(int argc, const char * argv[])
{
    clock_t start = clock();
    
    pthread_t thread[numThreads];
    cout<<"Check 1"<<endl;
  
    for(i=0; i<numThreads; ++i){
        cout<<"For loop iteration "<<i<<endl;
        pthread_mutex_lock( &mutex1 );
        if(pthread_create(&thread[i], NULL, &run, (void*)i)){
            cout<<"Could not create the thread."<<endl;
            return -1;
        }else {
            cout<<" Thread created"<<endl;
        }
        pthread_mutex_unlock( &mutex1 );

        
    }
    
    clock_t end = clock();
    clock_t total = end - start;
    std::cout<<"Execution time of this writing benchmark "<<total<<"\n";
  
    return 0;
}

