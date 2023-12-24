#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
/*test fork with intensive read write*/
using std::cout;
using std::endl;

int main()
{
    if (fork() == 0)
    {
        // map two swap-backed pages
        auto *page0 = (char *)vm_map(nullptr, 0);
        auto *page1 = (char *)vm_map(nullptr, 0);

        // write the filename into virtual memory
        auto *filename = page0 + VM_PAGESIZE;
        strcpy(filename, "data1.bin");

        // map a file-backed page
        auto *page2 = (char *)vm_map(page1, 0);

        filename[4]='2';
        vm_yield();

        // map a file-backed page
        auto *page3 = (char *)vm_map(page1, 0);

        /* Print the first part of the paper */
        for (unsigned int i = 0; i < 39; i++)
        {
            cout << page3[i] << endl;
            vm_yield();
            cout << page2[i] << endl;
            page3[i]=page2[i];
            vm_yield();
        }
    }
    else
    {
        // map two swap-backed pages
        auto *page0 = (char *)vm_map(nullptr, 0);
        auto *page1 = (char *)vm_map(nullptr, 0);

        // write the filename into virtual memory
        auto *filename = page0 + VM_PAGESIZE;
        strcpy(filename, "data2.bin");

        // map a file-backed page
        auto *page2 = (char *)vm_map(filename, 0);

        filename[4] = '1';
        vm_yield();

        // map a file-backed page
        auto *page3 = (char *)vm_map(filename, 0);

        /* Print the first part of the paper */
        for (unsigned int i = 0; i < 39; i++)
        {
            cout << page3[i] << endl;
            vm_yield();
            cout << page2[i] << endl;
            page3[i] = page2[i];
            vm_yield();
        }
    }
}