

/* Copyright (C) 2010 CodeFutures Corporation

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

// if we want to support all mysql symbols we need to *at least* import these headers
#include <my_global.h>
#include <m_ctype.h>
#include <m_string.h>


#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <map>


#include <mysql.h>
#include <errmsg.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>

#include <typelib.h>

#include <mysql/BuildInfo.h>
#include <mysql/MySQLClient.h>
#include <mysql/MySQLAbstractConnection.h>
#include <mysql/MySQLOSPConnection.h>
#include <mysql/MySQLNativeConnection.h>
#include <opensharding/OSPConnection.h>
#include <opensharding/OSPTCPConnection.h>
#include <opensharding/OSPNamedPipeConnection.h>
#include <opensharding/OSPUnixSocketConnection.h>
#include <opensharding/OSPWireRequest.h>
#include <opensharding/OSPConnectRequest.h>
#include <opensharding/OSPWireResponse.h>
#include <opensharding/OSPPingRequest.h>
#include <opensharding/OSPPingResponse.h>
#include <opensharding/OSPConnectResponse.h>
#include <opensharding/OSPErrorResponse.h>
#include <logger/Logger.h>
#include <myutil/MyOSPLogger.h>
#include <myutil/MyOSPConfig.h>
#include <util/Util.h>
#include <util/MutexLock.h>

#include <mysql/MySQLDriver.h>
#include <mysql/MySQLAbstractConnection.h>
#include <mysql/MySQLConnMap.h>
#include <mysql/MySQLErrorState.h>

using namespace mysql;
using namespace logger;
using namespace std;
using namespace util;
using namespace opensharding;

extern TYPELIB sql_protocol_typelib;

/* CONSTANTS */

static int PROTOCOL_TCP = 1;
static int PROTOCOL_PIPES = 2;
static int PROTOCOL_UNIX_SOCKET = 3;

/* GLOBAL VARIABLES */

static Logger &xlog = MyOSPLogger::getLogger("MySQLDriver");
static unsigned int Pid = 0;

static bool bannerDisplayed = false;


/* map for mysql structure that we created in mysql_init so we can delete them in mysql_close */
static map<MYSQL*, bool> *mysqlAllocMap = new map<MYSQL*, bool>();


/* Mapping of MYSQL structues to wrapper structures */
static MySQLConnMap *_mysqlResourceMap = NULL;

/* Temporary global variable for next named pipe number for this process */
static int nextPipeNo = 1;

static pthread_mutex_t MySQLDriver_init_mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t MySQLDriver_resource_mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t MySQLDriver_connpool_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Wrapper around libmysqlclient.so */
static MySQLClient *mysqlclient = NULL;


const char *client_errors[]=
{
  "Unknown MySQL error",
  "Can't create UNIX socket (%d)",
  "Can't connect to local MySQL server through socket '%-.100s' (%d)",
  "Can't connect to MySQL server on '%-.100s' (%d)",
  "Can't create TCP/IP socket (%d)",
  "Unknown MySQL server host '%-.100s' (%d)",
  "MySQL server has gone away",
  "Protocol mismatch; server version = %d, client version = %d",
  "MySQL client ran out of memory",
  "Wrong host info",
  "Localhost via UNIX socket",
  "%-.100s via TCP/IP",
  "Error in server handshake",
  "Lost connection to MySQL server during query",
  "Commands out of sync; you can't run this command now",
  "Named pipe: %-.32s",
  "Can't wait for named pipe to host: %-.64s  pipe: %-.32s (%lu)",
  "Can't open named pipe to host: %-.64s  pipe: %-.32s (%lu)",
  "Can't set state of named pipe to host: %-.64s  pipe: %-.32s (%lu)",
  "Can't initialize character set %-.32s (path: %-.100s)",
  "Got packet bigger than 'max_allowed_packet' bytes",
  "Embedded server",
  "Error on SHOW SLAVE STATUS:",
  "Error on SHOW SLAVE HOSTS:",
  "Error connecting to slave:",
  "Error connecting to master:",
  "SSL connection error",
  "Malformed packet",
  "This client library is licensed only for use with MySQL servers having '%s' license",
  "Invalid use of null pointer",
  "Statement not prepared",
  "No data supplied for parameters in prepared statement",
  "Data truncated",
  "No parameters exist in the statement",
  "Invalid parameter number",
  "Can't send long data for non-string/non-binary data types (parameter: %d)",
  "Using unsupported buffer type: %d  (parameter: %d)",
  "Shared memory: %-.100s",
  "Can't open shared memory; client could not create request event (%lu)",
  "Can't open shared memory; no answer event received from server (%lu)",
  "Can't open shared memory; server could not allocate file mapping (%lu)",
  "Can't open shared memory; server could not get pointer to file mapping (%lu)",
  "Can't open shared memory; client could not allocate file mapping (%lu)",
  "Can't open shared memory; client could not get pointer to file mapping (%lu)",
  "Can't open shared memory; client could not create %s event (%lu)",
  "Can't open shared memory; no answer from server (%lu)",
  "Can't open shared memory; cannot send request event to server (%lu)",
  "Wrong or unknown protocol",
  "Invalid connection handle",
  "Connection using old (pre-4.1.1) authentication protocol refused (client option 'secure_auth' enabled)",
  "Row retrieval was canceled by mysql_stmt_close() call",
  "Attempt to read column without prior row fetch",
  "Prepared statement contains no metadata",
  "Attempt to read a row while there is no result set associated with the statement",
  "This feature is not implemented yet",
  "Lost connection to MySQL server at '%s', system error: %d",
  "Statement closed indirectly because of a preceeding %s() call",
  "The number of columns in the result set differs from the number of bound buffers. You must reset the statement, rebind the result set columns, and execute the statement again",
  ""
};

//QUESTIONABLE Functions that need to be evaluated more.//
#ifdef strmov
#undef strmov
#define strmov strmov_overlapp
#endif

#if !defined(MC68000) && !defined(DS90)

char *strmov(register char *dst, register const char *src)
{
  while ((*dst++ = *src++)) ;
  return dst-1;
}

#else

char *strmov(dst, src)
     char *dst, *src;
{
  asm("   movl  4(a7),a1  ");
  asm("   movl  8(a7),a0  ");
  asm(".L4: movb  (a0)+,(a1)+ ");
  asm("   jne .L4   ");
  asm("   movl  a1,d0   ");
  asm("   subql #1,d0   ");
}

#endif


void __attribute__ ((constructor)) MySQLDriver_init(void) {
  if (xlog.isDebugEnabled()) xlog.debug("Inializing the driver.");

}

void __attribute__ ((destructor)) MySQLDriver_fini(void) {
  if (xlog.isDebugEnabled()) xlog.debug("Termination Complete NEED TO ADD MYSQL CLOSE HERE.");
 //mysql_close();
}


const char *sql_protocol_names_lib[] = { "TCP", "SOCKET", "PIPE", "MEMORY", NullS };
TYPELIB sql_protocol_typelib = {array_elements(sql_protocol_names_lib)-1,"",
        sql_protocol_names_lib, NULL};

///ADD latin1 
        
        /* Copyright (C) 2000 MySQL AB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */


static uchar ctype_latin1[] = {
    0,
   32, 32, 32, 32, 32, 32, 32, 32, 32, 40, 40, 40, 40, 40, 32, 32,
   32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
   72, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
  132,132,132,132,132,132,132,132,132,132, 16, 16, 16, 16, 16, 16,
   16,129,129,129,129,129,129,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 16, 16, 16, 16, 16,
   16,130,130,130,130,130,130,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 16, 16, 16, 16, 32,
   16,  0, 16,  2, 16, 16, 16, 16, 16, 16,  1, 16,  1,  0,  1,  0,
    0, 16, 16, 16, 16, 16, 16, 16, 16, 16,  2, 16,  2,  0,  2,  1,
   72, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
   16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1, 16,  1,  1,  1,  1,  1,  1,  1,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2, 16,  2,  2,  2,  2,  2,  2,  2,  2
};

static uchar to_lower_latin1[] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
   16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
   32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
   48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
   64, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122, 91, 92, 93, 94, 95,
   96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
  128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
  224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
  240,241,242,243,244,245,246,215,248,249,250,251,252,253,254,223,
  224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
  240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255
};

static uchar to_upper_latin1[] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
   16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
   32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
   48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
   64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
   96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,123,124,125,126,127,
  128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
  192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
  208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
  192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
  208,209,210,211,212,213,214,247,216,217,218,219,220,221,222,255
};

static uchar sort_order_latin1[] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
   16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
   32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
   48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
   64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
   96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,123,124,125,126,127,
  128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
   65, 65, 65, 65, 92, 91, 92, 67, 69, 69, 69, 69, 73, 73, 73, 73,
   68, 78, 79, 79, 79, 79, 93,215,216, 85, 85, 85, 89, 89,222,223,
   65, 65, 65, 65, 92, 91, 92, 67, 69, 69, 69, 69, 73, 73, 73, 73,
   68, 78, 79, 79, 79, 79, 93,247,216, 85, 85, 85, 89, 89,222,255
};

/*
  WL#1494 notes:
  
  We'll use cp1252 instead of iso-8859-1.
  cp1252 contains printable characters in the range 0x80-0x9F.
  In ISO 8859-1, these code points have no associated printable
  characters. Therefore, by converting from CP1252 to ISO 8859-1,
  one would lose the euro (for instance). Since most people are
  unaware of the difference, and since we don't really want a
  "Windows ANSI" to differ from a "Unix ANSI", we will:
   
   - continue to pretend the latin1 character set is ISO 8859-1
   - actually allow the storage of euro etc. so it's actually cp1252

  Also we'll map these five undefined cp1252 character:
    0x81, 0x8D, 0x8F, 0x90, 0x9D
  into corresponding control characters:
     U+0081, U+008D, U+008F, U+0090, U+009D.
  like ISO-8859-1 does. Otherwise, loading "mysqldump"
  output doesn't reproduce these undefined characters.
*/

