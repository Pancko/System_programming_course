// Andreev S.V. 37 group
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>

void ShowDIR(const char* DIRPATH)
{
    struct dirent* catalogue;
    struct stat buffer;
    char file_path[1024];
    DIR* directory = opendir(DIRPATH);
    
    if (directory == NULL)
    {
        perror("opendir");
        exit(-1);
    }

    while ((catalogue = readdir(directory)) != NULL)
    {
        if (!strcmp(catalogue->d_name, ".") || !strcmp(catalogue->d_name, ".."))
            continue;
        strcpy(file_path, DIRPATH);
        strcat(file_path, "/");
        strcat(file_path, catalogue->d_name);
        printf("%s\n", file_path);

        if (stat(file_path, &buffer) == -1)
        {
            perror("stat");
        }

        if ((buffer.st_mode & __S_IFMT) == __S_IFDIR && catalogue->d_name[0] != '.')
        {
            ShowDIR(file_path);
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Need a directory name\n");
        exit(-1);
    }

    ShowDIR(argv[1]);
    
    return 0;
}