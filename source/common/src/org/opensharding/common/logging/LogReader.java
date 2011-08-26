package org.opensharding.common.logging;

import java.io.*;
import java.text.SimpleDateFormat;
import java.util.*;

/**
 * A reader for log files, supporting:
 * 
 * Tail entries
 * Read entries from a specified timestamp
 * Read n lines from a specified entry position
 * Grep entries for a string
 * 
 * Log entries must start with a header on a new line.
 * All header fields must start/end with []. 
 * The first header field must be a timestamp.
 * can be any recognizable date format string (see SimpleDateFormat).
 * The date format must contain padded values such as:
 * 
 * MMM (3 char month)
 * dd (2 digit day of month)
 * YYYY (4 digit year)
 * HH (2 digit hour, 24 hour format) or hh (2 digit hour, 12 hour format)
 * mm (2 digit minutes)
 * ss (2 digit seconds)
 * aa (AM/PM marker if 12 hour format)
 * 
 * A new log entry is identified by reading the first few bytes of the timestamp,
 * from the top of the file or after a \n.
 * 
 * The LogReader.properties file must be on the classpath, and specifies the entry format,
 * using the same syntax as Log4J.
 * 
 * @author coryisaacson
 *
 */
public class LogReader {

	// Properties file.
	final static String PROPFILE = "LogReader.properties";
	
	// Entry timestamp format.
	static String entryTimestampFormatString = null;

	// Tail size, the number of bytes from the end of the file to start tailing from.
	static long tailSize = 100000;
	
	// Entry prefix date format.
	static SimpleDateFormat entryPrefixFormat = null;
	
	// The log file itself.
	private RandomAccessFile logFile = null;
		
	// The prefix format is turned into a 2 dim array of chars for fast recognition.
	static char[][] prefixPattern = null;
	
	// Recognized date chars.
	static char[] dateChars = {'M','d','y','h','H','m','s','a'};
	static char[] digitChars = {'0','1','2','3','4','5','6','7','8','9'};
	
	// The length of bytes to determine if this is a new log entry or not.
	static int MATCHENTRYLENGTH = 4;
	
	// Buffers.
	static int MAXLOGENTRYSIZE = 2048;
	private byte[] entryBuffer = null;
	private byte[] prefixBuffer = null;
	
	// Filter match buffer.
	static int MAXFILTERMATCHBUFFERSIZE = 1024;
	private byte[] filterMatchBuffer = null;
	private int filterMatchLength = -1;
	
	// Buffer pool for reading chunks of the file.
	final static int MAXREADBUFFERSIZE = 8092;
	private byte[] readBuffer = null;
//	private int readBufferPos = -1;
	private int readBufferLength = -1;
	private long readBufferStart = -1;
	private long readBufferEnd = -1;
		
	// Contral variables for log entry navigation. Using these vars the position of the current and
	// last read logEntry can be determined. They also can be compared to determine which direction
	// the entries are being read in (FORWARD or BACKWARD).
	// The starting position of the last log entry that was read.
	long lastEntryStartPos = -1;
	// The starting position of the next log entry about to be read.
	long nextEntryStartPos = -1;
	
	// File position to support custom seek and read methods.
	private long filePos = -1;
	
	// File length. Used to avoid getting the length from the file, which is very expensive.
	private long fileLength = -1;
	
	// Filter string.
	private String filter = null;
		
