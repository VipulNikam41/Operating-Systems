# Part1:
echo "Output For Part 1"
g++ 11_alloc.cpp test_alloc.c -o alloc
./alloc
 
# Part2:
echo "Output For Part 2"
g++ -w 11_ealloc.cpp test_ealloc.c -o ealloc
./ealloc

# Trash
rm alloc ealloc