unsigned short cs_to_uni[256]={
0x0000,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x0007,
0x0008,0x0009,0x000A,0x000B,0x000C,0x000D,0x000E,0x000F,
0x0010,0x0011,0x0012,0x0013,0x0014,0x0015,0x0016,0x0017,
0x0018,0x0019,0x001A,0x001B,0x001C,0x001D,0x001E,0x001F,
0x0020,0x0021,0x0022,0x0023,0x0024,0x0025,0x0026,0x0027,
0x0028,0x0029,0x002A,0x002B,0x002C,0x002D,0x002E,0x002F,
0x0030,0x0031,0x0032,0x0033,0x0034,0x0035,0x0036,0x0037,
0x0038,0x0039,0x003A,0x003B,0x003C,0x003D,0x003E,0x003F,
0x0040,0x0041,0x0042,0x0043,0x0044,0x0045,0x0046,0x0047,
0x0048,0x0049,0x004A,0x004B,0x004C,0x004D,0x004E,0x004F,
0x0050,0x0051,0x0052,0x0053,0x0054,0x0055,0x0056,0x0057,
0x0058,0x0059,0x005A,0x005B,0x005C,0x005D,0x005E,0x005F,
0x0060,0x0061,0x0062,0x0063,0x0064,0x0065,0x0066,0x0067,
0x0068,0x0069,0x006A,0x006B,0x006C,0x006D,0x006E,0x006F,
0x0070,0x0071,0x0072,0x0073,0x0074,0x0075,0x0076,0x0077,
0x0078,0x0079,0x007A,0x007B,0x007C,0x007D,0x007E,0x007F,
0x20AC,0x0081,0x201A,0x0192,0x201E,0x2026,0x2020,0x2021,
0x02C6,0x2030,0x0160,0x2039,0x0152,0x008D,0x017D,0x008F,
0x0090,0x2018,0x2019,0x201C,0x201D,0x2022,0x2013,0x2014,
0x02DC,0x2122,0x0161,0x203A,0x0153,0x009D,0x017E,0x0178,
0x00A0,0x00A1,0x00A2,0x00A3,0x00A4,0x00A5,0x00A6,0x00A7,
0x00A8,0x00A9,0x00AA,0x00AB,0x00AC,0x00AD,0x00AE,0x00AF,
0x00B0,0x00B1,0x00B2,0x00B3,0x00B4,0x00B5,0x00B6,0x00B7,
0x00B8,0x00B9,0x00BA,0x00BB,0x00BC,0x00BD,0x00BE,0x00BF,
0x00C0,0x00C1,0x00C2,0x00C3,0x00C4,0x00C5,0x00C6,0x00C7,
0x00C8,0x00C9,0x00CA,0x00CB,0x00CC,0x00CD,0x00CE,0x00CF,
0x00D0,0x00D1,0x00D2,0x00D3,0x00D4,0x00D5,0x00D6,0x00D7,
0x00D8,0x00D9,0x00DA,0x00DB,0x00DC,0x00DD,0x00DE,0x00DF,
0x00E0,0x00E1,0x00E2,0x00E3,0x00E4,0x00E5,0x00E6,0x00E7,
0x00E8,0x00E9,0x00EA,0x00EB,0x00EC,0x00ED,0x00EE,0x00EF,
0x00F0,0x00F1,0x00F2,0x00F3,0x00F4,0x00F5,0x00F6,0x00F7,
0x00F8,0x00F9,0x00FA,0x00FB,0x00FC,0x00FD,0x00FE,0x00FF
};
unsigned char pl00[256]={
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
0x00,0x81,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x8D,0x00,0x8F,
0x90,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x9D,0x00,0x00,
0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,
0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,
0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,
0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
};
unsigned char pl01[256]={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x8C,0x9C,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x8A,0x9A,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x9F,0x00,0x00,0x00,0x00,0x8E,0x9E,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x83,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
unsigned char pl02[256]={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x88,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x98,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
unsigned char pl20[256]={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x96,0x97,0x00,0x00,0x00,
0x91,0x92,0x82,0x00,0x93,0x94,0x84,0x00,
0x86,0x87,0x95,0x00,0x00,0x00,0x85,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x89,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x8B,0x9B,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
unsigned char pl21[256]={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x99,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
unsigned char *uni_to_cs[256]={
pl00,pl01,pl02,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
pl20,pl21,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
};

static
int my_mb_wc_latin1(CHARSET_INFO *cs  __attribute__((unused)),
        my_wc_t *wc,
        const unsigned char *str,
        const unsigned char *end __attribute__((unused)))
{
  if (str >= end)
    return MY_CS_TOOSMALL;
  
  *wc=cs_to_uni[*str];
  return (!wc[0] && str[0]) ? -1 : 1;
}

static
int my_wc_mb_latin1(CHARSET_INFO *cs  __attribute__((unused)),
        my_wc_t wc,
        unsigned char *str,
        unsigned char *end __attribute__((unused)))
{
  unsigned char *pl;
  
  if (str >= end)
    return MY_CS_TOOSMALL;
  
  pl= uni_to_cs[(wc>>8) & 0xFF];
  str[0]= pl ? pl[wc & 0xFF] : '\0';
  return (!str[0] && wc) ? MY_CS_ILUNI : 1;
}

static MY_CHARSET_HANDLER my_charset_handler=
{
    NULL,     /* init */
    NULL,
    my_mbcharlen_8bit,
    my_numchars_8bit,
    my_charpos_8bit,
    my_well_formed_len_8bit,
    my_lengthsp_8bit,
    my_numcells_8bit,
    my_mb_wc_latin1,
    my_wc_mb_latin1,
    my_caseup_str_8bit,
    my_casedn_str_8bit,
    my_caseup_8bit,
    my_casedn_8bit,
    my_snprintf_8bit,
    my_long10_to_str_8bit,
    my_longlong10_to_str_8bit,
    my_fill_8bit,
    my_strntol_8bit,
    my_strntoul_8bit,
    my_strntoll_8bit,
    my_strntoull_8bit,
    my_strntod_8bit,
    my_strtoll10_8bit,
    my_strntoull10rnd_8bit,
    my_scan_8bit
};


CHARSET_INFO my_charset_latin1=
{
    8,0,0,        /* number    */
    MY_CS_COMPILED | MY_CS_PRIMARY, /* state     */
    "latin1",       /* cs name    */
    "latin1_swedish_ci",    /* name      */
    "",         /* comment   */
    NULL,       /* tailoring */
    ctype_latin1,
    to_lower_latin1,
    to_upper_latin1,
    sort_order_latin1,
    NULL,   /* contractions */
    NULL,   /* sort_order_big*/
    cs_to_uni,    /* tab_to_uni   */
    NULL,   /* tab_from_uni */
    my_unicase_default, /* caseinfo     */
    NULL,   /* state_map    */
    NULL,   /* ident_map    */
    1,      /* strxfrm_multiply */
    1,                  /* caseup_multiply  */
    1,                  /* casedn_multiply  */
    1,      /* mbminlen   */
    1,      /* mbmaxlen  */
    0,      /* min_sort_char */
    255,    /* max_sort_char */
    ' ',                /* pad char      */
    0,                  /* escape_with_backslash_is_dangerous */
    &my_charset_handler,
    &my_collation_8bit_simple_ci_handler
};




/*
 * This file is the latin1 character set with German sorting
 *
 * The modern sort order is used, where:
 *
 * 'ä'  ->  "ae"
 * 'ö'  ->  "oe"
 * 'ü'  ->  "ue"
 * 'ß'  ->  "ss"
 */


/*
 * This is a simple latin1 mapping table, which maps all accented
 * characters to their non-accented equivalents.  Note: in this
 * table, 'ä' is mapped to 'A', 'ÿ' is mapped to 'Y', etc. - all
 * accented characters except the following are treated the same way.
 * Ü, ü, Ö, ö, Ä, ä
 */

static uchar sort_order_latin1_de[] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
   16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
   32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
   48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
   64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
   96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,123,124,125,126,127,
  128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
   65, 65, 65, 65,196, 65, 92, 67, 69, 69, 69, 69, 73, 73, 73, 73,
   68, 78, 79, 79, 79, 79,214,215,216, 85, 85, 85,220, 89,222,223,
   65, 65, 65, 65,196, 65, 92, 67, 69, 69, 69, 69, 73, 73, 73, 73,
   68, 78, 79, 79, 79, 79,214,247,216, 85, 85, 85,220, 89,222, 89
};


/*
  same as sort_order_latin_de, but maps ALL accented chars to unaccented ones
*/

uchar combo1map[]={
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
   16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
   32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
   48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
   64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
   96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,123,124,125,126,127,
  128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
   65, 65, 65, 65, 65, 65, 92, 67, 69, 69, 69, 69, 73, 73, 73, 73,
   68, 78, 79, 79, 79, 79, 79,215,216, 85, 85, 85, 85, 89,222, 83,
   65, 65, 65, 65, 65, 65, 92, 67, 69, 69, 69, 69, 73, 73, 73, 73,
   68, 78, 79, 79, 79, 79, 79,247,216, 85, 85, 85, 85, 89,222, 89
};

uchar combo2map[]={
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0,69, 0, 0, 0, 0, 0,69, 0, 0,83, 0, 0, 0, 0,69, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,69, 0, 0, 0, 0, 0,69, 0, 0, 0, 0
};


/*
  Some notes about the following comparison rules:
  By definition, my_strnncoll_latin_de must works exactly as if had called
  my_strnxfrm_latin_de() on both strings and compared the result strings.

  This means that:
  Ä must also matches ÁE and Aè, because my_strxn_frm_latin_de() will convert
  both to AE.

  The other option would be to not do any accent removal in
  sort_order_latin_de[] at all
*/


static int my_strnncoll_latin1_de(CHARSET_INFO *cs __attribute__((unused)),
          const uchar *a, uint a_length,
          const uchar *b, uint b_length,
                                  my_bool b_is_prefix)
{
  const uchar *a_end= a + a_length;
  const uchar *b_end= b + b_length;
  uchar a_char, a_extend= 0, b_char, b_extend= 0;

  while ((a < a_end || a_extend) && (b < b_end || b_extend))
  {
    if (a_extend)
    {
      a_char=a_extend; a_extend=0;
    }
    else
    {
      a_extend=combo2map[*a];
      a_char=combo1map[*a++];
    }
    if (b_extend)
    {
      b_char=b_extend; b_extend=0;
    }
    else
    {
      b_extend=combo2map[*b];
      b_char=combo1map[*b++];
    }
    if (a_char != b_char)
      return (int) a_char - (int) b_char;
  }
  /*
    A simple test of string lengths won't work -- we test to see
    which string ran out first
  */
  return ((a < a_end || a_extend) ? (b_is_prefix ? 0 : 1) :
    (b < b_end || b_extend) ? -1 : 0);
}


static int my_strnncollsp_latin1_de(CHARSET_INFO *cs __attribute__((unused)),
            const uchar *a, uint a_length,
            const uchar *b, uint b_length,
                                    my_bool diff_if_only_endspace_difference)
{
  const uchar *a_end= a + a_length, *b_end= b + b_length;
  uchar a_char, a_extend= 0, b_char, b_extend= 0;
  int res;

#ifndef VARCHAR_WITH_DIFF_ENDSPACE_ARE_DIFFERENT_FOR_UNIQUE
  diff_if_only_endspace_difference= 0;
#endif

  while ((a < a_end || a_extend) && (b < b_end || b_extend))
  {
    if (a_extend)
    {
      a_char=a_extend;
      a_extend= 0;
    }
    else
    {
      a_extend= combo2map[*a];
      a_char=   combo1map[*a++];
    }
    if (b_extend)
    {
      b_char= b_extend;
      b_extend= 0;
    }
    else
    {
      b_extend= combo2map[*b];
      b_char=   combo1map[*b++];
    }
    if (a_char != b_char)
      return (int) a_char - (int) b_char;
  }
  /* Check if double character last */
  if (a_extend)
    return 1;
  if (b_extend)
    return -1;

  res= 0;
  if (a != a_end || b != b_end)
  {
    int swap= 1;
    if (diff_if_only_endspace_difference)
      res= 1;                                   /* Assume 'a' is bigger */
    /*
      Check the next not space character of the longer key. If it's < ' ',
      then it's smaller than the other key.
    */
    if (a == a_end)
    {
      /* put shorter key in a */
      a_end= b_end;
      a= b;
      swap= -1;         /* swap sign of result */
      res= -res;
    }
    for ( ; a < a_end ; a++)
    {
      if (*a != ' ')
  return (*a < ' ') ? -swap : swap;
    }
  }
  return res;
}


static int my_strnxfrm_latin1_de(CHARSET_INFO *cs __attribute__((unused)),
         uchar * dest, uint len,
         const uchar * src, uint srclen)
{
  const uchar *de = dest + len;
  const uchar *se = src + srclen;
  for ( ; src < se && dest < de ; src++)
  {
    uchar chr=combo1map[*src];
    *dest++=chr;
    if ((chr=combo2map[*src]) && dest < de)
      *dest++=chr;
  }
  if (dest < de)
    bfill(dest, de - dest, ' ');
  return (int) len;
}


void my_hash_sort_latin1_de(CHARSET_INFO *cs __attribute__((unused)),
          const uchar *key, uint len,
          ulong *nr1, ulong *nr2)
{
  const uchar *end= key+len;
  /*
    Remove end space. We have to do this to be able to compare
    'AE' and 'Ä' as identical
  */
  while (end > key && end[-1] == ' ')
    end--;

  for (; key < end ; key++)
  {
    uint X= (uint) combo1map[(uint) *key];
    nr1[0]^=(ulong) ((((uint) nr1[0] & 63)+nr2[0]) * X) + (nr1[0] << 8);
    nr2[0]+=3;
    if ((X= combo2map[*key]))
    {
      nr1[0]^=(ulong) ((((uint) nr1[0] & 63)+nr2[0]) * X) + (nr1[0] << 8);
      nr2[0]+=3;
    }
  }
}


static MY_COLLATION_HANDLER my_collation_german2_ci_handler=
{
  NULL,     /* init */
  my_strnncoll_latin1_de,
  my_strnncollsp_latin1_de,
  my_strnxfrm_latin1_de,
  my_strnxfrmlen_simple,
  my_like_range_simple,
  my_wildcmp_8bit,
  my_strcasecmp_8bit,
  my_instr_simple,
  my_hash_sort_latin1_de,
  my_propagate_complex
};


CHARSET_INFO my_charset_latin1_german2_ci=
{
  31,0,0,       /* number    */
  MY_CS_COMPILED|MY_CS_STRNXFRM,  /* state     */
  "latin1",       /* cs name    */
  "latin1_german2_ci",      /* name      */
  "",         /* comment   */
  NULL,         /* tailoring */
  ctype_latin1,
  to_lower_latin1,
  to_upper_latin1,
  sort_order_latin1_de,
  NULL,         /* contractions */
  NULL,         /* sort_order_big*/
  cs_to_uni,        /* tab_to_uni   */
  NULL,         /* tab_from_uni */
  my_unicase_default,                   /* caseinfo     */
  NULL,         /* state_map    */
  NULL,         /* ident_map    */
  2,          /* strxfrm_multiply */
  1,                                    /* caseup_multiply  */
  1,                                    /* casedn_multiply  */
  1,          /* mbminlen   */
  1,          /* mbmaxlen  */
  0,          /* min_sort_char */
  247,          /* max_sort_char */
  ' ',                                  /* pad char      */
  0,                                    /* escape_with_backslash_is_dangerous */
  &my_charset_handler,
  &my_collation_german2_ci_handler
};


CHARSET_INFO my_charset_latin1_bin=
{
  47,0,0,       /* number    */
  MY_CS_COMPILED|MY_CS_BINSORT,   /* state     */
  "latin1",       /* cs name    */
  "latin1_bin",       /* name      */
  "",         /* comment   */
  NULL,         /* tailoring */
  ctype_latin1,
  to_lower_latin1,
  to_upper_latin1,
  NULL,         /* sort_order   */
  NULL,         /* contractions */
  NULL,         /* sort_order_big*/
  cs_to_uni,        /* tab_to_uni   */
  NULL,         /* tab_from_uni */
  my_unicase_default,                   /* caseinfo     */
  NULL,         /* state_map    */
  NULL,         /* ident_map    */
  1,          /* strxfrm_multiply */
  1,                                    /* caseup_multiply  */
  1,                                    /* casedn_multiply  */
  1,          /* mbminlen   */
  1,          /* mbmaxlen  */
  0,          /* min_sort_char */
  255,          /* max_sort_char */
  ' ',                                  /* pad char      */
  0,                                    /* escape_with_backslash_is_dangerous */
  &my_charset_handler,
  &my_collation_8bit_bin_handler
};


//////////////////////////////////////////////////

MySQLConnMap* getResourceMap() {
    MutexLock lock("MySQLDriver_resource_mutex", &MySQLDriver_resource_mutex);
    if (_mysqlResourceMap==NULL) {
        _mysqlResourceMap = new MySQLConnMap();
    }
    return _mysqlResourceMap;
}

MySQLClient* getMySQLClient() {
    MutexLock lock("MySQLDriver_init_mutex", &MySQLDriver_init_mutex);
    if (!mysqlclient) {
        mysqlclient = new MySQLClient();
        if (!mysqlclient->init()) { xlog.error("Failed to init mysqlClient"); }
    }
    return mysqlclient;
}

int getNextNamedPipeID() {
    MutexLock lock("MySQLDriver_init_mutex", &MySQLDriver_init_mutex);
    return nextPipeNo++;
}

// this method called from mysql_server_init and shows banner
void banner() {
    if (!bannerDisplayed) {
        Logger::configure("/etc/myosp-log.properties");
        if (xlog.isDebugEnabled()) {
            xlog.debug(string("Open Sharding MySQL Driver") +
                       string(" (libmyosp) version ") +
                       WRAPPER_VERSION + " (" + WRAPPER_BUILD_TSTAMP ")");
        }
        bannerDisplayed = true;
    }
}

void trace(const char *name) {
    if (xlog.isTraceEnabled()) {
        xlog.trace(string("In method '") + name + "'");
    }
}

void trace(const char *name, MYSQL *mysql) {
    if (xlog.isTraceEnabled()) {
        xlog.trace("In method '" + string(name) +
                   "' (MYSQL=" + Util::toString(mysql) + ")");
    }
}

void trace(const char *name, MYSQL_RES *res) {
    if (xlog.isTraceEnabled()) {
        xlog.trace("In method '" + string(name) +
                   "' (MYSQL_RES=" + Util::toString(res) + ")");
    }
}

void trace(const char *name, MYSQL_STMT *stmt) {
    if (xlog.isTraceEnabled()) {
        xlog.trace("In method '" + string(name) +
                   "' (MYSQL_STMT=" + Util::toString(stmt) + ")");
    }
}

string getLogPrefix(MYSQL *mysql) {
    string ret = "[mysql=" + Util::toString(mysql) + "] ";
    return ret;
}

#define LOGVERSION "1.1"
#define DRIVERNAME "MyOSP"
#define DBMS "MySQL"
void log_entry_for_analyser(string domain, void *connId,
              int stmtId, string methodSignature, string* params,
              int nParams, string returnVal,
                            struct timeval *tstart, struct timeval *tend) {
  // [LOGSTART][logVersion][driverName][domain][dbms][pid][threadId][connId][stmtId][start][dur][methodSignature(){params}{returnVal}]\n[LOGEND]
    struct timeval tdiff;
    timersub(tend, tstart, &tdiff);
    char sdiff[64];
    sprintf(sdiff, "%ld", tdiff.tv_sec*1000 + tdiff.tv_usec/1000);
    long long ts = (tstart->tv_sec * 1000) + (tstart->tv_usec/1000);
    if(!Pid) Pid=getpid(); //Pid is a global unsigned int.
  unsigned int ThreadId = (unsigned int) pthread_self();
    stringstream ss;
    ss   << "[LOGSTART]"
       << "[" << LOGVERSION      << "]"
       << "[" << DRIVERNAME      << "]"
       << "[" << domain        << "]"  //TODO domain will be an empty string for now.
       << "[" << DBMS          << "]"
         << "[" << Pid                   << "]"
         << "[" << ThreadId        << "]"
         << "[" << Util::toString(connId) << "]"
         << "[" << stmtId                << "]"
         << "[" << ts                    << "]"
         << "[" << sdiff                 << "]"
     << "[" << methodSignature
     << Util::buildParamList(params, nParams, SQUIGGLY_BRACKETS, false)
     << "{" << returnVal             << "}]" << endl
         << "[LOGEND]"                 << endl;

    // output to the log
    try {//TODO:Change Logging
      if(MyOSPConfig::isShardAnalyze() == ANALYZE_LOG_OUTPUT_SYSLOG) {
      syslog(LOG_DEBUG, "%s", ss.str().c_str());
      }
      else if(MyOSPConfig::isShardAnalyze() == ANALYZE_LOG_OUTPUT_STDERR) {
      cerr << ss.str();
        }
      else if(MyOSPConfig::isShardAnalyze() == ANALYZE_LOG_OUTPUT_STDOUT) {
      cout << ss.str();
        }
      else if(MyOSPConfig::isShardAnalyze() == ANALYZE_LOG_OUTPUT_FILE) {
      fprintf(MyOSPConfig::getAnalyzeLogFile(), "%s", ss.str().c_str());
      fflush(MyOSPConfig::getAnalyzeLogFile());
        }
    }
    catch (...) {
      cerr << "Unable to write to log file in " << OSPConfig::getConfigMap()["shard.analyze.log.dir"] << endl;
    }
}

void cleanup() {
    //TODO: close resources? do we care? this gets called just before the process exist
}

int setErrorState(MYSQL *mysql, int _errno, const char *_error,
                  const char *_sqlstate) {

    xlog.error(
        string("Setting error state to ")
        + Util::toString(_errno)
        + string("; ")
        + string(_error)
    );

    getResourceMap()->setErrorState(mysql, new MySQLErrorState(_errno,
                                   _error, _sqlstate));

    return -1;
}

MySQLAbstractConnection *getConnection(MYSQL *mysql, bool createIfNotExist) {
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(mysql);
    
    /*
    if (xlog.isTraceEnabled()) {
        //xlog.trace(
            string("getConnection(MYSQL=")
            + Util::toString((void*)mysql)
            + string(") returning ")
            + string(conn?"a connection object":"NULL")
        );
    }
    */

    return conn;
}

#define valid(x) (x ? true : false)

/*
extern "C" CHARSET_INFO *default_charset_info = NULL; //TODO: init this

extern "C" my_bool my_init() {
    //TODO: delegate to real driver
    return true;
}

extern "C" CHARSET_INFO *get_charset_by_csname(const char *cs_name, uint cs_flags, int my_flags) {
    //TODO: delegate to real driver
    return NULL;
}

extern "C" void *my_malloc(size_t Size, int MyFlags) {
    //TODO: delegate to real driver
    return NULL;
}

extern "C" void my_no_flags_free(void *ptr) {
    //TODO: delegate to real driver
}
*/

MYSQL *mysql_init(MYSQL *mysql) {
    //trace("mysql_init", mysql);

    // show product banner and configure logging
    banner();

    // the API docs say we have to allocate the pointer if it does not already exist
    if (mysql == NULL) {
        mysql = new MYSQL();
        // keep track of this because we have to delete it later
        (*mysqlAllocMap)[mysql] = true;
    }

    // call mysql_init on real driver
    mysql = getMySQLClient()->mysql_init(mysql);

    return mysql;
}

unsigned int mysql_errno(MYSQL *mysql) {
    //trace("mysql_errno", mysql);
    MySQLErrorState *errorState = getResourceMap()->getErrorState(mysql);
    if (errorState != NULL) {
        //xlog.trace(string("mysql_errno() returning local error state ") + Util::toString((int)errorState->my_errno));
        unsigned int ret =  errorState->my_errno;

        if (xlog.isDebugEnabled() || ret>0) {
            xlog.debug(string("mysql_errno returning ") + Util::toString((int)ret));
        }

        // HACK to avoid Python "error totally whack" error messages
        if (ret>CR_MAX_ERROR) {
            return CR_UNKNOWN_ERROR; // 2000
        }

        return ret;
    }
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn) {
        unsigned int ret = conn->mysql_errno(mysql);

        if (xlog.isDebugEnabled() || ret>0) {
            xlog.debug(string("mysql_errno returning ") + Util::toString((int)ret));
        }

        // HACK to avoid Python "error totally whack" error messages
        if (ret>CR_MAX_ERROR) {
            return CR_UNKNOWN_ERROR; // 2000
        }

        return ret;
    } else {
        // no error
        if (xlog.isDebugEnabled()) {
        }
        //HACK: but this could problems with customers
//            xlog.debug("mysql_errno() called but there is no error and no connection! Simulating 2006 / MySQL server has gone away");
//        return CR_SERVER_GONE_ERROR; // 2006
            xlog.debug("mysql_errno() called but there is no error and no connection! Faking success and returning 0");
        return 0; // 2006
    }
}

const char *mysql_error(MYSQL *mysql) {
    //trace("mysql_error", mysql);
    MySQLErrorState *errorState = getResourceMap()->getErrorState(mysql);
    if (errorState != NULL) {

        if (xlog.isDebugEnabled()) {
            xlog.debug(string("mysql_error returning cached errorState"));
        }

        return errorState->my_error;
    }
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn) {
        const char *ret = conn->mysql_error(mysql);
        if (ret == NULL) {
            ret = "";
        }

        if (xlog.isDebugEnabled()) {
            xlog.debug(string("mysql_error returning ") + string(ret));
        }

        return ret;
    } else {
        // no error
//        if (xlog.isDebugEnabled()) {
//            xlog.debug("mysql_error() called but there is no error and no connection! Simulating 2006 / MySQL server has gone away");
//        }
        //HACK:
//        return "MySQL server has gone away";
            xlog.debug("mysql_error() called but there is no error and no connection! Faking success and returning empty string");
        return "";
    }
}

const char * mysql_sqlstate(MYSQL *mysql) {
    //trace("mysql_sqlstate", mysql);
    MySQLErrorState *errorState = getResourceMap()->getErrorState(mysql);
    if (errorState != NULL) {
        return errorState->my_sqlstate;
    }
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (!conn) {
        return "TBD";
    }
    return conn->mysql_sqlstate(mysql);
}

/******************************************************************/
/*************Added function do_osp_connect****************/
/******************************************************************/

int do_osp_connect(MYSQL *mysql, MySQLConnectionInfo *info)
{
    if (xlog.isDebugEnabled()) {
        xlog.debug(string("do_osp_connect: mysql: ") + Util::toString(mysql) + string(" db: ") + info->target_schema_name);
    }

    try {

        if (!info) {
            setErrorState(mysql, CR_UNKNOWN_ERROR, "ERROR: ConnectionInfo is NULL", "OSP01");
            return -1;
        }

        if (info->target_schema_name=="") {
            setErrorState(mysql, CR_UNKNOWN_ERROR, "ERROR: database name is blank", "OSP05");
            return -1;
        }

        if (xlog.isDebugEnabled()) {
             xlog.debug("Creating OSP connection");
        }

        // get connection pool
        OSPConnectionPool *pool = NULL;
        {
            MutexLock lock("MySQLDriver_connpool_mutex", &MySQLDriver_connpool_mutex);
            pool = getResourceMap()->getOSPConnectionPool(info->target_schema_name);
            if (!pool) {
                pool = new OSPConnectionPool();
                getResourceMap()->setOSPConnectionPool(info->target_schema_name, pool);
            }
        }

        // get a connection from the pool
        OSPConnection* ospNetworkConnection = pool->borrowConnection();

        if (ospNetworkConnection) {
            // validate the connection with a ping to the osp process
            try {
                if (xlog.isDebugEnabled()) xlog.debug("Got OSP connection from pool, validating with ping");
                OSPPingRequest pingRequest;
                OSPPingResponse *pingResponse = dynamic_cast<OSPPingResponse*>(ospNetworkConnection->sendMessage(&pingRequest));
                if (xlog.isDebugEnabled()) xlog.debug("Ping OK");
                delete pingResponse;
            }
            catch (...) {
                // bad connection, don't return to pool, try again
                if (xlog.isDebugEnabled()) xlog.debug("Ping failed, removing connection from pool");
                delete ospNetworkConnection;
                ospNetworkConnection = NULL;
            }
        }

        // if there is no available connection or the pooled connection was bad then create a new one
        if (!ospNetworkConnection) {
            // create a dedicated named pipe connection
            if (info->getProtocol() == PROTOCOL_TCP) {
                if (xlog.isDebugEnabled()) xlog.debug("Creating OSPTCPConnection OK");
                ospNetworkConnection = new OSPTCPConnection(info->getHost(), info->getPort());
            }
            else if (info->getProtocol() == PROTOCOL_PIPES) {
                if (xlog.isDebugEnabled()) xlog.debug("Getting next named pipe ID");
                int pipeNo = getNextNamedPipeID();
                if (xlog.isDebugEnabled()) xlog.debug(string("Got next named pipe ID: ") + Util::toString(pipeNo));
                if (xlog.isDebugEnabled()) xlog.debug("Creating OSPNamedPipeConnection OK");
                ospNetworkConnection = new OSPNamedPipeConnection(info, pipeNo);
            }
            else if (info->getProtocol() == PROTOCOL_UNIX_SOCKET) {
                if (xlog.isDebugEnabled()) xlog.debug("Creating OSPUnixSocketConnection OK");
                 ospNetworkConnection = new OSPUnixSocketConnection(info);
            }
            else {
                throw Util::createException("UNSUPPORTED PROTOCOL");
            }
        }

        // create MySQL OSP connection object
        try {
            MySQLOSPConnection *conn = new MySQLOSPConnection(
                                mysql, info->host, info->port, info->target_schema_name,
                                info->user, info->passwd, getResourceMap(), pool,
                                ospNetworkConnection);


            // store mapping from the MYSQL structure to the OSP connection
            getResourceMap()->setConnection(mysql, conn);

            // success
            getResourceMap()->clearErrorState(mysql);

            if (xlog.isDebugEnabled()) {
                xlog.debug(string("do_osp_connect(\"") + Util::toString(mysql) + string(",") + info->target_schema_name + string("\") SUCCESS"));
            }
        }
        catch (...) {
            setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error", "OSP01");
            return -1;
        }


    } catch (const char *exception) {
        xlog.error(string("do_osp_connect: failed due to exception: ") + exception);
        setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [2]", "OSP01");
        return -1;
    } catch (...) {
        xlog.error(string("do_osp_connect: failed due to unknown exception: db: ") + info->target_schema_name);
        setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [3]", "OSP01");
        return -1;
    }

    return 0;

}

/********************************************************************************/
/****This is only a temporary fix, the new 2.6 MyOSP will not have this issue.***/
/********************************************************************************/
MYSQL *mysql_real_connect(MYSQL *mysql, const char *_host, const char *_user,
    const char *_passwd, const char *db, unsigned int port,
    const char *unix_socket, unsigned long clientflag) {

    struct timeval tstart, tend;

    if (MyOSPConfig::isShardAnalyze()) {
         gettimeofday(&tstart, NULL);
    }

    //trace("mysql_real_connect", mysql);
    try {

        if (db!=NULL) {
            //Check for osp: in database name for attempts to use deprecated functionality.
            if (strncmp(db, "osp:", 4)==0) {
                //setErrorState writes message to xlog.error()
                setErrorState(mysql, CR_UNKNOWN_ERROR, "Failed to connect to DB, use of 'osp:dbname' in database string of the myosp driver is no longer supported. [4]", "OSP01");
                return NULL;
            }
        }
        
        if (!getMySQLClient()->init()) {
          setErrorState(mysql, CR_UNKNOWN_ERROR, "Failed to load MySQL driver", "OSP01");
          xlog.error("failed to init mysqlClient");
          return NULL;
        }

        // delete any existing connection info or connection for this mysql handle regardless of what mode we are in
        // NOTE: later on we could be smarter and see if the user is trying to connect to the same server again in
        // which case we could avoid the cost of disconnecting and reconnecting
        MySQLConnectionInfo *old_info = getResourceMap()->getConnectInfo(mysql);
        if (old_info) {
            delete old_info;
        }

        MySQLAbstractConnection *conn = getConnection(mysql, false);
        if (conn) {
            if (xlog.isDebugEnabled()) {
                xlog.debug("In mysql_real_connect() closing down existing connection before creating new connection");
            }
            // In real connect we always close.
            conn->mysql_close(mysql);
            // erase all resources for this mysql handle
            getResourceMap()->erase(mysql);
            // delete the connection
            delete conn;
            // reset reference
            conn = NULL;
        }

        // build up the connection info based on the function arguments
        MySQLConnectionInfo *info = new MySQLConnectionInfo();

        // these fields are populated from arguments to this function regardless of URL
        info->virtual_host=(_host==NULL ? string("") : string(_host));
        info->user = _user==NULL ? string("") : string(_user);
        info->passwd = _passwd==NULL ? string("") : string(_passwd);
        info->port = port;
        info->unix_socket = unix_socket;
        info->clientflag = clientflag;

        // In MySQL, a database name is the same as a schema in other DBMS engines.
        string databaseName = (db ? Util::toString(db) : string(""));

        if (xlog.isDebugEnabled()) {
            xlog.debug(string("mysql_real_connect")
            + string(": mysql handle: ") + Util::toString(mysql)
            + string(", virtual-host: ") + info->virtual_host
            + string(", real-host: ") + info->host
            + string(", port: ") + Util::toString(info->port)
            + string(", user: ") + info->user
            + string(", db: ") + (databaseName=="" ? "NULL" : databaseName.c_str())
            );
        }

        // store the connection info in the map
        getResourceMap()->setConnectInfo(mysql, info);

        if (MyOSPConfig::isOspHost(info->virtual_host)) {
            
            if (xlog.isDebugEnabled()) {
                xlog.debug(string("mysql_real_connect: connect in OSP mode"));
            }
           
            string host_url;
           
            vector<string> conn_info;
            
            //ignoring all but actual_host, port, and schema for now.
            try {
                
                if (xlog.isDebugEnabled()) {
                    xlog.debug(string("mysql_real_connect: host: ") + string(_host==NULL ? "NULL" : _host));
                }
                host_url = MyOSPConfig::getHostUrl(_host);

                if (xlog.isDebugEnabled()) {
                    xlog.debug(string("mysql_real_connect: host_url: ") + host_url);
                }
                conn_info = MyOSPConfig::parseVirtualHostUrl(host_url);

                if (xlog.isDebugEnabled()) {
                    xlog.debug("mysql_real_connect: MyOSPConfig::parseVirtualHostUrl ");
                    vector<string>::iterator it;
                    for ( it=conn_info.begin() ; it < conn_info.end(); it++ ) {
                        xlog.debug(string(": ") + (*it));
                    }
                }

            }
            catch (char* e) {
                setErrorState(mysql, CR_UNKNOWN_ERROR, e, "OSP01");
                return mysql;
            }

            int urlIndex = 0;

            info->osp_vendor = conn_info[urlIndex++];

            string protocolName = conn_info[urlIndex++];

            if (protocolName == "pipes") {
                info->protocol = PROTOCOL_PIPES;
            }
            else if (protocolName == "tcp") {
                info->protocol = PROTOCOL_TCP;
            }
            else if (protocolName == "socket") {
                info->protocol = PROTOCOL_UNIX_SOCKET;
            }
            else {
                xlog.error("Invalid protocol. protocolName: " + protocolName);
                throw "Invalid protocol. protocolName";
            }

            if (info->protocol == PROTOCOL_UNIX_SOCKET) {
                // read socket file name
                info->socket_file = conn_info[urlIndex++];
            }
            else {
                // read host and port
                info->host = conn_info[urlIndex++];
                info->port = Util::toInt(conn_info[urlIndex++]);

                if (info->port == 0) {
                    info->port = 4545;
                }
            }

            info->osp_domain         = conn_info[urlIndex++];
            info->target_dbms        = conn_info[urlIndex++];
            info->target_schema_name = conn_info[urlIndex++];

            if (databaseName != "") {
                // if a database name was supplied as an argument to this function then this takes precedence
                info->target_schema_name = databaseName;
            }

            if (xlog.isDebugEnabled()) {
                xlog.debug(string("mysql_real_connect: ")
                + string(", virtual-host: ") + info->virtual_host
                + string(", real-host: ") + info->host
                + string(", port: ") + Util::toString(info->port)
                + string(", user: ") + info->user
                + string(", osp_vendor: ") + info->osp_vendor
                + string(", protocol: ") + Util::toString(info->protocol)
                + string(", target_dbms: ") + info->target_dbms
                + string(", db: ") + (databaseName=="" ? "NULL" : databaseName)
                );
            }

            if (-1 == do_osp_connect(mysql, info)) {
                setErrorState(mysql, 9001, "Failed to connect to DB [1]", "OSP01");
                return NULL;
            }

        }
        
        // Delegate Mode.
        else {
        
            if (xlog.isDebugEnabled()) {
                xlog.debug("Delegate mode.");
             } //Checkpoint 1

            MySQLAbstractConnection *conn = NULL;

            try {

                if (xlog.isDebugEnabled()) {
                    xlog.debug("Creating native connection");
                }

                // create native connection
                conn = new MySQLNativeConnection(mysql, getMySQLClient(), getResourceMap());

                // store mapping from the MYSQL structure to the native connection
                getResourceMap()->setConnection(mysql, conn);

                if (conn->connect(
                        _host,
                        _user,
                        _passwd,
                        db,
                        port,
                        unix_socket,
                        clientflag
                    )) {

                    // success
                    getResourceMap()->clearErrorState(mysql);

                }
                else {
                    setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [1]", "OSP01");
                    return NULL;
                }

                if (xlog.isDebugEnabled()) {
                    xlog.debug(string("mysql_real_connect: Delegate Mode: SUCCESS: mysql: ") + Util::toString(mysql)
                               + string(" db: ") + Util::toString(db));
                }


            } catch (const char *exception) {
                xlog.error(string("mysql_real_connect failed due to exception: ") + exception);
                setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [2]", "OSP01");
                return NULL;
            } catch (...) {
                xlog.error(string("mysql_real_connect: failed due to unknown exception: ")
                           + string(" host: ") + string(_host==NULL?"NULL":_host)
                           + string(" db: ") + string(db==NULL?"NULL":db));
                setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [3]", "OSP01");
                return NULL;
            }
        }

        if (xlog.isDebugEnabled()) {
            xlog.debug(string("mysql_real_connect() SUCCESS Returning ") + Util::toString((void*)mysql));
        }

        if (MyOSPConfig::isShardAnalyze()) {
            gettimeofday(&tend, NULL);
            string * params = new string[8];
            params[0] = Util::toString(mysql);
            params[1] = Util::toString(_host);
            params[2] = Util::toString(_user);
            params[3] = "hidden"; //Don't log the password value
            params[4] = Util::toString(db);
            params[5] = Util::toString(port);
            params[6] = Util::toString(unix_socket);
            params[7] = Util::toString(clientflag);
            log_entry_for_analyser("", (void *) mysql, 0,
                    "mysql_real_connect(MYSQL *mysql, const char *_host, const char *_user, "
                    "const char *_passwd, const char *db, unsigned int port, const char *unix_socket, "
                    "unsigned long clientflag)", params, 8,"", &tstart, &tend);
            delete [] params;
        }

        return mysql;

    } catch (const char *ex1) {
        xlog.error(string("mysql_real_connect: failed due to exception: ") + string(ex1));
        setErrorState(mysql, CR_UNKNOWN_ERROR, "Failed to connect to DB [2]", "OSP01");
        return NULL;
    } catch (...) {
        xlog.error(string("mysql_real_connect: failed due to unknown exception"));
        setErrorState(mysql, CR_UNKNOWN_ERROR, "Failed to connect to DB [3]", "OSP01");
        return NULL;
    }
}

/**************************************/
/*****New Method for Mysql_Select_DB***/
/**************************************/
int mysql_select_db(MYSQL *mysql, const char *_db) {

    struct timeval tstart, tend;
    if(MyOSPConfig::isShardAnalyze()) {
        gettimeofday(&tstart, NULL);
    }

    int result;

    if (xlog.isDebugEnabled()) {
        xlog.debug(string("mysql_select_db(\"") + Util::toString(mysql) + string(",") + Util::toString(_db) + string("\")"));
    }

    if (!_db) {
        setErrorState(mysql, CR_UNKNOWN_ERROR, "Passed NULL database name", "OSP01");
        xlog.error("failed to init mysqlClient");
        return -1;
    }

    // get the connection info that should have been stored in the previous call to mysql_real_connect()
    MySQLConnectionInfo *info = getResourceMap()->getConnectInfo(mysql);

    if (!info) {
        setErrorState(mysql, CR_UNKNOWN_ERROR, "Passed NULL connectionInfo", "OSP01");
        return -1;
    }
    
    MySQLAbstractConnection *conn = getConnection(mysql, false);

    if (!conn) {
      setErrorState(mysql, CR_UNKNOWN_ERROR, "No Connection From MySQL Select DB", "OSP01");
      return -1;
    }

    // see if db matches the current target_schema_name
    if (strcmp(info->target_schema_name.c_str(), _db)==0) {
        if (xlog.isDebugEnabled()) {
            xlog.debug("mysql_select_db() re-using existing connection");
        }
        // success
        return 0;
    }

    // update the info object to reflect the change in database name
    string original_schema_name = info->target_schema_name;
    info->target_schema_name = _db;

    if (xlog.isDebugEnabled()) {
        xlog.debug(
            string("mysql_select_db() switching from ")
            + original_schema_name
            + string(" to ")
            + info->target_schema_name
        );
    }

    if (MyOSPConfig::isOspHost(info->virtual_host)) {

        // a different db is being selected so we need to close the old connection now
        conn->mysql_close(mysql);

        // erase all resources for this mysql handle
        getResourceMap()->erase(mysql);

        // delete the connection
        delete conn;

        // reset reference
        conn = NULL;

        result = do_osp_connect(mysql, info);

    }
    else {

        // delegate mode
        if (xlog.isDebugEnabled()) {
            xlog.debug("Delegate Mode for Mysql_select_db.");
        }

        try
        {
          // just delegate to the driver
          conn->mysql_select_db(mysql,info->target_schema_name.c_str());

          // success
          getResourceMap()->clearErrorState(mysql);

          if (xlog.isDebugEnabled()) {
              xlog.debug(string("mysql_select_db(\"") + Util::toString(mysql) + string(",") + Util::toString(_db) + string("\") SUCCESS"));
          }

          result = 0;

        } catch (const char *exception) {
          xlog.error(string("mysql_select_db() failed due to exception: ") + exception);
          setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [2]", "OSP01");
          result = -1;
        } catch (...) {
          xlog.error(string("mysql_select_db(") + Util::toString(_db) + string(") failed due to exception"));
          setErrorState(mysql, CR_UNKNOWN_ERROR, "OSP connection error [3]", "OSP01");
          result = -1;
        }

    }

    if(MyOSPConfig::isShardAnalyze()) {
        gettimeofday(&tend, NULL);
        string * params = new string[2];
        params[0] = Util::toString(mysql);
        params[1] = Util::toString(_db);
        log_entry_for_analyser("", (void *) mysql, 0,
                "mysql_select_db(MYSQL *mysql, const char *db)",
                params, 2, "", &tstart, &tend);
        delete [] params;
    }

    return result;
}

my_bool mysql_autocommit(MYSQL *mysql, my_bool auto_mode) {
    //trace("mysql_autocommit", mysql);
    if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_autocommit(" + Util::toString(auto_mode) + ")");
    }
    if (!valid(mysql)) {
        return false;
    }
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        xlog.error(string("mysql_autocommit() returning false because no connection found for mysql handle ") + Util::toString((void*)mysql));
        return false;
    }

    return conn->mysql_autocommit(mysql, auto_mode);
}

