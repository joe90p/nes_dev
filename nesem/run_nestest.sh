./phlem nestest.nes test > trace.txt
 sed -i 's/[ \t]*$//' trace.txt
 sed -i 's/@.*$//' trace.txt
vimdiff trace.txt nestest.log
