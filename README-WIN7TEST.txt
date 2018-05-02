hp2xx on Windows7. 2012/Nov/17

hp2xx is failing to run on Windows 7.

hp2xx -- opening temporary file: Invalid argument

This error due to try to make temporary file at drive root (C:\).
See : http://sourceforge.net/p/gnuwin32/discussion/74807/thread/903411c4/

to fix it...

1.Get hp2xx-3.4.4-src-MINGW.zip from test builds.

2.Install MinGW (with MYSYS component)

3.Add PATH \MinGW\bin and \MinGW\msys\1.0\bin

4.Get zlib library and header from 	http://gnuwin32.sourceforge.net/downlinks/zlib-lib-zip.php

5.Copy all ".h" files to MinGW's "include" directry, and "libz.a" to "lib" directry.

6.Change line 693 "if ((pg->td = fopen("hp2xx.$$$", "w+b")) == NULL)" in hp2xx.c

7.Compile from command prompt "mingw32-make all"

8.Now, you can use hp2xx on Windows7.

If you want the binary, see the png14 directory.

7m4mon@gmail.com