MYSQL_STMT * mysql_stmt_init(MYSQL *mysql) {
    //trace("mysql_stmt_init", mysql);
    if (!valid(mysql)) {
        return false;
    }
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        xlog.error(string("mysql_stmt_init() returning NULL because no connection found for mysql handle ") + Util::toString((void*)mysql));
        return NULL;
    }
    MYSQL_STMT *stmt = conn->mysql_stmt_init(mysql);
    //TODO: stmtMap[stmt] =
    return stmt;
}

int mysql_stmt_prepare(MYSQL_STMT *stmt, const char *query,
        unsigned long length) {

    if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_prepare not yet implemented)");
    }
    //trace("mysql_stmt_prepare", stmt);

    return false;

    /*
    if (!valid(stmt)) {
        return false;
    }
    MySQLStatement *dbsStmt = (MySQLStatement *) stmt;
    return dbsStmt->parentConn->mysql_stmt_prepare(stmt, query, length);
    */
}

int mysql_stmt_execute(MYSQL_STMT *stmt) {
    //trace("mysql_stmt_execute", stmt);
    if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_execute not yet implemented)");
    }
    return false;

    /*
    if (!valid(stmt)) {
        return false;
    }
    MySQLStatement *dbsStmt = (MySQLStatement *) stmt;
    return dbsStmt->parentConn->mysql_stmt_execute(stmt);
    */
}

