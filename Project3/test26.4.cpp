#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
/*page write based on physical memory addr
    non empty arena*/
using std::cout;
using std::endl;

int main()
{ // map two swap-backed pages
    auto *page00 = (char *)vm_map(nullptr, 0);
    auto *page01 = (char *)vm_map(nullptr, 0);
    // write the filename into virtual memory
    auto *filename0 = page00 + VM_PAGESIZE;
    strcpy(filename0, "data1.bin");

    // map a file-backed page
    auto *page02 = (char *)vm_map(filename0, 0);
    filename0[4] = '2';
    // map a file-backed page
    auto *page03 = (char *)vm_map(filename0, 1);

    fork();
    /* Print the first part of the paper */
    for (unsigned int i = 18; i < 29; i++)
    {
        cout << page03[i] << endl;
        cout << page02[i] << endl;
        page03[i] = page02[i];
        vm_yield();
        page02[i] = page03[i + 10];
    }

    if (fork() == 0)
    {
        // map two swap-backed pages
        auto *page0 = (char *)vm_map(nullptr, 0);
        auto *page1 = (char *)vm_map(nullptr, 0);

        // write the filename into virtual memory
        auto *filename = page0 + VM_PAGESIZE;
        strcpy(filename, "data1.bin");

        // map a file-backed page
        auto *page2 = (char *)vm_map(filename, 0);

        fork();

        filename[4] = '2';
        vm_yield();

        // map a file-backed page
        auto *page3 = (char *)vm_map(filename, 0);

        /* Print the first part of the paper */
        for (unsigned int i = 0; i < 19; i++)
        {
            cout << page3[i] << endl;
            vm_yield();
            cout << page2[i] << endl;
            page3[i] = page2[i];
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
        fork();

        filename[4] = '1';
        vm_yield();

        // map a file-backed page
        auto *page3 = (char *)vm_map(filename, 0);

        /* Print the first part of the paper */
        for (unsigned int i = 0; i < 19; i++)
        {
            cout << page3[i] << endl;
            vm_yield();
            cout << page2[i] << endl;
            page3[i] = page2[i];
            vm_yield();
        }
    }

    if (fork() == 0)
    {
        // map two swap-backed pages
        auto *page0 = (char *)vm_map(nullptr, 0);
        auto *page1 = (char *)vm_map(nullptr, 0);

        // write the filename into virtual memory
        auto *filename = page0 + VM_PAGESIZE;
        strcpy(filename, "data1.bin");

        // map a file-backed page
        auto *page2 = (char *)vm_map(filename, 0);

        filename[4] = '2';
        vm_yield();

        // map a file-backed page
        auto *page3 = (char *)vm_map(filename, 0);

        /* Print the first part of the paper */
        for (unsigned int i = 0; i < 19; i++)
        {
            cout << page3[i] << endl;
            vm_yield();
            cout << page2[i] << endl;
            page3[i] = page2[i+2];
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
        for (unsigned int i = 0; i < 19; i++)
        {
            cout << page3[i] << endl;
            vm_yield();
            cout << page2[i] << endl;
            page3[i] = page2[i+2];
            vm_yield();
        }
    }
}