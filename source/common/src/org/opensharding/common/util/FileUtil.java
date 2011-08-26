package org.opensharding.common.util;

import java.io.*;
import java.net.URI;
import java.net.URISyntaxException;

public class FileUtil {
    
    public static String readFile(File file) throws IOException {
        StringBuilder buf = new StringBuilder();
        BufferedReader r = new BufferedReader(new FileReader(file));
        String line = r.readLine();
        while (line != null) {
            buf.append(line).append('\n');
            line = r.readLine();
        }
        r.close();
        return buf.toString();
    }

    public static void writeFile(File file, String data) throws IOException {
        FileOutputStream os = new FileOutputStream(file);
        os.write(data.getBytes());
        os.close();
    }

    public static void copyFile(File src, File dest) throws IOException {
        InputStream is = new FileInputStream(src);
        OutputStream os = new FileOutputStream(dest);
        byte buffer[] = new byte[1024];
        int r;
        while ((r=is.read(buffer))!=-1) {
            os.write(buffer, 0, r);
        }
        os.close();
        is.close();
    }

    /**
     * Create File object to represent string, which can either be a relative path or an absolute URL.
     * 
     * @param strFile
     * @return
     * @throws URISyntaxException
     */
    public static File getFileFromString(String strFile) throws URISyntaxException {
        if (strFile.startsWith("http:") || strFile.startsWith("https:") || strFile.startsWith("file:")) {
            return new File(new URI(strFile));
        }
        else {
            return new File(strFile);
        }
    }
}
