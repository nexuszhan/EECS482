#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
/*test swiap page evict*/
using std::cout;
using std::endl;

int main()
{
    if(fork()==0){
        /* Allocate swap-backed page from the arena */
        char *filename1 = static_cast<char *>(vm_map(nullptr, 0));
        /* Write the name of the file that will be mapped */
        strcpy(filename1, "lampson83.txt");

        /* Allocate swap-backed page from the arena */
        char *filename2 = static_cast<char *>(vm_map(nullptr, 0));
        /* Write the name of the file that will be mapped */
        strcpy(filename2, "lampson84.txt");

        auto *filename = filename1 + VM_PAGESIZE - 3;
        strcpy(filename, "hhhhhhh");

        cout << "nn" << endl;

        vm_yield();

        /* Print the first part of the paper */
        for (unsigned int i = 0; i < 13; i++)
        {
            cout << filename[i];
        }
        cout << endl;
    }
    else{
        /* Allocate swap-backed page from the arena */
        char *filename1 = static_cast<char *>(vm_map(nullptr, 0));
        /* Write the name of the file that will be mapped */
        strcpy(filename1, "lampson87.txt");

        /* Allocate swap-backed page from the arena */
        char *filename2 = static_cast<char *>(vm_map(nullptr, 0));
        /* Write the name of the file that will be mapped */
        strcpy(filename2, "lampson88.txt");

        auto *filename = filename1 + VM_PAGESIZE - 3;
        strcpy(filename, "bbbbbbb");

        cout <<"mm"<<endl;

        vm_yield();

        /* Print the first part of the paper */
        for (unsigned int i = 0; i < 13; i++)
        {
            cout << filename[i];
        }
        cout << endl;
    }
}