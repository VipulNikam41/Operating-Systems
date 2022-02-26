#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <thread>
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
int r1;

int h = 0, w = 0;

int ** gray;
int ** r;
int ** g;
int val;
int ** b;
int shmid;
int ** edges;

int complete_rows;

void allocateMatrix(char *pixel) 
{
	sem_wait(write_sem);
    h = atoi(pixel);
    sem_post(read_sem);

    sem_wait(write_sem);
    w = atoi(pixel);
    sem_post(read_sem);

}

void getDimensions() 
{
    key_t my_key = ftok("shmfile", 65);
    shmid = shmget(my_key, sizeof(char), 0666|IPC_CREAT);
    char *pixel = (char *) shmat(shmid,(void*)0,0);

    allocateMatrix(pixel);

  	shmdt(pixel);

    shmctl(shmid,IPC_RMID,NULL);

    // Dynamically allocate the 2D Array
    edges = gray = new int*[h];
    for(int i=0; i<h; ++i)
    {
        edges[i] = gray[i] = new int[w];
    }
    complete_rows = 0;
}

void processRow(int row) 
{
    r1 = row;
    for(int j=0; j < w; ++j) 
    {
        int pixel = 0;
        for(int x = 0; x<3; ++x)
        {
            for(int y = 0; y<3; ++y)
            {
                if(r1 + x >= h || j + y >= w)
                    continue;
                else
                    pixel = pixel + gray[r1+x][j+y] * filter[x][y];
            }
        }

        int al = min(pixel, 255);
        edges[r1][j] = max(al, 0);
    }
}

void detectEdges() 
{
    key_t my_key = ftok("shmfile_image", 65); // ftok function is used to generate unique key
    shmid = shmget(my_key, w * h * sizeof(int) + 1, 0666|IPC_CREAT); // shmget returns an ide in shmid

    int *pixel = (int *) shmat(shmid,(void*)0,0); // shmat to join to shared memory
    
    for(int i=0; i<h; ++i)
    {
        sem_wait(write_sem);
        for(int j=0; j < w; ++j)
        {
            gray[i][j] = *(pixel + i*w + j);
        }

        if(i >= 2 && i < h)
        {
            processRow(i - 2);
        }
    }

    // Process remaining rows
    processRow(h - 2);
    processRow(h - 1);

    // Destroy the shared memory
    shmdt(pixel);
    shmctl(shmid,IPC_RMID,NULL);

    //writeToFile
    ofstream fout(file_name2);

    fout << "P2\n";
    fout << w << " " << h << "\n";
    fout << 255 << "\n";

    for(int i=0; i < h; ++i){
        for(int j = 0; j < w; ++j){
            fout << edges[i][j] << "\n";
        }
    }

    fout.close();
}

int main(int argc, char** argv) 
{
    if(argc < 2)
    {
        printf("Incorrect input\n");
        return 0;
    }
    file_name1 = argv[1];
    file_name2 = argv[2];
    
    read_sem = sem_open(SNAME_READ, 0);
    write_sem = sem_open(SNAME_WRITE, 0);

    getDimensions();

    detectEdges();

    sem_destroy(write_sem);
    sem_destroy(read_sem);

    return 0;
}