	// Static initializer.
	static {
		try {
			InputStream inputStream = LogReader.class.getResourceAsStream("/" + PROPFILE);
			if(inputStream == null) {
				throw new Exception("The LogReader.properties file must be on the class path.");
			}
			Properties props = new Properties();
			props.load(inputStream);
			
			entryTimestampFormatString = props.getProperty("entry-prefix-format");
			System.out.println("entryTimestampFormatString: " + entryTimestampFormatString);
			if(entryTimestampFormatString == null) {
				throw new Exception("The entry-prefix-format was not found in LogReader.properties.");
			}
			String MAXLOGENTRYSIZEString = props.getProperty("max-log-entry-size");
			if(MAXLOGENTRYSIZEString != null || !MAXLOGENTRYSIZEString.equals("")) {
				MAXLOGENTRYSIZE = Integer.parseInt(MAXLOGENTRYSIZEString);
			}
			
			String tailSizeString = props.getProperty("tail-size");
			if(tailSizeString != null) {
				tailSize = Long.parseLong(tailSizeString);
			}
			
			// Load the prefixPattern.
			loadPrefixPattern();
			
			// Load the date format, if its invalid an Exception will be thrown.
			entryPrefixFormat = new SimpleDateFormat(entryTimestampFormatString);
			
		} catch (Exception e) {
			// TODO: Work out how to log messages for errors from the log reader.
			e.printStackTrace();
		}
		
	}
	
	public LogReader(String logFileName) {
		
		try {
			
			if(entryPrefixFormat == null) {
				throw new Exception("The entry-prefix-format in LogReader.properties was not found or is invalid.");
			}
			
			// Open the file.
			File file = new File(logFileName);
			if(!file.exists()) {
				throw new Exception("The log file does not exist. logFileName: " + logFileName);
			}
			logFile = new RandomAccessFile(file, "r");

			// Set the file pointer to the beginning.
			seek(0);
			fileLength = logFile.length();
			
			// Init the buffers.
			prefixBuffer = new byte[MATCHENTRYLENGTH];
			entryBuffer = new byte[MAXLOGENTRYSIZE];
			readBuffer = new byte[MAXREADBUFFERSIZE];
			filterMatchBuffer = new byte[MAXFILTERMATCHBUFFERSIZE];
			
		} catch (Exception e) {
			// TODO: Work out how to log messages for errors from the log reader.
			e.printStackTrace();
		}
	}
	
	/**
	 * Load the prefix pattern into the 2 dim char array.
	 */
	private static void loadPrefixPattern() throws Exception {
		int monthPos = -1;
		int amPmPos = -1;
		
		if(entryTimestampFormatString.length() < MATCHENTRYLENGTH) {
			throw new Exception("The entry timestamp must be at least " + MATCHENTRYLENGTH + " bytes long.");
		}
		// The first array dim is the length of the format pattern.
		prefixPattern = new char[MATCHENTRYLENGTH][];
		
		// Read each char of the pattern and build the nested arrays.
		for(int i=0;i<MATCHENTRYLENGTH;i++) {
			// If this is a valid date char.
			char c = entryTimestampFormatString.charAt(i);
			// Handle each type of char. For recognized date chars, set each unique char in the array.
			if(c == 'M') {
				monthPos++;
				if(monthPos == 0) {
					prefixPattern[i] = new char[8];
					prefixPattern[i][0] = 'J';
					prefixPattern[i][1] = 'F';
					prefixPattern[i][2] = 'M';
					prefixPattern[i][3] = 'A';
					prefixPattern[i][4] = 'S';
					prefixPattern[i][5] = 'O';
					prefixPattern[i][6] = 'N';
					prefixPattern[i][7] = 'D';
					
				} else if(monthPos == 1) {
					prefixPattern[i] = new char[6];
					prefixPattern[i][0] = 'a';
					prefixPattern[i][1] = 'e';
					prefixPattern[i][2] = 'p';
					prefixPattern[i][3] = 'u';
					prefixPattern[i][4] = 'c';
					prefixPattern[i][5] = 'o';
					
				} else if(monthPos == 2) {
					prefixPattern[i] = new char[11];
					prefixPattern[i][0] = 'n';
					prefixPattern[i][1] = 'b';
					prefixPattern[i][2] = 'r';
					prefixPattern[i][3] = 'y';
					prefixPattern[i][4] = 'n';
					prefixPattern[i][5] = 'l';
					prefixPattern[i][6] = 'g';
					prefixPattern[i][7] = 'p';
					prefixPattern[i][8] = 't';
					prefixPattern[i][9] = 'v';
					prefixPattern[i][10] = 'c';
					
				}
				
			} else if(c == 'd') {
				prefixPattern[i] = digitChars;
			} else if(c == 'y') {
				prefixPattern[i] = digitChars;
			} else if(c == 'H') {
				prefixPattern[i] = digitChars;
			} else if(c == 'h') {
				prefixPattern[i] = digitChars;
			} else if(c == 'm') {
				prefixPattern[i] = digitChars;
			} else if(c == 's') {
				prefixPattern[i] = digitChars;
			} else if(c == 'a') {
				amPmPos++;
				if(amPmPos == 0) {
					prefixPattern[i] = new char[4];
					prefixPattern[i][0] = 'a';
					prefixPattern[i][1] = 'A';
					prefixPattern[i][2] = 'p';
					prefixPattern[i][3] = 'P';
				} else if(amPmPos ==1) {
					prefixPattern[i] = new char[2];
					prefixPattern[i][0] = 'm';
					prefixPattern[i][0] = 'M';
				}
				 
			// Else its a literal char.
			} else {
				prefixPattern[i] = new char[1];
				prefixPattern[i][0] = c;
			}
		}
	}
	
