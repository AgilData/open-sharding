package org.opensharding.common.util;

import org.opensharding.common.logging.Logger;
import org.opensharding.common.logging.LoggerFactory;

/**
 * TODO: write javadoc
 *
 * @author Andy Grove
 */
public class StringUtil {
	
	private final static Logger logger = LoggerFactory.getLogger(StringUtil.class);
	
	// Values returned from identifyStringCase.
	public final static int UPPERCASE = 1;
	public final static int LOWERCASE = 2;
	public final static int PROPERCASE = 3;
	public final static int CAMELCASE = 4;


    //TODO: optimize to use StringBuilder

    public static String rpad(String str, int l) {
        while (str.length() < l) {
            str += " ";
        }
        return str;
    }

    public static String lpad(String str, int l) {
        while (str.length() < l) {
            str = " " + str;
        }
        return str;
    }
    
    /**
     * Escape all occurrences of escapeStr string with '\escapeStr'
     * @param str
     * @param escapeStr
     * @return
     */
    public static String escape(String str, String escapeStr) {
    	
    	if(str == null || str.length() == 0) {
    		return str;
    	}

    	StringBuilder sb = new StringBuilder(str);
    	
    	int pos = 0;
    	pos = sb.indexOf(escapeStr, pos);
    	while(pos != -1) {
    		// Only escape if not already escaped.
    		if(pos == 0) {
    			sb.insert(pos, '\\');	
    			
    		} else if(pos > 0 && pos < sb.length() && sb.charAt(pos-1) != '\\') {
    			
    			sb.insert(pos,'\\');
    		}
    		
    		pos += 2; // Skip the '\escapeChar'.
    		if(pos < sb.length()) {
        		pos = sb.indexOf(escapeStr, pos);
    		} else {
    			pos = -1;
    		}
    	}
    	
    	return sb.toString();
    }
    
    /**
     * Unescape a string, replacing all occurrences of '\escapeStr' with 'escapeStr'.
     * @param str
     * @param escapeStr
     * @return
     */
    public static String unescape(String str, String escapeStr) {
    	if(str == null || str.length() == 0) {
    		return str;
    	}
    	
    	str = str.replaceAll("\\\\" + escapeStr, escapeStr);
    	
    	return str;
    }
    
    /**
     * Evaluate the case of a given input string. Values returned are:
     * 
     * UPPERCASE if all chars are upper case
     * LOWERCASE if all chars are lower case
     * PROPERCASE if first char is upper case and mixed case for other chars.
     * CAMELCASE if first char is lowercase and mixed case for other chars. 
     * 
     * @param inputStr
     * @return
     */
    public static int identifyStringCase(String inputStr) {
    	
    	boolean foundUpper = false;
    	boolean foundLower = false;
    	boolean firstCharIsUpper = false;
    	for(int i=0;i<inputStr.length();i++) {
    		char c = inputStr.charAt(i);
    		if(i == 0 && Character.isUpperCase(c)) {
    			firstCharIsUpper = true;
    		}
    		if(Character.isUpperCase(c)) {
    			foundUpper = true;
    		}
    		if(Character.isLowerCase(c)) {
    			foundLower = true;
    		}
    	}
    	
    	// Determine the case.
    	int stringCase = -1;
    	if(foundUpper && !foundLower) {
    		stringCase = UPPERCASE;
    	} else if(!foundUpper && foundLower) {
    		stringCase =  LOWERCASE;
    	} else if(firstCharIsUpper && foundLower) {
    		stringCase =  PROPERCASE;
    	} else if(!firstCharIsUpper && foundUpper) {
    		stringCase =  CAMELCASE;
    	}
    	
    	return stringCase;
    	    	
    }

}
