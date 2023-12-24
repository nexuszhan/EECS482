#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
/*test page evict*/
using std::cout;
using std::endl;

int main()
{
    /* Allocate swap-backed page from the arena */
    char *filename = static_cast<char *>(vm_map(nullptr, 0));

    /* Write the name of the file that will be mapped */
    strcpy(filename, "data1.bin");

    /* Map a page from the specified file */
    char *p1 = static_cast<char *>(vm_map(filename, 0));
    char *p2 = static_cast<char *>(vm_map(filename, 0));
    char *p3 = static_cast<char *>(vm_map(filename, 0));
    char *p15 = static_cast<char *>(vm_map(filename, 1));
    char *p16 = static_cast<char *>(vm_map(filename, 2));
    char *p17 = static_cast<char *>(vm_map(filename, 3));
    p1[0]='a';
    p2[0]='b';
    p15[0] = 'c';
    p16[0] = 'd';
    p17[1] = 'd';

    /* Print the first part of the paper */
    for (unsigned int i = 0; i < 10; i++)
    {
        cout << p1[i] << p2[i] << p3[i] << p15[i] << p16[i] << p17[i]<<endl;
    }
}