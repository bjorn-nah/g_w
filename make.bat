@echo off

set name="g_w.nes"

set CC65_HOME=..\

cc65 -Oi g_w.c --add-source
ca65 crt0.s
ca65 g_w.s

ld65 -C nrom_128_horz.cfg -o %name% crt0.o g_w.o nes.lib

pause

del *.o
del g_w.s

C:\dev\nes\fceux\fceux.exe %name%