int mysql_query(MYSQL *mysql, const char *sql) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_query implemented to mysql_real_query)");
    }
    return mysql_real_query(mysql, sql, sql==NULL ? 0 : strlen(sql));
}

int mysql_real_query(MYSQL *mysql, const char *sql, unsigned long length) {
    //trace("mysql_real_query", mysql);

    //struct timeval tvStart, tvEnd;
    //gettimeofday(&tvStart, NULL);

    if (xlog.isDebugEnabled()) {
        xlog.debug(getLogPrefix(mysql) + "mysql_real_query(" + string(sql) + ")");
    }

    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        xlog.error(string("mysql_real_query() returning -1 because no connection found for mysql handle ") + Util::toString((void*)mysql));
        setErrorState(mysql, CR_UNKNOWN_ERROR, "no connection found for mysql handle", "OSP01");
        return -1;
    }

    int result;
    if(MyOSPConfig::isShardAnalyze()) {
      struct timeval tstart; gettimeofday(&tstart, NULL);
        result = conn->mysql_real_query(mysql, sql, length);
      struct timeval tend; gettimeofday(&tend, NULL);
      string * params = new string[3];
      params[0] = Util::toString(mysql);
      params[1] = (sql ? Util::toEscapedStringLiteral(sql) : "NULL");
      params[2] = Util::toString(length);
      log_entry_for_analyser("", (void *) mysql, 0,
          "mysql_real_query(MYSQL *mysql, const char *sql, unsigned long length)",
          params, 3, "", &tstart, &tend);
      delete [] params;
    }
    else {
      result = conn->mysql_real_query(mysql, sql, length);
    }

    //gettimeofday(&tvEnd, NULL);

