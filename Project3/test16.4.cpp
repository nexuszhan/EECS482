// test sharing between file backed pages with nested fork and non-empty parent arena (SZ)

#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;

int main()
{
    char* filename = static_cast<char *>(vm_map(nullptr, 0));

    /* Write the name of the file that will be mapped */
    strcpy(filename, "data1.bin");

    /* Map a page from the specified file */
    char* p_parent = static_cast<char *>(vm_map (filename, 0));
    // Put the file content into physmem
    for (unsigned int i=0; i<1930; i++)
    {
        cout << p_parent[i];
    }

    if (fork() == 0)
    {
        /* Map a page from the specified file */
        char* p_child = static_cast<char *>(vm_map (filename, 1));

        /* Print the second part of the paper */
        for (unsigned int i=0; i<1930; i++)
        {
            cout << p_child[i];
        }

        vm_yield();
        
        if (fork() == 0)
        {
            // Should print modified content
            for (unsigned int i=0; i<1930; i++)
            {
                cout << p_parent[i];
            }
        }
        else
        {
            vm_yield();

            // Put something to physmem to evict
            char* filename1 = static_cast<char *>(vm_map(nullptr, 0));
            strcpy(filename1, "data3.bin");

            char* filename2 = static_cast<char *>(vm_map(nullptr, 0));

            strcpy(filename2, "data2.bin");
        }
    }
    else
    {
        vm_yield();

        char* p1 = static_cast<char*>(vm_map(filename, 1));
        /* Change the first part of the paper */
        for (unsigned int i=0; i<VM_PAGESIZE-1; i++) 
        {
            p_parent[i] = p_parent[i+1];
        }
        p_parent[VM_PAGESIZE-1] = p1[0];
    }
}