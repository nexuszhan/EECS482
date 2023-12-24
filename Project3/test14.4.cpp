#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;
int glb = 0;
int main()
{

    char* filename = static_cast<char *>(vm_map(nullptr, 0));
    for (unsigned int i=0; i<5; i++) {
        cout << filename[i];
        filename[i] = 'a';
        cout << filename[i];
    }
    strcpy(filename, "lampson83.txt");
    char* evict = static_cast<char *>(vm_map(nullptr, 0));
    evict[0] = 'a';
    
    cout << filename[0];
    filename[0] = 'b';

    evict = static_cast<char *>(vm_map(nullptr, 0));
    evict[0] = 'a';
    evict = static_cast<char *>(vm_map(nullptr, 0));
    evict[0] = 'a';
    evict = static_cast<char *>(vm_map(nullptr, 0));
    evict[0] = 'a';
    evict = static_cast<char *>(vm_map(nullptr, 0));
    evict[0] = 'a';

    cout << filename[0];
    
    evict = static_cast<char *>(vm_map(nullptr, 0));
    evict[0] = 'a';
    evict = static_cast<char *>(vm_map(nullptr, 0));
    evict[0] = 'a';
    evict = static_cast<char *>(vm_map(nullptr, 0));
    evict[0] = 'a';
    evict = static_cast<char *>(vm_map(nullptr, 0));
    evict[0] = 'a';

    cout << filename[0];
    evict = static_cast<char *>(vm_map(nullptr, 0));
    evict[0] = 'a';
    evict = static_cast<char *>(vm_map(nullptr, 0));
    evict[0] = 'a';
    evict = static_cast<char *>(vm_map(nullptr, 0));
    evict[0] = 'a';
    evict = static_cast<char *>(vm_map(nullptr, 0));
    evict[0] = 'a';

    filename[0] = 'b';


    cout << glb << "\n";
    glb++;
    char* filename_real = static_cast<char *>(vm_map(nullptr, 0));
    strcpy(filename_real, "lampson83.txt");
    char* p = static_cast<char *>(vm_map (filename_real, 0));
    evict = static_cast<char *>(vm_map(nullptr, 0));
    evict[0] = 'a';
    evict = static_cast<char *>(vm_map(nullptr, 0));
    evict[0] = 'a';
    evict = static_cast<char *>(vm_map(nullptr, 0));
    evict[0] = 'a';
    evict = static_cast<char *>(vm_map(nullptr, 0));
    evict[0] = 'a';
    while(auto new_swap_page = vm_map(nullptr, 0)){
        char* filenamee = static_cast<char *>(new_swap_page);
        strcpy(filenamee, "lampson83.txt");
        cout << glb << "\n";
        glb++;
        for (unsigned int i=0; i<5; i++) {
            cout << filenamee[i];
            filenamee[i] = 'a';
        }
        for (unsigned int i=2; i<7; i++) {
            cout << filenamee[i];
            filenamee[i] = 'b';
        }
    }

    
    if (fork()){

        for (int i=0; i<30; i++) {
            cout << p[i];
            p[i] = 'a';          
        }
        evict = static_cast<char *>(vm_map(nullptr, 0));
        evict[0] = 'a';
    } else {
        for ( int i=0; i<30; i++) {
            cout << p[i];
            p[i] = 'b';        
        }
    }
    

    vm_yield();
}