/*
    if (xlog.isDebugEnabled()) {
        if (result!=0) {
            // output details of error
            int _errno = mysql_errno(mysql);
            const char *_error = mysql_error(mysql);
            string msg = string("mysql_real_query(") + string(sql) + string(" ) took ") + Util::getElapsedTimeString(&tvStart, &tvEnd)
                        + string(" second(s) and is returning ") + Util::toString(result)
                        + string(" (FAILURE); errno=") + Util::toString(_errno)
                        + string("; error=") + string(_error);
            xlog.warn(msg);
        }
        else {
                // output details of successful query
                string msg = string("mysql_real_query(") + string(sql) + string(" ) took ") + Util::getElapsedTimeString(&tvStart, &tvEnd)
                            + string(" second(s) and is returning 0 (SUCCESS)");
                xlog.debug(msg);
        }
    }
    */
    
    return result;
}

int mysql_send_query(MYSQL *mysql, const char *sql, unsigned long length) {
    //trace("mysql_send_query", mysql);
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_send_query implemented)");
    }
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        setErrorState(mysql, CR_UNKNOWN_ERROR, "Call to mysql_send_query but there is no current connection", "OSP01");
        return -1;
    }
    return conn->mysql_send_query(mysql, sql, length);
}

MYSQL_RES * mysql_store_result(MYSQL *mysql) {
    ////trace("mysql_store_result BEGIN", mysql);
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_store_result implemented)");
    }
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_store_result but there is no current connection");
        return NULL;
    }
    MYSQL_RES *result = conn->mysql_store_result(mysql);
    getResourceMap()->setConnection(result, conn);
    ////trace("mysql_store_result END", mysql);
    return result;
}

