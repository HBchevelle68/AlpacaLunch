#include <interfaces/utility_interface.h>

off_t AlpacaFileUtils_fSize(const char *filename) {
	
    struct stat st; 

    if (stat(filename, &st) == 0){
        return st.st_size;
    }

    return -1; 
}