	/**
	 * Set up the reader to start at the top of the file.
	 * @throws Exception
	 */
	public void moveToStart() throws Exception {
		seek(0);
		lastEntryStartPos = -1;
		nextEntryStartPos = -1;
	}
	
	/**
	 * Set up the reader for tailing, to the position of the tail-size property
	 * length from the end of the file.
	 * @throws Exception
	 */
	public void moveToTail() throws Exception {
		fileLength = logFile.length();
		if(fileLength < tailSize) {
			seek(0);
		} else {
			seek(fileLength - tailSize);
		}
		lastEntryStartPos = -1;
		nextEntryStartPos = -1;
		System.out.println("moveToTail: filePos: " + filePos);
	}
	
	/**
	 * Update the file length, used for continuous tailing.
	 * This should not be called too frequently as it is an expensive IO operation.
	 * @return Returns true if the file length has changed, false if not.
	 */
	public boolean updateFileLength() throws Exception {
		boolean fileLengthChanged = false;
		long tempFileLength = logFile.length();
		if(tempFileLength != fileLength) {
			fileLengthChanged = true;
		}
		fileLength = tempFileLength;
		
		return fileLengthChanged;
	}
	
	/**
	 * Convenience method that always returns a result if there is an entry, or null if none found.
	 * @return
	 * @throws Exception
	 */
	public String findNextEntry() throws Exception {
		return findNextEntry(true);
	}
	