my_bool mysql_more_results(MYSQL *mysql) {
    //trace("mysql_more_results", mysql);
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_more_results implemented)");
    }
    if (!valid(mysql)) {
        return false;
    }
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_more_results but there is no current connection");
        return false;
    }
    return conn->mysql_more_results(mysql);
}

my_bool mysql_commit(MYSQL * mysql) {
    //trace("mysql_commit", mysql);
    /*
    if (xlog.isDebugEnabled()) {
        xlog.debug(string("mysql_commit(") + Util::toString(mysql) + string(")"));
    }
    */

//    struct timeval tvStart, tvEnd;
//    gettimeofday(&tvStart, NULL);

    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_commit but there is no current connection");
        return false;
    }

    my_bool result;
    if(MyOSPConfig::isShardAnalyze()) {
      struct timeval tstart; gettimeofday(&tstart, NULL);
        result = conn->mysql_commit(mysql) ;
      struct timeval tend; gettimeofday(&tend, NULL);
      string * params = new string[1];
      params[0] = Util::toString(mysql);
      log_entry_for_analyser("", (void *) mysql, 0,
          "mysql_commit(MYSQL * mysql)",
          params, 1, "", &tstart, &tend);
      delete [] params;
    }
    else {
        result = conn->mysql_commit(mysql);
    }

    //gettimeofday(&tvEnd, NULL);

    /*
    if (xlog.isDebugEnabled()) {
        // Zero if successful. Nonzero if an error occurred.
        if (ret) {
            string msg = string("mysql_commit() took ")
                + Util::getElapsedTimeString(&tvStart, &tvEnd)
                + string(" second(s) and is returning FAILURE"
            );
            xlog.debug(msg);
        }
        else {
            string msg = string("mysql_commit() took ")
                + Util::getElapsedTimeString(&tvStart, &tvEnd)
                + string(" second(s) and is returning SUCCESS"
            );
            xlog.debug(msg);
        }
    }
    */

    return result;
}

my_bool mysql_rollback(MYSQL * mysql) {
    //trace("mysql_rollback", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_rollback but there is no current connection");
        return false;
    }

    my_bool result;
    if(MyOSPConfig::isShardAnalyze()) {
      struct timeval tstart; gettimeofday(&tstart, NULL);
        result = conn->mysql_rollback(mysql);
      struct timeval tend; gettimeofday(&tend, NULL);
      string * params = new string[1];
      params[0] = Util::toString(mysql);
      log_entry_for_analyser("", (void *) mysql, 0,
          "mysql_rollback(MYSQL * mysql)",
          params, 1, "", &tstart, &tend);
      delete [] params;
      return result;
    }
    else {
        return conn->mysql_rollback(mysql);
    }
}

void mysql_close(MYSQL *mysql) {

    if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_close()");
    }

    //trace("mysql_close", mysql);
    if (!valid(mysql)) {
        // this is quite common, so we don't want to show a warning for this
        if (xlog.isDebugEnabled()) {
            xlog.debug("mysql_close() called with an invalid MYSQL handle");
        }
        return;
    }
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        // this is quite common, so we don't want to show a warning for this
        if (xlog.isDebugEnabled()) {
            xlog.debug("Call to mysql_close but there is no current connection");
        }
        return;
    }

    if (xlog.isDebugEnabled()) {
        xlog.debug("BEFORE remove from map");
    }

    MySQLConnectionInfo *info = getResourceMap()->getConnectInfo(mysql);
    // remove from the map
    getResourceMap()->erase(mysql);
    getResourceMap()->eraseResults(conn);

    // Delete info only after erase method.
    if (info) {
        delete info;
    }

    if (xlog.isDebugEnabled()) {
        xlog.debug("AFTER remove from map, BEFORE delegate mysql_close()");
    }

    // close the connection
    if(MyOSPConfig::isShardAnalyze()) {
      string * params = new string[1];
      params[0] = Util::toString(mysql);
      struct timeval tstart; gettimeofday(&tstart, NULL);
        conn->mysql_close(mysql);
      struct timeval tend; gettimeofday(&tend, NULL);
      log_entry_for_analyser("", (void *) mysql, 0,
          "mysql_close(MYSQL *mysql)",
          params, 1, "", &tstart, &tend);
      delete [] params;
    }
    else {
        // TODO: This should be done through the pool. The pool should be global.
        conn->mysql_close(mysql);
    }

    if (xlog.isDebugEnabled()) {
        xlog.debug("AFTER delegate mysql_close(), BEFORE delete connection object");
    }

    // delete the connection
    delete conn;

    if (xlog.isDebugEnabled()) {
        xlog.debug("AFTER delete connection object. END of mysql_close().");
    }

    // we delete the mysql structure if it was created by us in mysql_init or mysql_connect (we are required
    // to do this according to the MySQL documentation)
    if (mysqlAllocMap->find(mysql) != mysqlAllocMap->end()) {
        mysqlAllocMap->erase(mysql);
        delete mysql;
    }

}

my_bool mysql_stmt_bind_param(MYSQL_STMT * stmt, MYSQL_BIND * bnd) {
    //trace("mysql_stmt_bind_param", stmt);
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_bind_param not yet implemented)");
    }

    return false;

//    if (!valid(stmt)) {
//        return false;
//    }
//    MySQLStatement *dbsStmt = (MySQLStatement *) stmt;
//    return dbsStmt->parentConn->mysql_stmt_bind_param(stmt, bnd);
}

my_bool mysql_stmt_close(MYSQL_STMT * stmt) {
    //trace("mysql_stmt_close", stmt);
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_close not yet implemented)");
    }

    return false;
//    if (!valid(stmt)) {
//        return false;
//    }
//    MySQLStatement *dbsStmt = (MySQLStatement *) stmt;
//    return dbsStmt->parentConn->mysql_stmt_close(stmt);
}
/*
 int mysql_library_init(int argc, char **argv, char **groups){
 banner();
 //trace("mysql_library_init");
 return 0;
 }

 void mysql_library_end(void){
 //trace("mysql_library_end - UNLOADING Driver");
 cleanup();
 }
 */

bool serverInitDone = false;

int mysql_server_init(int argc, char **argv, char **groups) {
    //trace("mysql_server_init");
     if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_server_init implemented)");
    }
    //TODO: add mutex

    if (serverInitDone) {
        return 0;
    }

    // show product banner and configure logging
    banner();

    if (!getMySQLClient()->init()) {
        xlog.error("mysql_server_init() failed to init mysqlclient");
      //  setErrorState(mysql, CR_UNKNOWN_ERROR, "mysql_server_init() failed to init mysqlclient", "OSP01");
        return -1;
    }

    serverInitDone = true;

    // return success code
    return 0;
}

void mysql_server_end(void) {
    //trace("mysql_server_end");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_server_end implemented)");
    }
    cleanup();
}

MYSQL_PARAMETERS * mysql_get_parameters(void) {
    //trace("mysql_get_parameters");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_get_parameters not yet implemented)");
    }
    mysql_get_parametersFnType* tempFunction = (mysql_get_parametersFnType*)getMySQLClient()->get_mysql_function("mysql_get_parameters");
    MYSQL_PARAMETERS * tempValue = tempFunction();
    return tempValue;
}


unsigned int mysql_field_count(MYSQL *mysql) {
    //trace("mysql_field_count", mysql);

    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_field_count but there is no current connection");
        return 0;
    }
    return conn->mysql_field_count(mysql);
}

my_ulonglong mysql_affected_rows(MYSQL *mysql) {
    //trace("mysql_affected_rows", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_affected_rows but there is no current connection");
        return 0;
    }
    my_ulonglong ret = conn->mysql_affected_rows(mysql);
    if (xlog.isTraceEnabled()) {
        //xlog.trace(string("mysql_affected_rows() returning ") + Util::toString((int)ret));
    }
    return ret;
}

my_ulonglong mysql_insert_id(MYSQL *mysql) {
    //trace("mysql_insert_id", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_insert_id but there is no current connection");
        return 0;
    }

    my_ulonglong result;
    if(MyOSPConfig::isShardAnalyze()) {
      struct timeval tstart; gettimeofday(&tstart, NULL);
        result = conn->mysql_insert_id(mysql);
      struct timeval tend; gettimeofday(&tend, NULL);
      string * params = new string[1];
      params[0] = Util::toString(mysql);
      log_entry_for_analyser("", (void *) mysql, 0,
          "mysql_insert_id(MYSQL *mysql)",
          params, 1, "", &tstart, &tend);
      delete [] params;
      return result;
    }
    else {
      return conn->mysql_insert_id(mysql);
    }
}

unsigned int mysql_warning_count(MYSQL *mysql) {
    //trace("mysql_warning_count", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_warning_count but there is no current connection");
        return 0;
    }
    return conn->mysql_warning_count(mysql);
}

const char * mysql_info(MYSQL *mysql) {
    //trace("mysql_info", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_info but there is no current connection");
        return NULL;
    }
    return conn->mysql_info(mysql);
}

unsigned long mysql_thread_id(MYSQL *mysql) {
    //trace("mysql_thread_id", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_thread_id but there is no current connection");
        return 0;
    }
    return conn->mysql_thread_id(mysql);
}

const char * mysql_character_set_name(MYSQL *mysql) {
    //trace("mysql_character_set_name", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_character_set_name but there is no current connection");
        return NULL;
    }
    return conn->mysql_character_set_name(mysql);
}

int mysql_set_character_set(MYSQL *mysql, const char *csname) {
    //trace("mysql_set_character_set", mysql);
    if (xlog.isDebugEnabled()) {
        xlog.debug(string("set_character_set(") + string(csname) + string(")"));
    }
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_set_character_set but there is no current connection");
        return 0;
    }
    return conn->mysql_set_character_set(mysql, csname);
}

void mysql_get_character_set_info(MYSQL *mysql, MY_CHARSET_INFO *charset) {
    //trace("mysql_get_character_set_info", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_get_character_set_info but there is no current connection");
        return;
    }
    conn->mysql_get_character_set_info(mysql, charset);
}

int mysql_ping(MYSQL *mysql) {
    //trace("mysql_ping", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_ping but there is no current connection");
        // return non-zero to indicate the connection is bad
        setErrorState(mysql, CR_UNKNOWN_ERROR, "Call to mysql_ping but there is no current connection", "OSP01");
        return -1;
    }
    return conn->mysql_ping(mysql);
}

const char * mysql_stat(MYSQL *mysql) {
    //trace("mysql_stat", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_stat but there is no current connection");
        return NULL;
    }
    return conn->mysql_stat(mysql);
}

const char * mysql_get_server_info(MYSQL *mysql) {
    //trace("mysql_get_server_info", mysql);
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_get_server_info implemented)");
    }
    const char *ret = NULL;
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn) {
        ret = conn->mysql_get_server_info(mysql);
    }
    if (!ret) {
        // fail-safe to prevent segv
        ret = "5.1.32";
    }
    return ret;
}

