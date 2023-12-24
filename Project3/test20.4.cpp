#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
/*test swiap page evict*/
using std::cout;
using std::endl;

int main()
{
    /* Allocate swap-backed page from the arena */
    char *filename1 = static_cast<char *>(vm_map(nullptr, 0));
    /* Write the name of the file that will be mapped */
    strcpy(filename1, "lampson83.txt");

    /* Allocate swap-backed page from the arena */
    char *filename2 = static_cast<char *>(vm_map(nullptr, 0));
    /* Write the name of the file that will be mapped */
    strcpy(filename2, "lampson84.txt");

    /* Allocate swap-backed page from the arena */
    char *filename3 = static_cast<char *>(vm_map(nullptr, 0));
    /* Write the name of the file that will be mapped */
    strcpy(filename3, "lampson85.txt");

    /* Allocate swap-backed page from the arena */
    char *filename4 = static_cast<char *>(vm_map(nullptr, 0));
    /* Write the name of the file that will be mapped */
    strcpy(filename4, "lampson86.txt");

    /* Allocate swap-backed page from the arena */
    char *filename5 = static_cast<char *>(vm_map(nullptr, 0));
    /* Write the name of the file that will be mapped */
    strcpy(filename5, "lampson87.txt");

    /* Allocate swap-backed page from the arena */
    char *filename6 = static_cast<char *>(vm_map(nullptr, 0));
    /* Write the name of the file that will be mapped */
    strcpy(filename6, "lampson87.txt");

    auto *filename = filename1 + VM_PAGESIZE - 3;
    strcpy(filename, "hhhhhhh");

    /* Print the first part of the paper */
    for (unsigned int i = 0; i < 10; i++)
    {
        cout << filename2[i];
    }
    cout << endl;
}