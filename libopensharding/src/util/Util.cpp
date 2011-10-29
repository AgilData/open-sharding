/**
 * Copyright (c) 2011, CodeFutures Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 * disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
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


#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <time.h>
#ifdef _WINDOWS
#include <Windows.h>
#include <Rpc.h>
#pragma comment(lib, "Rpcrt4.lib")
#else
#include <sys/time.h>
#include <uuid/uuid.h>
#endif

#include <util/Util.h>
#include <logger/Logger.h>

using namespace std;
using namespace logger;

namespace util {

/*static*/ Logger &Util::xlog = Logger::getLogger("Util");

/*static*/ void Util::init() {
     srand ( time(NULL) );
}

/*static*/ const char *Util::notImplemented(const char *klass, const char *method) {
    //logger.info << "Call to unimplemented method " << klass << "::" << method << endl;
    // return exception message to throw
    return "Method Not Implemented - See stderr for details";
}

/*static*/ char * Util::EMPTYCHARBUF = "";

/*
 * Generate short (16 char) unique ID for Tribes usage.
 */
/*static*/ void Util::generateUniqueID(char *uniqueId) {
    //TODO: this is not very unique
    //BUG: https://cisaacson-bugz.cvsdude.com/show_bug.cgi?id=23
    const char *chars = "0123456789abcdef";

    for (int i=0; i<16; i++) {
        uniqueId[i] = chars[rand()%16];
    }
    uniqueId[16] = '\0';

}

/*static*/ string Util::generateUUID() {
#ifdef _WINDOWS
	UUID uuid;
	char * tmp;
	UuidCreate(&uuid);
	UuidToStringA(&uuid, tmp);
	string ret(tmp);
	return ret;
#else
    char tmp[37]; // 36 chars plus \0
    uuid_t uuid;
    uuid_generate(uuid);
    uuid_unparse(uuid, tmp);
    string ret(tmp);
    return ret;
#endif

    // call Apache Portable Runtime UUID function
    /*
    apr_uuid_t uuid;
    apr_uuid_get(&uuid);
    char *ret = new char[APR_UUID_FORMATTED_LENGTH+1];
    memset(ret, 0, APR_UUID_FORMATTED_LENGTH+1);
    apr_uuid_format(ret, &uuid);
    return ret;
    */
}

/*static*/ string Util::createRandomWord(unsigned int length) {
    const char *chars = "01234567890 abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const int l = strlen(chars);

    char *word = new char[length+1];

    for (unsigned int i=0; i<length; i++) {
        int index = rand()%l;
        word[i] = chars[index];
    }
    word[length] = '\0';

    //printf("createRandomWord returning '%s'\n", word);

    return word;
}

/* Prints a hex dump of a region of memory */
void Util::dump(const char *buf, unsigned int len, int lineLength) {

    // prepare temporary buffer for constructing each line of output
    int tempLen = lineLength*5 + 512;
    char *temp = new char[tempLen];
    memset(temp, 0, tempLen);
    int tempOffset = 0;

    int c = 0;
    char *text = new char[lineLength+1];
    memset(text, 0, lineLength+1);
    for (unsigned int i=0; i<len; i++) {
        if ((buf[i] >= 'a' && buf[i] <= 'z')
            || (buf[i] >= 'A' && buf[i] <= 'Z')
            || (buf[i] >= '0' && buf[i] <= '9')
            || buf[i]==' '
            || buf[i]=='"'
            || buf[i]=='\''
            || buf[i]=='!'
            || buf[i]=='@'
            || buf[i]=='$'
            || buf[i]=='%'
            || buf[i]=='^'
            || buf[i]=='&'
            || buf[i]=='*'
            || buf[i]=='('
            || buf[i]==')'
            || buf[i]=='{'
            || buf[i]=='}'
            || buf[i]=='['
            || buf[i]==']'
            || buf[i]=='\\'
            || buf[i]=='.'
            || buf[i]==','
            || buf[i]=='/'
            || buf[i]=='?'
            || buf[i]=='<'
            || buf[i]=='>'
            || buf[i]=='|'
            || buf[i]=='~'
            || buf[i]=='`'
            || buf[i]=='+'
            || buf[i]=='-'
            || buf[i]=='='
            || buf[i]==':'
            || buf[i]==';') {
            text[c] = buf[i];
         }
         else {
            text[c] = '.';
        }

        // write the hex value to the temp buffer
        sprintf(temp+tempOffset, "%02x", buf[i] & 0xFF);
        tempOffset += 2;

        if (++c == lineLength || i+1==len) {
            while (c != lineLength) {
                sprintf(temp+tempOffset, "   ");
                tempOffset += 3;
                c++;
            }
            sprintf(temp+tempOffset, " [%s]", text);

            // log the output
            xlog.debug(temp);

            // reset the temp buffer
            memset(temp, 0, tempLen);
            tempOffset = 0;

            // reset the text buffer
            c = 0;
            memset(text, 0, lineLength+1);
        }
        else {
            sprintf(temp+tempOffset, " ");
            tempOffset++;
        }
    }

    //sprintf(temp+tempOffset, "\n");

    // delete the temp buffer
    delete [] temp;
	delete [] text;

}