const char * mysql_get_client_info(void) {
    //trace("mysql_get_client_info");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_get_client_info implemented)");
    }
    mysql_get_client_infoFnType* tempFunction = (mysql_get_client_infoFnType*)getMySQLClient()->get_mysql_function("mysql_get_client_info");
    const char * tempValue = tempFunction();
    return tempValue;
}

unsigned long mysql_get_client_version(void) {
    //trace("mysql_get_client_version");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_get_client_version implemented)");
    }
    mysql_get_client_versionFnType* tempFunction = (mysql_get_client_versionFnType*)getMySQLClient()->get_mysql_function("mysql_get_client_version");
    unsigned long tempValue = tempFunction();
    //xlog.trace(string("mysql_get_client_version() returning ") + Util::toString((int)tempValue));
    return tempValue;
}

const char * mysql_get_host_info(MYSQL *mysql) {
    //trace("mysql_get_host_info", mysql);

    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_get_host_info but there is no current connection");
        return NULL;
    }
    return conn->mysql_get_host_info(mysql);
}

unsigned long mysql_get_server_version(MYSQL *mysql) {
    //trace("mysql_get_server_version");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_get_server_version implemented)");
    }
    unsigned long ret = 0;
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn) {
        ret = conn->mysql_get_server_version(mysql);
    }
    return ret;
}

unsigned int mysql_get_proto_info(MYSQL *mysql) {
    //trace("mysql_get_proto_info", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_get_proto_info but there is no current connection");
        return 0;
    }
    return conn->mysql_get_proto_info(mysql);
}

MYSQL_RES * mysql_list_dbs(MYSQL *mysql, const char *wild) {
    //trace("mysql_list_dbs", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_list_dbs but there is no current connection");
        return NULL;
    }
    return conn->mysql_list_dbs(mysql, wild);
}

MYSQL_RES * mysql_list_tables(MYSQL *mysql, const char *wild) {
    //trace("mysql_list_tables", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_list_tables but there is no current connection");
        return NULL;
    }
    return conn->mysql_list_tables(mysql, wild);
}

MYSQL_RES * mysql_list_processes(MYSQL *mysql) {
    //trace("mysql_list_processes", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_list_processes but there is no current connection");
        return NULL;
    }
    return conn->mysql_list_processes(mysql);
}

// this is the signature for MySQL prior to 5.1.18
int mysql_options(MYSQL *mysql, enum mysql_option option, const char *arg) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_options not yet implemented)");
    }
    // mysql_options modifies the MYSQL struct so we can just delegate to the real driver for this
    return getMySQLClient()->mysql_options(mysql, option, arg);
    /*
    //trace("mysql_options", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (!conn) {
        if (xlog.isDebugEnabled()) xlog.debug("Ignoring call to mysql_options and simulating success because there is no connection");
        return 0;
    }
    return conn->mysql_options(mysql, option, arg);
    */
}

// this is the signature for MySQL 5.1.18 and later
int mysql_options(MYSQL *mysql, enum mysql_option option, const void *arg) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_options not yet implemented)");
    }
    // mysql_options modifies the MYSQL struct so we can just delegate to the real driver for this
    return getMySQLClient()->mysql_options(mysql, option, arg);
    //trace("mysql_options", mysql);
    /*
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (!conn) {
        if (xlog.isDebugEnabled()) xlog.debug("Ignoring call to mysql_options and simulating success because there is no connection");
        return 0;
    }
    return conn->mysql_options(mysql, option, arg);
    */
}

unsigned long mysql_escape_string(char *to, const char *from,
        unsigned long from_length) {
    //trace("mysql_escape_string");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_escape_string implemented)");
    }

    return getMySQLClient()->mysql_escape_string(to, from, from_length);
}

unsigned long mysql_hex_string(char *to, const char *from,
        unsigned long from_length) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_hex_string not yet implemented)");
    }
    //trace("mysql_hex_string");
    return -1;
}

unsigned long mysql_real_escape_string(MYSQL *mysql, char *to,
        const char *from, unsigned long length) {

    //trace("mysql_real_escape_string", mysql);
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_real_escape_string implemented)");
    }

    return getMySQLClient()->mysql_real_escape_string(mysql, to, from, length);
}

int mysql_next_result(MYSQL *mysql) {
    //trace("mysql_next_result", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_next_result but there is no current connection");
        setErrorState(mysql, CR_UNKNOWN_ERROR, "Call to mysql_next_result but there is no current connection", "OSP01");
        return -1;
    }
    return conn->mysql_next_result(mysql);
}


void mysql_debug(const char *debug) {
    //trace("mysql_debug");
    xlog.debug(string("mysql_debug(\"") + string(debug==NULL?"":debug) + string("\""));
}

MYSQL_RES * mysql_use_result(MYSQL *mysql) {
    //trace("mysql_use_result", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_use_result but there is no current connection");
        return NULL;
    }
    return conn->mysql_use_result(mysql);
}


int mysql_set_server_option(MYSQL *mysql, enum enum_mysql_set_option option) {
    //trace("mysql_set_server_option", mysql);
    MySQLAbstractConnection *conn = getConnection(mysql, false);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_set_server_option but there is no current connection");
        return 0;
    }
    return conn->mysql_set_server_option(mysql, option);
}

//---------------------------------------------------------------------------------------------------
//TODO: the following methods all have MYSQL_RES* arguments and should delegate to ResultSet object
// rather than to a Connection object!!
//---------------------------------------------------------------------------------------------------

void mysql_free_result(MYSQL_RES *res) {
    //trace("mysql_free_result", res);
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_free_result implemented)");
    }
    if (!res) {
        return;
    }
    MySQLAbstractResultSet *rs = getResourceMap()->getResultSet(res);
    if (rs == NULL) {
        xlog.warn(string("Call to mysql_free_result(") + Util::toString((void*)res) + string(") but there is no result set mapped to that MYSQL_RES*"));
        return;
    }

    // remove from map
    getResourceMap()->erase(res);

    // delegate to result set class
    rs->mysql_free_result(res);

    // delete the result set class
    delete rs;
}

void mysql_data_seek(MYSQL_RES *result, my_ulonglong offset) {
    //trace("mysql_data_seek", result);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(result);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_data_seek but there is no current connection");
        return;
    }
    conn->mysql_data_seek(result, offset);
}

MYSQL_ROW_OFFSET mysql_row_seek(MYSQL_RES *result, MYSQL_ROW_OFFSET offset) {
    //trace("mysql_row_seek", result);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(result);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_row_seek but there is no current connection");
        return NULL;
    }
    return conn->mysql_row_seek(result, offset);
}

MYSQL_RES * mysql_list_fields(MYSQL *mysql, const char *table, const char *wild) {
    //trace("mysql_list_fields", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_list_fields");
    return NULL;
}

void myodbc_remove_escape(MYSQL *mysql, char *name) {
    //trace("myodbc_remove_escape", mysql);
    ////if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: myodbc_remove_escape");
}

unsigned int mysql_thread_safe(void) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_thread_safe not implemented)");
    }
    //trace("mysql_thread_safe");
    ////if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_thread_safe");
    return 0;
}

my_bool mysql_embedded(void) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_embedded not implemented)");
    }
    //trace("mysql_embedded");
    ////if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_embedded");
    return false;
}

MYSQL_MANAGER* mysql_manager_init(MYSQL_MANAGER* con) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_manager_init not yet implemented)");
    }
    //trace("mysql_manager_init");
    ////if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_manager_init");
    return NULL;
}

MYSQL_MANAGER* mysql_manager_connect(MYSQL_MANAGER* con, const char* host,
        const char* user, const char* passwd, unsigned int port) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_manager_connect not yet implemented)");
    }
    //trace("mysql_manager_connect");
//    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_manager_connect");
    return NULL;
}

void mysql_manager_close(MYSQL_MANAGER* con) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_manager_close not yet implemented)");
    }
    //trace("mysql_manager_close");
//    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_manager_close");
}

int mysql_manager_command(MYSQL_MANAGER* con, const char* cmd, int cmd_len) {
    //trace("mysql_manager_command");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_manager_command not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_manager_command");
    return -1;
}

int mysql_manager_fetch_line(MYSQL_MANAGER* con, char* res_buf,
        int res_buf_size) {
    //trace("mysql_manager_fetch_line");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_manager_fetch_line not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_manager_fetch_line");
    return -1;
}

my_bool mysql_read_query_result(MYSQL *mysql) {
    //trace("mysql_read_query_result", mysql);
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_read_query_result not yet implemented)");
    }
    if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_read_query_result");
    return false;
}

int mysql_stmt_fetch(MYSQL_STMT *stmt) {
    //trace("mysql_stmt_fetch");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_fetch not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_fetch");
    return -1;
}

int mysql_stmt_fetch_column(MYSQL_STMT *stmt, MYSQL_BIND *bind_arg,
        unsigned int column, unsigned long offset) {
    //trace("mysql_stmt_fetch_column");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_fetch_column not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_fetch_column");
    return -1;
}

int mysql_stmt_store_result(MYSQL_STMT *stmt) {
    //trace("mysql_stmt_store_result");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_store_result not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_store_result");
    return -1;
}

unsigned long mysql_stmt_param_count(MYSQL_STMT * stmt) {
    //trace("mysql_stmt_param_count");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_param_count not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_param_count");
    return 0;
}

my_bool mysql_stmt_attr_set(MYSQL_STMT *stmt,
        enum enum_stmt_attr_type attr_type, const void *attr) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_attr_set not yet implemented)");
    }
    //trace("mysql_stmt_attr_set");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_attr_set");
    return 0;
}

my_bool mysql_stmt_attr_get(MYSQL_STMT *stmt,
        enum enum_stmt_attr_type attr_type, void *attr) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_attr_get not yet implemented)");
    }
    //trace("mysql_stmt_attr_get");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_attr_get");
    return false;
}

my_bool mysql_stmt_bind_result(MYSQL_STMT * stmt, MYSQL_BIND * bnd) {
    //trace("mysql_stmt_bind_result");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_bind_result not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_bind_result");
    return false;
}

my_bool mysql_stmt_reset(MYSQL_STMT * stmt) {
    //trace("mysql_stmt_reset");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_reset not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_reset");
    return false;
}

my_bool mysql_stmt_free_result(MYSQL_STMT *stmt) {
    //trace("mysql_stmt_free_result");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_free_result not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_free_result");
    return false;
}

my_bool mysql_stmt_send_long_data(MYSQL_STMT *stmt, unsigned int param_number,
        const char *data, unsigned long length) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_send_long_data not yet implemented)");
    }
    //trace("mysql_stmt_send_long_data");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_send_long_data");
    return false;
}

MYSQL_RES * mysql_stmt_result_metadata(MYSQL_STMT *stmt) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_result_metadata not yet implemented)");
    }
    //trace("mysql_stmt_result_metadata");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_result_metadata");
    return NULL;
}

MYSQL_RES * mysql_stmt_param_metadata(MYSQL_STMT *stmt) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_param_metadata not yet implemented)");
    }
    //trace("mysql_stmt_param_metadata");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_param_metadata");
    return NULL;
}

unsigned int mysql_stmt_errno(MYSQL_STMT * stmt) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_errno not yet implemented)");
    }
    //trace("mysql_stmt_errno");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_errno");
    return -1;
}

const char * mysql_stmt_error(MYSQL_STMT * stmt) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_error not yet implemented)");
    }
    //trace("mysql_stmt_error");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_error");
    return NULL;
}

const char * mysql_stmt_sqlstate(MYSQL_STMT * stmt) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_sqlstate not yet implemented)");
    }
    //trace("mysql_stmt_sqlstate");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_sqlstate");
    return NULL;
}

