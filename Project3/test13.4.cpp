/*  test for swap-backed pages 
    eager swap reservation
*/
#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;
int glb = 0;
int main()
{
    fork();
    char* evict = static_cast<char *>(vm_map(nullptr, 0));
    evict[0] = 'a';
    if (fork()){
        char* filename = static_cast<char *>(vm_map(nullptr, 0));
        strcpy(filename, "lampson83.txt");
        cout << glb << "\n";
        glb++;
        for (unsigned int i=0; i<5; i++) {
            cout << filename[i];
            filename[i] = 'a';
        }
        for (unsigned int i=2; i<7; i++) {
            cout << filename[i];
            filename[i] = 'b';
        }
        cout<< "process 1\n";
    } else {
        while(auto new_swap_page = vm_map(nullptr, 0)){
        char* filename = static_cast<char *>(new_swap_page);
        strcpy(filename, "lampson83.txt");
        cout << glb << "\n";
        glb++;
        for (unsigned int i=0; i<5; i++) {
            cout << filename[i];
            filename[i] = 'a';
        }
        for (unsigned int i=2; i<7; i++) {
            cout << filename[i];
            filename[i] = 'b';
        }
    }
    }
    

    
    cout << glb << "\n";
    vm_yield();



    
}