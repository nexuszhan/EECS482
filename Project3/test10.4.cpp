#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
/*add a lot of file backed page*/
using std::cout;
using std::endl;

int main()
{ // map two swap-backed pages
    auto *page00 = (char *)vm_map(nullptr, 0);
    // write the filename into virtual memory
    auto *filename0 = page00;
    strcpy(filename0, "data1.bin");
    // map a file-backed page
    auto *page01 = (char *)vm_map(filename0, 0);

    filename0[4] = '2';
    // map a file-backed page
    auto *page02 = (char *)vm_map(filename0, 0);

    filename0[4] = '3';
    // map a file-backed page
    auto *page03 = (char *)vm_map(filename0, 0);

    filename0[4] = '4';
    // map a file-backed page
    auto *page04 = (char *)vm_map(filename0, 0);

    fork();
    /* Print the first part of the paper */
    for (unsigned int i = 18; i < 29; i++)
    {
        page01[i + 5] = page02[i];
        page02[i] = page03[i + 3];
        page01[i + 10] = page03[i];
        page03[i] = page04[i + 7];
        page01[i + 15] = page04[i];
        page04[i] = page02[i + 17];
        vm_yield();
        for (unsigned int j = 1; j < 50; j++)
        {
            cout << page01[j];
        }
        cout << endl;
    }
}