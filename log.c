#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_PATH_LEN 256
#define MAX_HASH_LEN 41

char* read_file_content(const char* path)
{
    FILE* f=fopen(path,"r");
    if (!f) return NULL;
    fseek(f,0,SEEK_END);
    long size=ftell(f);
    fseek(f,0,SEEK_SET);
    char* buffer=malloc(size+1);
    if(!buffer){
        perror("Failed to allocate memory for file content");
        exit(1);
    }
    fread(buffer,1,size,f);
    buffer[size]='\0';
    fclose(f);
    if(size>0 && buffer[size-1] == '\n')
    {
        buffer[size-1]='\0';
    }
    return buffer;
}

char* print_commit_and_get_parent(const char* commit_hash)
{
    char object_path[MAX_PATH_LEN];
    sprintf(object_path,".myvcs/objects/%s",commit_hash);
    char* commit_content=read_file_content(object_path);
    if(!commit_content){
        fprintf(stderr,"Error: COuld not read commit object %s\n",commit_hash);
        return NULL;
    }
    printf("commit %s\n",commit_hash);
    char* author_line = strstr(commit_content, "author");
    if(author_line){
        char* end_of_line=strchr(author_line,'\n');
        if(end_of_line) *end_of_line='\0';
        printf("%s\n",author_line);
        if(end_of_line) *end_of_line='\n';
    }
    char* message=strstr(commit_content,"\n\n");
    if(message){
        printf("\n  %s\n\n",message+2);
    }
    char* parent_line=strstr(commit_content,"parent ");
    char* parent_hash=NULL;
    if(parent_line){
        parent_hash=malloc(MAX_HASH_LEN);
        sscanf(parent_line,"parent %40s",parent_hash);
    }
    free(commit_content);
    return parent_hash;
}

int main()
{
    char* current_hash=read_file_content(".myvcs/refs/main");
    if(!current_hash){
        printf("No commits yet.");
        return 0;
    }
    while(current_hash!=NULL){
        char* parent_hash=print_commit_and_get_parent(current_hash);
        free(current_hash);
        current_hash=parent_hash;
    }
    return 0;
}