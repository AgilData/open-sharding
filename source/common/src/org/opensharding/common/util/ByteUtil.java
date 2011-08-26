package org.opensharding.common.util;

import java.text.DecimalFormat;
import org.apache.commons.codec.binary.*;

/**
 * Byte manipulation utility class.
 * @author coryisaacson
 *
 */
public class ByteUtil {

	/**
	 * Convert a short to a byte[].
	 * @param s
	 * @return
	 */
	public static byte[] toBytes(short s)
    {
		return new byte[] {
                (byte)(s >>> 8),
                (byte)s};

    }
 
	/**
	 * Convert a byte[] to a short.
	 * @param b
	 * @return
	 */
    public static short toShort(byte[] b)
    {
    	return (short)
    	((b[0] << 8)
    	+ (b[1] & 0xFF));

    }

	/**
	 * Convert an int to a byte[].
	 * @param i
	 * @return
	 */
	public static byte[] toBytes(int i)
    {
		return new byte[] {
                (byte)(i >>> 24),
                (byte)(i >>> 16),
                (byte)(i >>> 8),
                (byte)i};
    }

	/**
	 * Convert a byte[] to an int.
	 * @param b
	 * @return
	 */
    public static int toInt(byte[] b)
    {
    	return (b[0] << 24)
        	+ ((b[1] & 0xFF) << 16)
        	+ ((b[2] & 0xFF) << 8)
        	+ (b[3] & 0xFF);
    }
    
    /**
     * Format a byte array as a PostgreSQL binary string.
     * @param data
     * @return
     */
    public static String toPostgresBinaryString(byte[] data) {
    	
    	StringBuffer sb = new StringBuffer();
    	sb.append("E'");
    	for(byte b : data) {
    		
    		// Escape special chars to a 3 digit octal preceded by \\. 
    		if(b == 0 || b == 39 || b == 92 || b <= 31 || b >= 127) {
        		String octal = Integer.toOctalString(b);
        		
        		sb.append("\\\\");
        		if(octal.length() == 1) {
        			sb.append("00");
        		} else if(octal.length() == 2) {
        			sb.append("0");
        		}
        		sb.append(octal);
        		
        	// Else output the char value.
    		} else {
    			sb.append((char)b);
    		}
    	}
    	sb.append("'");
    	return sb.toString();
    }
    
    /**
     * Convert a byte array to a hex string.
     * @param data
     * @return
     */
    public static String toHexString(byte[] data) {
		StringBuilder sb = new StringBuilder();
		
		for(byte b : data) {
			int i = (int)b;
			// A byte value in Java goes from 0 - 127, and then 128 = -128, 129 = -127, ...-1 = 255.
			// A (int)b cast just takes the literal byte value, so -128 = -128, which gives a totally incorrect hex value.
			// The int value of each byte must be between 0 and 255 for correct conversion.
			// Therefore, for byte values < 0, add 256 to get the correct int value.
			if(i < 0) {
				i += 256;
			}
			String hexString = Integer.toHexString(i);
			if(hexString.length() == 1) {
				sb.append("0");
			}
			sb.append(hexString);
		}
				
		return sb.toString();
    }
    
    /**
     * Convert a hexString to a byte array.
     * @param hexString
     * @return
     */
    public static byte[] toBytes(String hexString) throws Exception {
    	int len = hexString.length();
    	if(len % 2 != 0) {
    		throw new Exception("The hexString is invalid. It must have a length divisible by 2.");
    	}
    	int arrayLen = len / 2;
    	byte[] data = new byte[arrayLen];
    	for(int i=0;i<arrayLen;i++) {
    		int intVal = Integer.parseInt(hexString.substring(i*2, (i*2)+2),16);
    		data[i] = (byte)intVal;
    	}
    	
    	return data;
    }

    /**
     * Encode byte data to base 64.
     * @param data
     * @return
     */
    public static String encodeBase64(byte[] data) {
    	String base64String = null;
    	
    	base64String = new String(Base64.encodeBase64(data));
    	
    	return base64String;
    }
    
    /**
     * Decide a base64 string to byte data.
     * @param base64String
     * @return
     */
    public static byte[] decodeBase64(String base64String) {
    	byte[] data = null;
    	
    	data = Base64.decodeBase64(base64String.getBytes());
    	
    	return data;
    }
}
