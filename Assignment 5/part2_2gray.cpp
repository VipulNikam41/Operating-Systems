#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>

#define vvi vector<vector<int>>
#define vi vector<int>
#define SNAME_READ "/readimage16"
#define SNAME_WRITE "/writeimage16"

using namespace std;

sem_t *write_sem;
sem_t *read_sem;

// int pixels = 0;
int MAX = 4096;
int filter[3][3] = { {1, 2, 1}, {2, -13, 2}, {1, 2, 1}};
string file_name1;
int v1, v2, v3;
string file_name2;

int h = 0, w = 0;

int ** gray;
int ** r;
int ** g;
int val;
int ** b;
int ** edges;

void readImage(string file_name1) 
{
    ifstream fin(file_name1);
    string first_line;

    fin >> first_line >> w >> h >> val;
    r = g = b = new int*[h];

    for(int i=0; i<h; ++i)
    {
        r[i] = g[i] = b[i] = new int[w];
        for(int j=0; j < w; ++j) 
        {
            fin >> r[i][j];
            fin >> g[i][j];
            fin >> b[i][j];
        }
    }
    fin.close();
}

void read_write(string h_str, string w_str, char *pixel)
{
    sem_wait(read_sem);
    strcpy(pixel, h_str.c_str());
    sem_post(write_sem);

    sem_wait(read_sem);
    strcpy(pixel, w_str.c_str());
    sem_post(write_sem);

    shmdt(pixel);
}
void gray_write(string file_name2) 
{
    key_t my_key = ftok("shmfile", 65);
    int shmid = shmget(my_key, sizeof(char), 0666|IPC_CREAT);
    char *pixel = (char *) shmat(shmid,(void*)0,0);
    string h_str = to_string(h);
    string w_str = to_string(w);

    read_write(h_str, w_str, pixel);

    my_key = ftok("shmfile_image", 65); // ftok function is used to generate unique key
    shmid = shmget(my_key, w * h * sizeof(int) + 1, 0666|IPC_CREAT); // shmget returns an ide in shmid
    int *pixels = (int *) shmat(shmid,(void*)0,0); // shmat to join to shared memory

    for(int i=0; i < h; ++i) 
    {
        for(int j = 0; j < w; ++j)
        {
            int *fnl = pixels + i*w + j;
            
            v1 = 0.3*r[i][j];
            v2 = 0.59*g[i][j];
            v3 = 0.11*b[i][j];

            *(fnl) = (int) (v1 + v2 + v3);
        }
        sem_post(write_sem);
    }
    shmdt(pixels);
}

int main(int argc, char** argv) 
{
    if(argc < 2)
    {
        printf("Incorrect input\n");
        return 0;
    }

    write_sem = sem_open(SNAME_WRITE, O_CREAT, 0644, 0);  // Init value 0
    read_sem = sem_open(SNAME_READ, O_CREAT, 0644, 1); // Init value 1

    file_name2 = argv[2];

    readImage(argv[1]);
    gray_write(file_name2);

    return 0;
}