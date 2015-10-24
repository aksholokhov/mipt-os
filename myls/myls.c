#define _BSD_SOURCE

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>

int a = 0;
int l = 0;
int n = 0;
int r = 0;

void parse_permissions(char* buf, mode_t mode) {
    buf[1] = mode & S_IRUSR ? 'r' : '-';
    buf[2] = mode & S_IWUSR ? 'w' : '-'; 
    buf[3] = mode & S_IXUSR ? 'x' : '-';
    buf[4] = mode & S_IRGRP ? 'r' : '-';
    buf[5] = mode & S_IWGRP ? 'w' : '-'; 
    buf[6] = mode & S_IXGRP ? 'x' : '-';
    buf[7] = mode & S_IROTH ? 'r' : '-';
    buf[8] = mode & S_IWOTH ? 'w' : '-';
    buf[9] = mode & S_IXOTH ? 'x' : '-';
    buf[10] = 0;
}

int walk_dir(char* path) {
    DIR* dir = opendir(path);
    if (dir == NULL) {
        perror("can't open path");
        return -1;
    }
    struct dirent *item = readdir(dir);
    int path_len = strlen(path);
    int ignored_prefix = 0;
    while (item != NULL) {
        if (ignored_prefix == 2) {
            if (item->d_type == DT_DIR) {
                strcat(path, "/");
                if (item->d_name[0] != '.' || a) {
                    listdir(strcat(path, item->d_name));
                }
                path[path_len] = 0;
            }
        } else {
            ignored_prefix++;
        }
        item = readdir(dir);
    }
    closedir(dir);
    return 0;
}

char parse_type(unsigned char type) {
    switch (type) {
        case DT_BLK: return 'b';
        case DT_CHR: return 'c';
        case DT_DIR: return 'd';
        case DT_FIFO: return 'p';
        case DT_LNK: return 'l';
        case DT_REG: return '-';
        case DT_SOCK: return 's';
        case DT_UNKNOWN: return 'u';
    }
    return '-';
}

void stat_item(char* path, char* name, unsigned char type) {
    struct stat* st = (struct stat*)malloc(sizeof(struct stat));
    if (stat(path, st) == -1) {
        printf("%s:\n", path);
        perror("can't fetch stat");
        return;    
    }
    char* time = (char*)malloc(200*sizeof(char));
    if (strftime(time, 200, "%d.%m.%Y %H:%M:%S" , localtime(&(st->st_mtime))) == -1) {
        perror("time:");   
    }
    char* permissions = (char*)malloc(12*sizeof(char));
    permissions[0] = parse_type(type);
    parse_permissions(permissions, st->st_mode);
    if (l) {
        printf("%s %d %s %s %s %s\n", permissions, (int)st->st_nlink, getpwuid(st->st_uid)->pw_name, getgrgid(st->st_gid)->gr_name, time, name);
    }
    else if (n) {
        printf("%s %d %d %d %s %s\n", permissions, (int)st->st_nlink, st->st_uid, st->st_gid, time, name);
    }
    free(time);
}   

int listdir(char* path) {
    DIR* dir = opendir(path);
    if (dir == NULL) {
        perror("can't open path");
        return -1;
    }
    struct dirent *item = readdir(dir);
    printf("\n %s: \n", path);
    while (item != NULL) {
        int path_len = strlen(path);
        strcat(path, "/");
        if (item->d_name[0] != '.' || a) {     
            strcat(path, item->d_name);
            (n | l) ? stat_item(path, item->d_name, item->d_type) : printf("%s ", item->d_name);
        }
        path[path_len] = 0;
        item = readdir(dir);
    }
    printf("\n");
    closedir(dir);
    if (r) {
        return walk_dir(path);
    }
    return 0;

}

int main(int argc, char** argv) {
    char path[1024] = ".";

    //predef flags
    char* flag_l = "-l";
    char* flag_a = "-a";
    char* flag_n = "-n";
    char* flag_r = "-R";
    
    //parse args
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], flag_l) == 0) {
            l = 1;
        }
        else if (strcmp(argv[i], flag_a) == 0) {
            a = 1;
        } 
        else if (strcmp(argv[i], flag_n) == 0) {
            n = 1;
        }
        else if (strcmp(argv[i], flag_r) == 0) {
            r = 1;
        }
        else if (i == argc -1) {
            strncpy(path, argv[i], 1024);
        }
        else {
            printf("Wrong arg: %s", argv[i]);
            return 1;
        }
    }

    return listdir(path);
}
