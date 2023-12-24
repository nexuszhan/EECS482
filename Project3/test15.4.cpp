// test sharing between file backed pages (SZ)

#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;

int main()
{
    if (fork() == 0)
    {
        if (fork() == 0)
        {
            vm_yield();
            char* filename = static_cast<char *>(vm_map(nullptr, 0));

            /* Write the name of the file that will be mapped */
            strcpy(filename, "data1.bin");

            /* Map a page from the specified file */
            char* p = static_cast<char *>(vm_map (filename, 0));
            for (unsigned int i=0; i<VM_PAGESIZE; i++)
            {
                cout << p[i];
            }
        }
        else
        {
            /* Allocate swap-backed page from the arena */
            char* filename = static_cast<char *>(vm_map(nullptr, 0));

            /* Write the name of the file that will be mapped */
            strcpy(filename, "data1.bin");

            /* Map a page from the specified file */
            char* p = static_cast<char *>(vm_map (filename, 0));

            /* Print the first part of the paper */
            for (unsigned int i=0; i<1930; i++)
            {
                cout << p[i];
            }

            // /* Print the first part of the paper */
            // for (unsigned int i=0; i<1930; i++)
            // {
            //     cout << p[i];
            // }
        }
        
    }
    else
    {
        char* filename = static_cast<char *>(vm_map(nullptr, 0));

        /* Write the name of the file that will be mapped */
        strcpy(filename, "data1.bin");

        /* Map a page from the specified file */
        char* p = static_cast<char *>(vm_map (filename, 0));

        /* Print the first part of the paper */
        for (unsigned int i=0; i<1930; i++) 
        {
            cout << p[i];
        }

        vm_yield();

        char* p1 = static_cast<char*>(vm_map(filename, 1));
        /* Change the first part of the paper */
        for (unsigned int i=0; i<VM_PAGESIZE-1; i++) 
        {
            p[i] = p[i+1];
        }
        p[VM_PAGESIZE-1] = p1[0];
    }
}