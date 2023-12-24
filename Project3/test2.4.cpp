#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
/*multiple processes*/
using std::cout;
using std::endl;

int main()
{
    if(fork()==0){
        // successfully create child process
        /* Allocate swap-backed page from the arena */
        char *filename = static_cast<char *>(vm_map(nullptr, 0));
        vm_yield();

        /* Write the name of the file that will be mapped */
        strcpy(filename, "lampson83.txt");

        /* Map a page from the specified file */
        char *p = static_cast<char *>(vm_map(filename, 0));

        vm_yield();

        /* Print the first part of the paper */
        for (unsigned int i = 0; i < 3; i++)
        {
            cout << p[i];
        }
    }
    else{
        /* Allocate swap-backed page from the arena */
        char *filename = static_cast<char *>(vm_map(nullptr, 0));

        /* Write the name of the file that will be mapped */
        strcpy(filename, "data1.bin");
        vm_yield();

        /* Map a page from the specified file */
        char *p = static_cast<char *>(vm_map(filename, 0));
        /* Print the first part of the paper */
        for (unsigned int i = 0; i < 3; i++)
        {
            cout << p[i];
        }
    }
    cout<<"end of the process"<<endl;
}