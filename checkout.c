#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_PATH_LEN 256
#define MAX_HASH_LEN 41
#define MAX_LINE_LEN 512

char* read_file_content(const char* path){
    FILE* f=fopen(path,"r");
    if(!f) return NULL;
    fseek(f,0,SEEK_END);
    long size=ftell(f);
    fseek(f,0,SEEK_SET);
    char* buffer=malloc(size+1);
    fread(buffer,1,size,f);
    buffer[size]='\0';
    fclose(f);
    return buffer;
}

char* get_tree_hash_from_commit(const char* commit_hash){
    char object_path[MAX_PATH_LEN];
    sprintf(object_path,".myvcs/objects/%s",commit_hash);
    char* commit_content=read_file_content(object_path);
    if(!commit_content) return NULL;
    char* tree_line=strstr(commit_content,"tree ");
    if(!tree_line){
        free(commit_content);
        return NULL;
    }
    char* tree_hash=malloc(MAX_HASH_LEN);
    sscanf(tree_line,"tree %40s",tree_hash);
    free(commit_content);
    return tree_hash;
}

char* get_blob_hash_from_tree(const char* tree_hash, const char* filename){
    char object_path[MAX_PATH_LEN];
    sprintf(object_path,".myvcs/objects/%s",tree_hash);
    char* tree_content=read_file_content(object_path);
    if(!tree_content) return NULL;
    char* line=strtok(tree_content,"\n");
    while(line!=NULL){
        char current_file[MAX_PATH_LEN];
        char current_hash[MAX_HASH_LEN];
        sscanf(line,"%s %s",current_file,current_hash);
        if(strcmp(current_file,filename)==0){
            free(tree_content);
            return strdup(current_hash);
        }
        line=strtok(NULL,"\n");
    }
    free(tree_content);
    return NULL;
}

int main(int argc, char *argv[]){
    if(argc<3){
        fprintf(stderr,"Usage: %s <commit_hash> <filename>\n",argv[0]);
        return 1;
    }
    const char* commit_hash=argv[1];
    const char* filename=argv[2];
    char* tree_hash=get_tree_hash_from_commit(commit_hash);
    if(!tree_hash){
        fprintf(stderr,"Error: COuld not find tree for commit %s\n", commit_hash);
        return 1;
    }
    char* blob_hash=get_blob_hash_from_tree(tree_hash,filename);
    if(!blob_hash){
        fprintf(stderr, "Error: File '%s' not found in commit %s\n",filename,commit_hash);
        free(tree_hash);
        return 1;
    }
    char blob_path[MAX_PATH_LEN];
    sprintf(blob_path,".myvcs/objects/%s",blob_hash);
    char* blob_content=read_file_content(blob_path);
    if(!blob_content){
        fprintf(stderr,"Error: COuld not read blobl object %s\n",blob_hash);
        free(tree_hash);
        free(blob_hash);
        return 1;
    }
    FILE* target_file=fopen(filename,"w");
    if(!target_file){
        perror("Error: Could not open target file for writing");
        free(tree_hash);
        free(blob_hash);
        free(blob_content);
        return 1;
    }
    fwrite(blob_content,1,strlen(blob_content),target_file);
    fclose(target_file);
    printf("Restored '%s' to version from commit %s\n",filename,commit_hash);
    free(tree_hash);
    free(blob_hash);
    free(blob_content);
    return 0;
}