#define _GNU_SOURCE 500
#include <ftw.h>
#include <features.h>
#include <string.h>
#include <stdio.h>

int retval = FTW_SKIP_SUBTREE;
int a = 0;
int l = 0;
int n = 0;

int fn (const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf) {
    
    if (!(!a && fpath[ftwbuf->base] == '.')) 
        printf("%s \n", fpath);
    return ftwbuf->level == 0 ? 0 : retval; 
}

int main(int argc, char** argv) {
    char* path = ".";

    //predef flags
    char* flag_l = "-l";
    char* flag_a = "-a";
    char* flag_n = "-n";
    char* flag_r = "-R";
    
    //parse args
    //printf("%d", argc);
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], flag_l) == 0) {

        }
        else if (strcmp(argv[i], flag_a) == 0) {
            a = 1;
        } 
        else if (strcmp(argv[i], flag_n) == 0) {

        }
        else if (strcmp(argv[i], flag_r) == 0) {
            retval = 0;
        }
        else if (i == argc -1) {
           // printf("path = %s", path);
            path = argv[i];
        }
        else {
            printf("Wrong arg: %s", argv[i]);
            return 1;
        }
    }

    return nftw(path, fn, 5, FTW_ACTIONRETVAL);
}
