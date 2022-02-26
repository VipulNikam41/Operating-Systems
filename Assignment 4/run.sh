cp system.c /usr/src/minix/kernel/;
cp schedule.c /usr/src/minix/servers/sched/;
cd /usr/src/;
make build MKUPDATE=yes >log.txt 2>log.txt