char *Util::createString(const char *source) {
    if (source==NULL) {
        return NULL;
    }
    //log->debug("createString(const char*)");
    int l = strlen(source);
    char *ret = new char[l+1];
    strcpy(ret, source);
    ret[l] = '\0';
    //log->debug(ret);
    return ret;
}

/*static*/ string Util::toLower(string str) {
    int l = str.size();
    char *tmp = new char[l+1];
    for (int i=0;i<l;i++) {
        if (str[i] >= 0x41 && str[i] <= 0x5A) {
            tmp[i] = tolower(str[i]);
        }
        else {
            tmp[i] = str[i];
        }
    }
    tmp[l] = '\0';
    string ret(tmp);
    delete [] tmp;
    return ret;
}

bool Util::_strnicmp(const char *str1, const char *str2, int l) {
    //TODO: not as efficient as calling real _strnicmp
    for (int i=0; i<l; i++) {
        if (tolower(str1[i]) != tolower(str2[i])) {
            return -1; //TODO: does not follow the signature of strcmp() but fine for our purposes
        }
    }
    return 0;
}

bool Util::equalsIgnoreCase(string str1, string str2) {
    if (str1.size() != str2.size()) {
        return false;
    }
    return 0 == _strnicmp(str1.c_str(), str2.c_str(), str1.size());
}

bool Util::equalsIgnoreCase(const char *str1, const char *str2) {
    unsigned int len = strlen(str1);
    if (len != strlen(str2)) {
        return false;
    }
    return 0 == _strnicmp(str1, str2, len);
}

/*static*/ string Util::toString(long int i) {
    char temp[256];
    sprintf(temp, "%ld", i);
    return string(temp);
}

/*static*/ string Util::toString(int i) {
    char temp[256];
    sprintf(temp, "%d", i);
    return string(temp);
}

/*static*/ string Util::toString(unsigned int i) {
    char temp[256];
    sprintf(temp, "%u", i);
    return string(temp);
}

/*static*/ string Util::toString(void *p) {
    if (p==NULL) {
        return string("NULL");
    }
    char temp[256];
    sprintf(temp, "%p", p);
    return string(temp);
}

/*static*/ string Util::toString(const void *p) {
    if (p==NULL) {
        return string("NULL");
    }
    char temp[256];
    sprintf(temp, "%p", p);
    return string(temp);
}

/*static*/ string Util::toString(const int *s, int s_length) {
	stringstream ret;
	int i;
	char temp[256];
	ret << "[";
	for(i=0; i<s_length; i++) {
		if(i>0)
			ret << ", ";
		sprintf(temp, "%d", s[i]);
		ret << temp;
	}
	ret << "]";
	return ret.str();
}

/*static*/ string Util::toString(const unsigned int *s, int s_length) {
	stringstream ret;
	int i;
	char temp[256];
	ret << "[";
	for(i=0; i<s_length; i++) {
		if(i>0)
			ret << ", ";
		sprintf(temp, "%u", s[i]);
		ret << temp;
	}
	ret << "]";
	return ret.str();
}

/*static*/ string Util::toString(const unsigned long int *s, int s_length) {
	stringstream ret;
	int i;
	char temp[256];
	ret << "[";
	for(i=0; i<s_length; i++) {
		if(i>0)
			ret << ", ";
		sprintf(temp, "%lu", s[i]);
		ret << temp;
	}
	ret << "]";
	return ret.str();
}

