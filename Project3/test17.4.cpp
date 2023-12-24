// test eager reservation for non-empty arena (SZ)

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <vector>
#include "vm_app.h"

using std::cout;
using std::vector;

int main()
{
    vector<char*> filenames;

    for (unsigned int i=0; i<127; i++)
    {
        filenames.push_back(static_cast<char *>(vm_map(nullptr, 0)));
        strcpy(filenames[i], "data1.bin");
    }
        
    if (fork() == 0)
    {
        for (unsigned int i=126; i>=120; i--)
            cout << filenames[i] << "\n";

        for (unsigned int i=1; i<127; i*=2)
        {
            strcpy(filenames[i], "data2.bin");
        }

        if (fork() == 0)
        {
            for (unsigned int i=1; i<127; i*=2)
            {
                cout << filenames[i] << "\n";
            }
            cout << "process 3 end\n";
        }
        else
        {
            vm_yield();
            cout << "process 2 end\n";
        }
    }
    else
    {
        vm_yield();

        cout << "process 1 end\n";
    }
}