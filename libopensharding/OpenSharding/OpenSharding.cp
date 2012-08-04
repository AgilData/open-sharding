/*
 *  OpenSharding.cp
 *  OpenSharding
 *
 *  Created by Cory Isaacson on 8/1/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "OpenSharding.h"
#include "OpenShardingPriv.h"

void OpenSharding::HelloWorld(const char * s)
{
	 OpenShardingPriv *theObj = new OpenShardingPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void OpenShardingPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

