#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <thread>
#include <semaphore.h>

#define vvi vector<vector<int>>
#define vi vector<int>
using namespace std;

sem_t semaphore;
// atomic<int> turn{0};
int pixels = 0;
int MAX = 4096;
int filter[3][3] = { {1, 2, 1}, {2, -13, 2}, {1, 2, 1}};
string file_name1;
string file_name2;
// mutex mtx;
// file_name = argv[2];

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
    int val;

    fin >> first_line >> w >> h >> val;

    gray = new int*[h];
    r = g = b = gray;
    edges = gray;

    for(int i=0; i<h; ++i)
    {
        gray[i] = new int[w];
        b[i] = g[i] = r[i] = edges[i] = gray[i];
        for(int j=0; j < w; ++j) 
        {
            fin >> r[i][j]; 
            fin >> g[i][j];
            fin >> b[i][j];
        }
    }
    fin.close();

}

void gray_write() 
{
    ofstream fout(file_name2);
    fout << "P2\n";
    fout << w << " " << h << "\n";
    fout << 255 << "\n";

    for(int i=0; i < h; ++i) 
    {
        int v1, v2, v3;
        for(int j = 0; j < w; ++j)
        {
            v1 = 0.3*r[i][j];
            v2 = 0.59*g[i][j];
            v3 = 0.11*b[i][j];
            gray[i][j] = v1 + v2 + v3;
        }
        if(i > 2 && i < h)
            sem_post(&semaphore);
    }

    for(int i = 0; i < h; ++i)
    {
        if(i < h - 4)
            sem_wait(&semaphore);
        for(int j = 0; j < w; ++j)
        {
            int pixel = 0;
            for(int x = 0; x<3; ++x)
            {
                for(int y = 0; y<3; ++y)
                {
                    if(i + x >= h || j + y >= w)
                        continue;
                    else
                        pixel += gray[i+x][j+y] * filter[x][y];
                }
            }

            int al = min(pixel, 255);
            edges[i][j] = max(al, 0);

            fout << edges[i][j] << "\n";
        }
    }
    fout.close();
}

int main(int argc, char** argv) 
{
    if(argc < 3)
    {
        printf("Incorrect input\n");
        return 0;
    }

    file_name2 = argv[1];
    file_name2 = argv[2];

    readImage(argv[1]);
    
    sem_init(&semaphore, 0, 0);

    gray_write();

    sem_destroy(&semaphore);
    return 0;
}