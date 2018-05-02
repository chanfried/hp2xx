03-28-2011

This is test source of hp2xx for the gnuwin32 project.
http://sourceforge.net/projects/getgnuwin32/files/getgnuwin32/test%20builds/

The sources directory has a custom makefile for mingw.
Change the CP var if you don't have gnuwin32 cp.exe


To compile from msys/rxvt
-
cd sources
make all


To compile from command prompt
-
cd sources
mingw32-make all



If the make is successful it will copy hp2xx.exe 
to the png14 directory. Read the README in the 
png14 directory for libpng and zlib info.

Run hp2xx.exe in the png14 directory for testing.


Known issues:
The tmpfile() issue has not yet been fixed.
Output doesn't display properly in rxvt terminal



Jay Satiro on behalf of the GetGnuWin32 project
<raysatiro@users.sourceforge.net>
