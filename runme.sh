source fill_first_file.sh > A.dat
make
./sparse A.dat B.dat
gzip -k A.dat
gzip -k B.dat
gzip -cd B.dat.gz | ./sparse C.dat
./sparse A.dat D.dat -b 100
stat --printf 'file %n: %b blocks\n' *.dat *.gz > result.txt