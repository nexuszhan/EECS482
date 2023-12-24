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

    
    cout << glb << "\n";
    vm_yield();



    
}