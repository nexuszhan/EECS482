#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
/*test swap page evict and state machine (SZ) */
using std::cout;
using std::endl;

int main()
{
    /* Allocate swap-backed page from the arena */
    char *filename = static_cast<char *>(vm_map(nullptr, 0));

    /* Write the name of the file that will be mapped */
    strcpy(filename, "lampson83.txt");

    /* Map a page from the specified file */
    char *p1 = static_cast<char *>(vm_map(nullptr, 0));
    strcpy(p1, "data1.bin");
    strcpy(p1, "null.bin");
    char *p2 = static_cast<char *>(vm_map(nullptr, 0));
    strcpy(p2, "data2.bin");
    char *p3 = static_cast<char *>(vm_map(nullptr, 0));
    strcpy(p3, "data3.bin");
    char *p15 = static_cast<char *>(vm_map(nullptr, 1));
    strcpy(p15, "data4.bin");
    char *p16 = static_cast<char *>(vm_map(nullptr, 2));
    strcpy(p16, "data1.bin");
    char *p17 = static_cast<char *>(vm_map(nullptr, 3));
    strcpy(p17, "data1.bin");

    cout << filename[0] << "\n";
    strcpy(filename, "lampson.txt");

    cout << p1[0] << "\n";

    char *p4 = static_cast<char *>(vm_map(nullptr, 0));
    strcpy(p4, "data1.bin");
    strcpy(p4, "null.bin");
    char *p5 = static_cast<char *>(vm_map(nullptr, 0));
    strcpy(p5, "data2.bin");
    cout << p1[1] << "\n";
    strcpy(p2, "null");
    strcpy(p5, "nothing");
    // p1[0]='a';
    // p2[0]='b';
    // p15[0] = 'c';
    // p16[0] = 'd';
    // p17[1] = 'd';

    // /* Print the first part of the paper */
    // for (unsigned int i = 0; i < 10; i++)
    // {
    //     cout << p1[i] << p2[i] << p3[i] << p15[i] << p16[i] << p17[i]<<endl;
    // }
}