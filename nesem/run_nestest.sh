#!/bin/bash
./phlem nestest.nes test > trace.txt
 sed -i 's/[ \t]*$//' trace.txt
 sed -i 's/@.*$//' trace.txt
diff trace.txt nestest.log > diff.txt
diff_file_size=$(du -k diff.txt | cut -f1)

if [ $diff_file_size -eq 0 ]
then
  echo "PASS"
else
  echo "FAIL"
  vimdiff trace.txt nestest.log
fi
