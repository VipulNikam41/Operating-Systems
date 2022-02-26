#include <iostream>
#include <thread>
#include <atomic>
#include <fstream>
#include <string.h>
#include <vector>
#include <mutex>

#define vvi vector<vector<int>>
#define vi vector<int>
using namespace std;

atomic<int> turn{0};
int pixels = 0;
int MAX = 4096;
string file_name1;
string file_name2;
int filter[3][3] = { {1, 2, 1}, {2, -13, 2}, {1, 2, 1}};
mutex mtx;

int h = 0, w = 0;

int ** gray;
int ** r;
int ** g;
int ** b;
int ** edges;
int val;

void gray_write() 
{
    ofstream fout(file_name2);
    fout << "P2\n";
    fout << w << " " << h << "\n";
    fout << 255 << "\n";

    int v1, v2, v3;
    for(int i=0; i < h; ++i) 
    {
        for(int j = 0; j < w; ++j)
        {
            v1 = 0.3*r[i][j];
            v2 = 0.59*g[i][j];
            v3 = 0.11*b[i][j];
            gray[i][j] = v1 + v2 + v3;
        
            int pixel = 0;
            for(int x = 0; x<3; ++x)
            {
                for(int y = 0; y<3; ++y)
                {
                    if(i + x >= h || j + y >= w)
                        continue;
                    else
                    {
                        pixel += gray[i+x][j+y] * filter[x][y];
                    }
                }
            }

            int al = min(pixel, 255);
            edges[i][j] = max(al, 0);

            fout << edges[i][j] << "\n";
        }
    }

    fout.close();
}

void readImage(string file_name1) 
{
    ifstream fin(file_name1);
    string first_line;

    fin >> first_line >> w >> h >> val;

    // Dynamically allocate all 2D Arrays
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

    gray_write();
}

int main(int argc, char** argv) 
{
    if(argc < 3)
    {
        printf("Incorrect input\n");
        return 0;
    }

    file_name1 = argv[1];
    file_name2 = argv[2];

    readImage(file_name1);

    return 0;
}