	/**
	 * Find the next log entry. Returns null if no more entries in the file.
	 * @param returnResult Indicate if a String result should be returned.
	 * @return
	 * @throws Exception
	 */
	public String findNextEntry(boolean returnResult) throws Exception {
		
		String entry = null;
		boolean inEntry = false;
		int entryLength = 0;
		
//		System.out.println("findNextEntry: top: filePos: " + filePos + " nextEntryStartPos: " + nextEntryStartPos);
		
		// Apply any filter.
		
		// If this is the top of the file, check for an entry prefix.
		if(filePos == 0) {
			// Perform the prefix match.
			nextEntryStartPos = matchEntryPrefix();
			if(nextEntryStartPos != -1) {
				lastEntryStartPos = 0;
			} else {
				lastEntryStartPos = -1;	
			}
		}
		
		// If a next entry is set, start entry.
		if(nextEntryStartPos != -1) {
			// Move to the start of the next entry.
			seek(nextEntryStartPos);
			
			// Start the next entry.
			entryLength = 0;
			inEntry = true;
			
			// Reset the nextEntryStartPos. The current entry becomes the last entry.
			lastEntryStartPos = nextEntryStartPos;
			nextEntryStartPos = -1;

		}

		// Read the file.
		while(true) {
			byte b = readByte();

			// If at the end of the file, either return the entry or stop
			if(b == -1) {
				if(inEntry) {
					if(returnResult) entry = new String(entryBuffer,0, entryLength);
					inEntry = false;
					entryLength = 0;
				}
				break;
			}

			// If this is a \n char, check for an entry prefix. If one is found, set the lastEntryStartPos and nextEntryStartPos.
			if((char)b == '\n') {
				nextEntryStartPos = matchEntryPrefix();
			}
				
			// If not yet in an entry and a there is a next entry, start the entry.
			if(!inEntry && nextEntryStartPos != -1) {

				// Start the next entry.
				entryLength = 0;
				inEntry = true;
				
				// Reset the nextEntryStartPos. The current entry becomes the last entry.
				lastEntryStartPos = nextEntryStartPos;
				nextEntryStartPos = -1;
								
			// Else inEntry and the next entry is found, complete the current entry and init inEntry flag.
			} else if(inEntry && nextEntryStartPos != -1) {
				if(returnResult) entry = new String(entryBuffer,0, entryLength);
				
				// Init inEntry flag.
				inEntry = false;
				entryLength = 0;
				break;
			}
				
			// If this is the max length of an entry.
			if(entryLength == MAXLOGENTRYSIZE) {
				if(returnResult) entry = new String(entryBuffer,0, entryLength);
				inEntry = false;
				entryLength = 0;
				break;
				
				// Keep reading until the next entry is found or the EOF is reached.
			}
			
			// If in an entry, and there is no next entry pending, add the char to the entryBuffer.
			if(inEntry) {
				entryBuffer[entryLength] = b;
				entryLength++;
			}
		}
		
		System.out.println("findNextEntry: entry.length: " + (entry != null ? entry.length() : null) 
				+ " filePos: " + filePos 
				+ " nextEntryStartPos: " + nextEntryStartPos + " lastEntryStartPos: " + lastEntryStartPos);
		return entry;
				
	}

	/**
	 * Convenience method that always returns a result if there is an entry, or null if none found.
	 * @return
	 * @throws Exception
	 */
	public String findPriorEntry() throws Exception {
		return findPriorEntry(true);
	}
	
	/**
	 * Get the prior entry to the current position in the file.
	 * This works by reading backwards until a \n is found,
	 * then determining if it is the start of an entry, and 
	 * if so call findNextEntry.
	 * @param returnResult Return a result string, or null if not found.
	 * @return
	 * @throws Exception
	 */
	public String findPriorEntry(boolean returnResult) throws Exception {
		
		String entry = null;
		char c = 0;
		
//		System.out.println("findPriorEntry: lastEntryStartPos: " + lastEntryStartPos + " filePos: " + filePos);
		
		// Clear any pending entry.
		if(nextEntryStartPos != -1) {
			nextEntryStartPos = -1;
		}
		
		// If there is no last entry, return.
		if(lastEntryStartPos == -1) {
			return null;
		}
		
		// If there is a last entry, and it is at the top of the file, return null as no prior entries.
		if(lastEntryStartPos == 0) {
			nextEntryStartPos = 0;
			lastEntryStartPos = -1;
			return null;
		}
		
		// If there is a last entry set, start searching before the lastEntryStartPos.
		if(lastEntryStartPos != -1) {
			// Move to 2 bytes before the last entry, before the start of the last entry and the \n char just before the entry.
			seek(lastEntryStartPos - 2);
		}
		
		
		// Read the file in reverse.
		while(true) {
			byte b = reverseReadByte();

			// If at the beginning of the file, get the next entry.
			if(b == -1) {
				filePos = 0;
				break;
			}

			// If this is a \n char, check for an entry. If an entry found, set it up as the pending entry.
			if((char)b == '\n') {
				
				// Advance 2 bytes to start after the \n char.
				seek(filePos + 2);
				
				// Check to see if an entry is found. This sets the nextEntryStartPos.
				nextEntryStartPos = matchEntryPrefix();
				if(nextEntryStartPos != -1) {
//					System.out.println("findPriorEntry: after \\n filePos: " + filePos + " nextEntryStartPos: " + nextEntryStartPos);
					break;
				}
				
				// If no entry found, reset the position 2 bytes back to continue searching.
				seek(filePos - 2);
			}
		}
		
		System.out.println("findPriorEntry: before findNextEntry: nextEntryStartPos: " + nextEntryStartPos 
				+ " lastEntryStartPos: " + lastEntryStartPos + " filePos: " + filePos);
		entry = findNextEntry(returnResult);

		return entry;
	}
	
