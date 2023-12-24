#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
/*test the capacity of swap_blocks*/
using std::cout;
using std::endl;

int main()
{
    /* Allocate swap-backed page from the arena */
    char *filename = static_cast<char *>(vm_map(nullptr, 0));

    for (unsigned int i = 0; i < 30; i++)
    {
        cout << filename[i];
    }
    cout<<endl;

    /* Write the name of the file that will be mapped */
    strcpy(filename, "lampson83.txt");

    /* Print the first part of the paper */
    for (unsigned int i = 0; i < 1930; i++)
    {
        cout << i << endl;
        /* Allocate swap-backed page from the arena */
        char *filename = static_cast<char *>(vm_map(nullptr, 0));

        if (filename == nullptr)
        {
            cout << "reach capacity" << endl;
            break;
        }

        /* Write the name of the file that will be mapped */
        strcpy(filename, "lampson83.txt");
    }
}