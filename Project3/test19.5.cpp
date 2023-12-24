#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
/* test file-backed page state machine (SZ) */
using std::cout;
using std::endl;

int main()
{
    /* Allocate swap-backed page from the arena */
    char* filename1 = static_cast<char *>(vm_map(nullptr, 0));

    /* Write the name of the file that will be mapped */
    strcpy(filename1, "lampson83.txt");

    char* p1 = static_cast<char *>(vm_map(filename1, 0));
    cout << p1[0] << p1[1] << "\n";

    char* filename2 = static_cast<char*>(vm_map(nullptr, 1));
    strcpy(filename2, "data1.bin");

    char* p2 = static_cast<char*>(vm_map(filename2, 1));
    p2[0] = p2[1];

    char* filename3 = static_cast<char *>(vm_map(nullptr, 1));
    strcpy(filename3, "data4.bin");
    char *p16 = static_cast<char *>(vm_map(nullptr, 2));
    strcpy(p16, "data1.bin");

    cout << p2[0] << "\n";

    char *p17 = static_cast<char *>(vm_map(nullptr, 3));
    strcpy(p17, "data1.bin");

    char *p18 = static_cast<char *>(vm_map(nullptr, 3));
    strcpy(p18, "data1.bin");

    p1[0] = p1[2];
    cout << p2[1] << "\n";

    char *p19 = static_cast<char *>(vm_map(nullptr, 3));
    strcpy(p19, "data1.bin");

    cout << p2[3] << "\n";

    p1[1] = p1[4];

    strcpy(filename3, "lampson83.txt");

    char* p3 = static_cast<char*>(vm_map(filename3, 0));
    cout << p3[1] << "\n";
}