	/**
	 * Indicates there is a next log entry pending. 
	 * @return
	 */
	public boolean isNextEntry() {
		if(nextEntryStartPos != -1) {
			return true;
		} else {
			return false;
		}
	}
	
	/**
	 * Set a filter string. Only normal chars can be filtered, international strings not supported.
	 * @param filter
	 */
	public void setFilter(String filter) {
		
		// Set the filter string and length.
		this.filter = filter;
		this.filterMatchLength = filter.length();
		
		// Build the filter match buffer.
		for(int i=0;i<filterMatchLength;i++) {
			filterMatchBuffer[i] = (byte)filter.charAt(i);
//			System.out.println("setFilter: " + filterMatchBuffer[i]);
		}
	}
	
	/**
	 * Clear the filter.
	 */
	public void clearFilter() {
		this.filter = null;
		this.filterMatchLength = -1;
	}
	
	/**
	 * Find the next entry using a filter. 
	 * @return entry.
	 * @throws Exception
	 */
	public String applyFilter() throws Exception {
		
		// If no filter, return.
		if(filter == null) {
			return null;
		}

		if(filePos == -1) {
			seek(0);
		}
		
		// Attempt to find the next match.
		long matchPos = matchFilter();
		
		// If no match, return.
		if(matchPos == -1) {
			return null;
		}
		
		// Find and return the prior entry (just above the match).
		lastEntryStartPos = matchPos + 2;
		seek(lastEntryStartPos);
		
		return findPriorEntry(true);
		
	}
		
	/**
	 * Provides a buffered reader, for reading forward in the file. 
	 * @return byte or -1 if the end of file has been reached.
	 */
	private byte readByte() throws Exception {
		
		byte b = -1;

		// Read the next byte from the buffer and increment the filePos.
		b = read();
		filePos++;
		
		return b;

	}
	
	/**
	 * Read the bytes in reverse, using a buffered reader.
	 * @return
	 * @throws Exception
	 */
	private byte reverseReadByte() throws Exception {
		
		byte b = -1;
		
		// Read the next byte from the buffer and decrement the filePos.
		b = read();
		filePos--;
		
		return b;
					
	}
	
	/**
	 * Read a byte from the file. Sets the readBufferStart, readBufferEnd, and readBufferLength
	 * variables. The readBuffer is set to the length of the buffer, or -1 if no more bytes.
	 * The filePos is used to determine which byte to read.
	 * Reads are performed in any direction compared to the readBuffer.
	 * @return The byte or -1 if read past the beginning or end of the file.
	 * @throws Exception
	 */
	private byte read() throws IOException {

		long blockstart = -1;
		
		// If read past the beginning of the file, return -1.
		if(filePos < 0) {
			readBufferLength = -1;
			readBufferStart = -1;
			readBufferEnd = -1;
			filePos = 0;
			
			return -1;
		}
		
		// TODO: Implement getting new file length for tail function.
		// If read past the end of the file, return -1.
		if(filePos + 1 >= fileLength) {
			readBufferLength = -1;
			readBufferStart = -1;
			readBufferEnd = -1;
			filePos = fileLength - 1;
			
			return -1;
		}

		// Load the buffer if required.
		if(filePos < readBufferStart || filePos > readBufferEnd) {
			
			blockstart = (filePos / MAXREADBUFFERSIZE) * MAXREADBUFFERSIZE;
			
			logFile.seek(blockstart);
			readBufferLength = logFile.read(readBuffer);
			readBufferStart = blockstart;
			readBufferEnd = readBufferStart + readBufferLength - 1;

//			if(filePos % (8092 * 100) == 0) {
//				System.out.println("loadReadBuffer: filePos: " + filePos  
//						+ " readBufferStart: " + readBufferStart + " readBufferEnd: " + readBufferEnd 
//						+ " blockstart: " + blockstart);
//			}
		}
		
		// Read and return the byte.
		int bufferPos = (int)(filePos - readBufferStart);

		return readBuffer[bufferPos];
		
	}
	
	
	/**
	 * Seek to a position in the file.
	 * @param seekPos
	 * @throws Exception
	 */
	private void seek(long seekPos) throws Exception {
		// Simply set the filePos.
		filePos = seekPos;
	}
		
