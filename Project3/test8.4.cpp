#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
/*test fork with read write*/
using std::cout;
using std::endl;

int main()
{
    if (fork() == 0)
    {
        /* Allocate swap-backed page from the arena */
        char *filename = static_cast<char *>(vm_map(nullptr, 0));

        /* Write the name of the file that will be mapped */
        strcpy(filename, "lampson83.txt");

        vm_yield();

        /* Map a page from the specified file */
        char *p = static_cast<char *>(vm_map(filename, 0));

        p[4] = 'x';
        p[1] = 'y';
        p[2] = 'z';

        /* Print the first part of the paper */
        for (unsigned int i = 0; i < 19; i++)
        {
            cout << p[i] << endl;
        }
    }
    else
    {
        /* Allocate swap-backed page from the arena */
        char *filename = static_cast<char *>(vm_map(nullptr, 0));

        /* Write the name of the file that will be mapped */
        strcpy(filename, "lampson83.txt");

        /* Map a page from the specified file */
        char *p = static_cast<char *>(vm_map(filename, 0));

        p[0] = 'a';
        p[1] = 'b';
        p[2] = 'c';

        /* Print the first part of the paper */
        for (unsigned int i = 0; i < 19; i++)
        {
            cout << p[i]<<endl;
        }

        vm_yield();

        /* Write the name of the file that will be mapped */
        strcpy(filename, "data1.bin");

        /* Map a page from the specified file */
        char *p1 = static_cast<char *>(vm_map(filename, 0));

        /* Print the first part of the paper */
        for (unsigned int i = 0; i < 19; i++)
        {
            cout << p1[i] << endl;
        }

        vm_yield();

        /* Write the name of the file that will be mapped */
        strcpy(filename, "lampson83.txt");

        /* Map a page from the specified file */
        char *p2 = static_cast<char *>(vm_map(filename, 0));

        /* Print the first part of the paper */
        for (unsigned int i = 0; i < 19; i++)
        {
            cout << p2[i] << endl;
        }
    }
}