/*static*/ string Util::buildParamList(const char * const * s, int s_length, int bracketType) {
	stringstream ret;
	int i;
	switch (bracketType)
	{
		case SQUARE_BRACKETS: ret << "["; break;
		case SQUIGGLY_BRACKETS: ret << "{"; break;
		default: ret << "{"; break;
	}
	for(i=0; i<s_length; i++) {
		if(i > 0) {
			ret << ", ";
		}
		ret << (s[i] ? "NULL" : s[i]);
	}
	switch (bracketType)
	{
		case SQUARE_BRACKETS: ret << "]"; break;
		case SQUIGGLY_BRACKETS: ret << "}"; break;
		default: ret << "}"; break;
	}
	return ret.str();
}

/*static*/ string Util::buildParamList(string * s, int s_length, int bracketType) {
	stringstream ret;
	int i;
	switch (bracketType)
	{
		case SQUARE_BRACKETS: ret << "["; break;
		case SQUIGGLY_BRACKETS: ret << "{"; break;
		default: ret << "{"; break;
	}
	for(i=0; i<s_length; i++) {
		if(i > 0) {
			ret << ", ";
		}
		ret << (s[i]=="" ? "NULL" : s[i]);
	}
	switch (bracketType)
	{
		case SQUARE_BRACKETS: ret << "]"; break;
		case SQUIGGLY_BRACKETS: ret << "}"; break;
		default: ret << "}"; break;
	}
	return ret.str();
}

/*static*/ string Util::trim(string str) {
    string whitespace = string(" \t\f\v\n\r");
    size_t start = str.find_first_not_of(whitespace);
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end+1);
}

/*static*/ string Util::removeQuotes(string keyValue) {
    if (keyValue.length()>2
        && (keyValue[0]=='\'' || keyValue[0]=='`' || keyValue[0]=='"') 
        && keyValue[0] == keyValue[keyValue.length()-1]) 
    {
        return keyValue.substr(1, keyValue.length()-2);
    }
    else {
        return keyValue;
    }
}

/*static*/ string Util::escapeQuotes(string str) {
	string::size_type pos = 0;
	    while ( (pos = str.find("\"", pos)) != string::npos ) {
	        str.replace( pos, 1, "\\\"" );
	        pos+=2;
	    }
	return str;
}

/*static*/ string Util::formatIP(char *addr) {
    char temp[64];
    sprintf(temp, "%d.%d.%d.%d", (0xFF & addr[0]), (0xFF & addr[1]), (0xFF & addr[2]), (0xFF & addr[3]));
    return string(temp);

}

/* Convert from IP address string e.g. "192.168.1.15" to 4-byte address. Opposite of formatIP.
 *
 */
/*static*/ void Util::parseIP(string hostAddr, char *host) {
    int pos1 = hostAddr.find_first_of('.');
    int pos2 = hostAddr.find_first_of('.', pos1+1);
    int pos3 = hostAddr.find_first_of('.', pos2+1);
    string s1 = hostAddr.substr(0,pos1);
    string s2 = hostAddr.substr(pos1+1,pos2-pos1-1);
    string s3 = hostAddr.substr(pos2+1,pos3-pos2-1);
    string s4 = hostAddr.substr(pos3+1);
    //logger.info << "parsed IP address " << hostAddr << "is [" << s1 << "." << s2 << "." << s3 << "." << s4 << "]" << endl;
    host[0] = 0xFF & atoi(s1.c_str());
    host[1] = 0xFF & atoi(s2.c_str());
    host[2] = 0xFF & atoi(s3.c_str());
    host[3] = 0xFF & atoi(s4.c_str());
}


/*static*/ const char* Util::createException(const char *message) {
    xlog.warn(string("Throwing exception: ") + string(message));
    return message;
}

/*static*/ string Util::getElapsedTimeString(struct timeval *tvStart, struct timeval *tvEnd) {
    struct timeval tvDiff; 
    timersub(tvEnd, tvStart, &tvDiff);
    char temp[64];
    sprintf(temp, "%ld.%06ld", tvDiff.tv_sec, tvDiff.tv_usec);
    return string(temp);
}


} // namespace util