MYSQL_ROW_OFFSET mysql_stmt_row_seek(MYSQL_STMT *stmt, MYSQL_ROW_OFFSET offset) {
    //trace("mysql_stmt_row_seek");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_row_seek not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_row_seek");
    return NULL;
}

MYSQL_ROW_OFFSET mysql_stmt_row_tell(MYSQL_STMT *stmt) {
    //trace("mysql_stmt_row_tell");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_row_tell not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_row_tell");
    return NULL;
}

void mysql_stmt_data_seek(MYSQL_STMT *stmt, my_ulonglong offset) {
    //trace("mysql_stmt_data_seek");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_data_seek not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_data_seek");
}

my_ulonglong mysql_stmt_num_rows(MYSQL_STMT *stmt) {
    //trace("mysql_stmt_num_rows");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_num_rows not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_num_rows");
    return 0;
}

my_ulonglong mysql_stmt_affected_rows(MYSQL_STMT *stmt) {
    //trace("mysql_stmt_affected_rows");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_affected_rows not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_affected_rows");
    return 0;
}

my_ulonglong mysql_stmt_insert_id(MYSQL_STMT *stmt) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_insert_id not yet implemented)");
    }
    //trace("mysql_stmt_insert_id");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_insert_id");
    return 0;
}

unsigned int mysql_stmt_field_count(MYSQL_STMT *stmt) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_stmt_field_count not yet implemented)");
    }
    //trace("mysql_stmt_field_count");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_stmt_field_count");
    return -1;
}

MYSQL * mysql_connect(MYSQL *mysql, const char *host, const char *user,
        const char *passwd) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_connect not yet implemented)");
    }
    //trace("mysql_connect", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_connect");
    return NULL;
}

int mysql_create_db(MYSQL *mysql, const char *DB) {
    //trace("mysql_create_db", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_create_db");
    setErrorState(mysql, CR_UNKNOWN_ERROR, "mysql_create_db is not supported in myosp", "OSP01");
    return -1;
}

int mysql_drop_db(MYSQL *mysql, const char *DB) {
    //trace("mysql_drop_db", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_drop_db");
    setErrorState(mysql, CR_UNKNOWN_ERROR, "mysql_drop_db is not supported in myosp", "OSP01");
    return -1;
}

my_bool mysql_thread_init(void) {
    //trace("mysql_thread_init");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_thread_init not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_thread_init");
    return false;
}

void mysql_thread_end(void) {
    //trace("mysql_thread_end");
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_thread_end not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_thread_end");
}

my_bool mysql_ssl_set(MYSQL *mysql, const char *key, const char *cert,
        const char *ca, const char *capath, const char *cipher) {
    //trace("mysql_ssl_set", mysql);
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_ssl_set not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_ssl_set");
    return false;
}

const char * mysql_get_ssl_cipher(MYSQL *mysql) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_get_ssl_cipher not yet implemented)");
    }
    //trace("mysql_get_ssl_cipher", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_get_ssl_cipher");
    return NULL;
}

my_bool mysql_change_user(MYSQL *mysql, const char *user, const char *passwd,
        const char *db) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_change_user not yet implemented)");
    }
    //trace("mysql_change_user", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_change_user");
    return false;
}

my_bool mysql_master_query(MYSQL *mysql, const char *q, unsigned long length) {
    //trace("mysql_master_query", mysql);
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_master_query not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_master_query");
    return false;
}

my_bool mysql_master_send_query(MYSQL *mysql, const char *q,
        unsigned long length) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_master_send_query not yet implemented)");
    }
    //trace("mysql_master_send_query", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_master_query");
    return false;
}

my_bool mysql_slave_query(MYSQL *mysql, const char *q, unsigned long length) {
    //trace("mysql_slave_query", mysql);
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_slave_query not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_slave_query");
    return false;
}

my_bool mysql_slave_send_query(MYSQL *mysql, const char *q,
        unsigned long length) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_slave_send_query not yet implemented)");
    }
    //trace("mysql_slave_send_query", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_slave_query");
    return false;
}

void mysql_enable_rpl_parse(MYSQL* mysql) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_enable_rpl_parse not yet implemented)");
    }
    //trace("mysql_enable_rpl_parse", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_enable_rpl_parse");
}

void mysql_disable_rpl_parse(MYSQL* mysql) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_disable_rpl_parse not yet implemented)");
    }
    //trace("mysql_disable_rpl_parse", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_disable_rpl_parse_rpl_parse");
}

int mysql_rpl_parse_enabled(MYSQL* mysql) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_rpl_parse_enabled not yet implemented)");
    }
    //trace("mysql_rpl_parse_enabled", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_rpl_parse_enabled");
    return 0;
}

void mysql_enable_reads_from_master(MYSQL* mysql) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_enable_reads_from_master not yet implemented)");
    }
    //trace("mysql_enable_reads_from_master", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_enable_reads_from_master");
}

void mysql_disable_reads_from_master(MYSQL* mysql) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_disable_reads_from_master not yet implemented)");
    }
    //trace("mysql_disable_reads_from_master", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_disable_reads_from_master");
}

my_bool mysql_reads_from_master_enabled(MYSQL* mysql) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_reads_from_master_enabled not yet implemented)");
    }
    //trace("mysql_reads_from_master_enabled", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_reads_from_master_enabled");
    return false;
}

enum mysql_rpl_type mysql_rpl_query_type(const char* q, int len) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_rpl_query_type not yet implemented)");
    }
    //trace("mysql_rpl_query_type");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_rpl_query_type");
    //TODO: not sure what to return here
    return MYSQL_RPL_MASTER;
}

my_bool mysql_rpl_probe(MYSQL* mysql) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_rpl_probe not yet implemented)");
    }
    //trace("mysql_rpl_probe", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_rpl_probe");
    return false;
}

int mysql_set_master(MYSQL* mysql, const char* host, unsigned int port,
        const char* user, const char* passwd) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_set_master not yet implemented)");
    }
    //trace("mysql_set_master", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_set_master");
    return 0;
}

int mysql_add_slave(MYSQL* mysql, const char* host, unsigned int port,
        const char* user, const char* passwd) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_add_slave not yet implemented)");
    }
    //trace("mysql_add_slave", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_add_slave");
    return 0;
}

int mysql_shutdown(MYSQL *mysql, enum mysql_enum_shutdown_level shutdown_level) {
    //trace("mysql_shutdown", mysql);
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_shutdown not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_shutdown");
    return 0;
}

int mysql_dump_debug_info(MYSQL *mysql) {
    //trace("mysql_dump_debug_info", mysql);
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_dump_debug_info not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_dump_debug_info");
    return 0;
}

int mysql_refresh(MYSQL *mysql, unsigned int refresh_options) {
    //trace("mysql_refresh", mysql);
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_refresh not yet implemented)");
    }
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_refresh");
    return 0;
}

int mysql_kill(MYSQL *mysql, unsigned long pid) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_kill not yet implemented)");
    }
    //trace("mysql_kill", mysql);
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_kill");
    return 0;
}

void mysql_set_local_infile_handler(MYSQL *mysql,
    int (*local_infile_init)(void **, const char *, void *),
    int (*local_infile_read)(void *, char *, unsigned int),
    void (*local_infile_end)(void *),
    int (*local_infile_error)(void *, char*, unsigned int),
    void *)
{
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_set_local_infile_handler not yet implemented)");
    }
    // no implementation
    //trace("mysql_set_local_infile_handler");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_set_local_infile_handler");
}

void mysql_set_local_infile_default(MYSQL *mysql) {
   if (xlog.isDebugEnabled()) {
        xlog.debug("mysql_set_local_infile_default not yet implemented)");
    }
    // no implementation
    //trace("mysql_set_local_infile_default");
    //if (xlog.isTraceEnabled()) xlog.trace("CALL TO UNIMPLEMENTED METHOD: mysql_set_local_infile_default");
}

my_ulonglong mysql_num_rows(MYSQL_RES *res) {
    //trace("mysql_num_rows", res);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(res);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) {
            xlog.debug("mysql_num_rows() returning 0 (no connection)");
        }
        return 0;
    }
    my_ulonglong ret = conn->mysql_num_rows(res);
    if (xlog.isDebugEnabled()) {
        xlog.debug(string("mysql_num_rows() returning ") + Util::toString((long int)ret));
    }
    return ret;
}

unsigned int mysql_num_fields(MYSQL_RES *res) {
    //trace("mysql_num_fields", res);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(res);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_num_fields but there is no current connection");
        return 0;
    }
    return conn->mysql_num_fields(res);
}

my_bool mysql_eof(MYSQL_RES *res) {
    //trace("mysql_eof", res);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(res);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_eof but there is no current connection");
        return 0;
    }
    return conn->mysql_eof(res);
}

MYSQL_ROW mysql_fetch_row(MYSQL_RES *result) {
    //trace("mysql_fetch_row BEGIN", result);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(result);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_fetch_row but there is no current connection");
        return NULL;
    }
    MYSQL_ROW ret = conn->mysql_fetch_row(result);
    //trace("mysql_fetch_row END", result);
    return ret;
}

unsigned long * mysql_fetch_lengths(MYSQL_RES *result) {
    //trace("mysql_fetch_lengths", result);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(result);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_fetch_lengths but there is no current connection");
        return NULL;
    }
    return conn->mysql_fetch_lengths(result);
}

MYSQL_FIELD_OFFSET mysql_field_seek(MYSQL_RES *result,
        MYSQL_FIELD_OFFSET offset) {
    //trace("mysql_field_seek", result);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(result);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_field_seek but there is no current connection");
        return 0;
    }
    return conn->mysql_field_seek(result, offset);
}

MYSQL_FIELD * mysql_fetch_field(MYSQL_RES *result) {
    //trace("mysql_fetch_field", result);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(result);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_fetch_field but there is no current connection");
        return NULL;
    }
    return conn->mysql_fetch_field(result);
}

MYSQL_FIELD * mysql_fetch_field_direct(MYSQL_RES *res, unsigned int fieldnr) {
    //trace("mysql_fetch_field_direct", res);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(res);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_fetch_field_direct but there is no current connection");
        return NULL;
    }
    return conn->mysql_fetch_field_direct(res, fieldnr);
}

MYSQL_FIELD * mysql_fetch_fields(MYSQL_RES *res) {
    //trace("mysql_fetch_fields", res);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(res);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_fetch_fields but there is no current connection");
        return NULL;
    }
    MYSQL_FIELD * ret = conn->mysql_fetch_fields(res);

    //BEGIN DEBUG CODE
    /*
    unsigned int n = mysql_num_fields(res);
    for (unsigned int i=0; i<n; i++) {
        //logger.info << "Field #" << i << endl;
        //logger.info << "Field Name: " << i << ret[i].name << endl;
    }
    */
    //END DEBUG CODE

    return ret;
}

MYSQL_ROW_OFFSET mysql_row_tell(MYSQL_RES *res) {
    //trace("mysql_row_tell", res);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(res);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_row_tell but there is no current connection");
        return 0;
    }
    return conn->mysql_row_tell(res);
}

MYSQL_FIELD_OFFSET mysql_field_tell(MYSQL_RES *res) {
    //trace("mysql_field_tell", res);
    MySQLAbstractConnection *conn = getResourceMap()->getConnection(res);
    if (conn == NULL) {
        if (xlog.isDebugEnabled()) xlog.debug("Call to mysql_field_tell but there is no current connection");
        return 0;
    }
    return conn->mysql_field_tell(res);
}

void my_net_local_init(NET *net){
   if (xlog.isDebugEnabled()) {
        xlog.debug("my_net_local_init not yet implemented)");
    }
}
