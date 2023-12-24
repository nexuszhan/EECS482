#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;
int glb = 0;
int main()
{
    /* Allocate swap-backed page from the arena */
    char* filename = static_cast<char *>(vm_map(nullptr, 0));
    strcpy(filename, "lampson83.txt");
    cout << glb << "\n";
    glb++;
    int id = fork();
    /* Map a page from the specified file */
    char* p = static_cast<char *>(vm_map (filename, 0));

    if (id==0){
        /* Print the first part of the paper */
        for (int i=0; i<30*glb; i++) {
        cout << p[i];
        p[i] = 'a';
        }
    } else {
        /* Print the second part of the paper */
        for (int i=0; i<30*glb; i++) {
            cout << p[i];
            p[i] = 'b';
        }
    }
    

    vm_yield();
}