	/**
	 * Match a char in an array.
	 * @param c
	 * @param matchArray
	 * @return
	 */
	private boolean matchChar(char c, char[] matchArray) throws Exception {
		long pos = logFile.getFilePointer();
		for(int i=0;i<matchArray.length;i++) {
			if(c == matchArray[i]) {
				return true;
			}
		}
		return false;
	}

	/**
	 * Reads bytes from the current position and returns true if this is an entry prefix.
	 * This method should be called each time a \n char is found, or at the start of the file.
	 * @return The start position of the match or -1 if no match.
	 * @throws Exception
	 */
	private long matchEntryPrefix() throws Exception {
		
		// Mark the current position in the file.
		long startPos = filePos;
		
		// If there are not enough bytes in the file for the prefix length, return false.
		if((filePos + MATCHENTRYLENGTH) > fileLength) {
			return -1;
		}
				
		// Determine if this is a prefix string.
		for(int i=0;i<MATCHENTRYLENGTH;i++) {
			
			byte b = readByte();
			char c = (char)b;
			if(b == -1) {
				return -1;
			}
			
			// If the char is not a prefix char, reset the position in the file and return -1.
			if(!matchChar((char)b, prefixPattern[i])) {
				seek(startPos);
				return -1;
			}
			
			// Add the char to the prefixBuffer.
			prefixBuffer[i] = b;
		}
		
		// Position the buffer at the start of the match.
		seek(startPos);
		
		// Return the startPos of the match.
		return startPos;
	}
	
	/**
	 * Find the next match in the file for the filter. 
	 * @return The start position of the match or -1 if no match.
	 * @throws Exception
	 */
	private long matchFilter() throws Exception {

		System.out.println("matchFilter: filter: " + filter + " filterMatchLength: " + filterMatchLength + " filePos: " + filePos) ;
		
		// Mark the current position in the file.
		long startPos = filePos;
		long ctr = -1;
		boolean matchFound = false;
		int matchPos = 0; 
		
		// If there are not enough bytes in the file for the match length, return -1.
		if((filePos + filterMatchLength) > fileLength) {
			return -1;
		}
		
		// Read each byte from the current position.
		while(true) {
			byte b = readByte();
			if(b == -1) {
				return -1;
			}
			
			// Perform a quick check to see if the first byte is a match. If so, perform the full match.
			if(b == filterMatchBuffer[matchPos]) {
				matchPos++;
			} else {
				matchPos = 0;
			}
		
			// If we have a complete match, break.
			if(matchPos + 1 == filterMatchLength) {
				break;
			}
			
		}
		
		System.out.println("matchFilter: foundMatch: " + matchFound + " filePos: " + filePos 
				+ " matchPos: " + matchPos + " filterMatchLength: " + filterMatchLength);
		// Return the filePos of the match.
		return filePos;
	}
	
	public void testReadPerformance() throws Exception {
		seek(0);
		long startTime = System.currentTimeMillis();
		
		while(true) {
			byte b = readByte();
			if(b == -1) {
				break;
			}
			
			if(filePos % 1000000 == 0) {
				System.out.println("testReadPerformance: filePos: " + filePos);
			}
		}
		
		System.out.println("testReadPerformance: filePos: " + filePos + " elapsed time: " + (System.currentTimeMillis() - startTime));
	}

	
}
