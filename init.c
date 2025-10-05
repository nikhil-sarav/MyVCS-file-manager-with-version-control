#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int main()
{
    const char* vcs_dir = ".myvcs";
    const char* objects_dir=".myvcs/objects";
    const char* refs_dir=".myvcs/refs";
    const char* refs_Dir=".myvcs/refs";
    const char* head_file_path=".myvcs/HEAD";

    if (mkdir(vcs_dir, 0755)!=0){
        perror("Failed to create .myvcs directory. It may already exist");
        return 1;
    }
    if (mkdir(objects_dir,0755)!=0){
        perror("Failed to create objects directory");
        return 1;
    }
    if (mkdir(refs_dir,0755)!=0){
        perror("Failed to create refs directory");
        return 1;
    }
    FILE *head_file=fopen(head_file_path,"w");
    if(head_file==NULL){
        perror("Failed to create HEAD file");
        return 1;
    }
    fprintf(head_file,"ref/main\n");
    fclose(head_file);
    printf("Initialized empty MyVCS repository in ./.myvcs/\n");
    return 0;
}