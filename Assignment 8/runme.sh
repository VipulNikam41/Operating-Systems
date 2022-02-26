./clean.sh

gcc optimal.c -o optimal
./optimal > optimal.dat
gnuplot optimal.p

gcc clock.c -o clock
./clock > clock.dat
gnuplot clock.p

gcc lru.c -o lru
./lru > lru.dat
gnuplot lru.p

gcc random.c -o random
./random > random.dat
gnuplot random.p