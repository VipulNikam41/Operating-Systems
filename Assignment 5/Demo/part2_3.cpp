#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <thread>
#include <semaphore.h>
#include <atomic>
#include <mutex>
#include <unistd.h> 

#define vvi vector<vector<int>>
#define vi vector<int>
using namespace std;

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
int r1;
int v1, v2, v3;
int ** g;
int val;
int ** b;
int ** edges;
int new_h, new_w;


void GRAY_WRITE() 
{
    int i=0;
    int p[2], pid, nbytes;
    if(pipe(p) < 0)
    {
        cout << "Failed to create pipe\n";
        exit(0);
    }
    // Writer
    if((pid=fork()) > 0)
    {
        char *pixel = new char[w + 1];
        for(int i=0; i < h; ++i) 
        {
            for(int j = 0; j < w; ++j)
            {
                string row = "";
                row += (char) (( (int) (0.3*r[i][j] + 0.59*g[i][j] + 0.11*b[i][j])) + 128);
                strcpy(pixel + j, row.c_str());
            }
            write(p[1], pixel, w *sizeof(char) + 1 );
        }
    }

    else 
    {
        int row = 0;
        char inbuf[w * sizeof(char) + 1]; 
        while ((nbytes = read(p[0], inbuf, w * sizeof(char) + 1)) > 0)
        {
            for(int j=0; j < w; ++j)
            {
                gray[row][j] = ( (int) (*(inbuf + j)) ) + 128;
            }
            if(row >= 2 && row < h)
            {
                r1 = row - 2;
                for(int j=0; j < w; ++j) 
                {
                    int pixel = 0;
                    for(int x = 0; x<3; ++x)
                    {
                        for(int y = 0; y<3; ++y)
                        {
                            if(r1 + x >= h || j + y >= w)
                                continue;
                            pixel += gray[r1+x][j+y] * filter[x][y];
                        }
                    }

                    int al = min(pixel, 255);
                    edges[r1][j] = max(al, 0);

                }
                row++;
                if(row >= h)
                    break;
            }

        // Process remaining rows
        }

        // writeToFile;
        ofstream fout(file_name2);
        fout << "P2\n";
        fout << w << " " << h << "\n";
        fout << 255 << "\n";

        for(int i=0; i < h; ++i)
        {
            for(int j = 0; j < w; ++j)
            {
                fout << edges[i][j] << endl;
            }
        }
    }


    ofstream fout(file_name2);
    fout << "P2\n";
    fout << w << " " << h << "\n";
    fout << 255 << "\n";

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
                        pixel += gray[i+x][j+y] * filter[x][y];
                }
            }
            int al = min(pixel, 255);
            edges[i][j] = max(al, 0);

            // // writeToFile;
            // fout << edges[i][j] << "\n";
        }
    }

    for(int i=0; i < h; ++i)
    {
        for(int j = 0; j < w; ++j)
        {
            fout << edges[i][j] << endl;
        }
    }
    fout.close();
}

void GRAY_WRlTE() 
{
    ofstream fout(file_name2);
    fout << "P2\n";
    fout << w << " " << h << "\n";
    fout << 255 << "\n";

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
                        pixel += gray[i+x][j+y] * filter[x][y];
                }
            }
            int al = min(pixel, 255);
            edges[i][j] = max(al, 0);

            // // writeToFile;
            // fout << edges[i][j] << "\n";
        }
    }

    for(int i=0; i < h; ++i)
    {
        for(int j = 0; j < w; ++j)
        {
            fout << edges[i][j] << endl;
        }
    }
    fout.close();
}

void readImage(string file_name1,string file_name2) 
{
    ifstream fin(file_name1);
    string first_line;

    fin >> first_line;
    fin >> w >> h >> val;

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
    
    GRAY_WRlTE();

     // Get file pointer 
}

int main(int argc, char** argv) 
{
    if(argc < 3)
    {
        printf("Incorrect input\n");
        return 0;
    }

    file_name2 = argv[2];
    readImage(argv[1], file_name2);
    
    return 0;
}