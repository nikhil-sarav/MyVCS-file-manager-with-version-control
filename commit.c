#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

#define MAX_PATH_LEN 256
#define MAX_LINE_LEN 512

char* sha1_hash_string(const char* content){
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((const unsigned char*)content, strlen(content),hash);
    char *hash_str=malloc(SHA_DIGEST_LENGTH*2+1);
    for(int i=0;i<SHA_DIGEST_LENGTH;i++){
        sprintf(hash_str+(i*2),"%02x",hash[i]);
    }
    hash_str[SHA_DIGEST_LENGTH*2]='\0';
    return hash_str;
}

void write_object(const char* hash, const char* content){
    char object_path[MAX_PATH_LEN];
    sprintf(object_path,".myvcs/objects/%s",hash);
    FILE *f = fopen(object_path,"w");
    if(!f){
        perror("Failed to write object file");
        exit(1);
    }
    fwrite(content,1,strlen(content),f);
    fclose(f);
}

char* read_file_content(const char* path){
    FILE* f=fopen(path,"r");
    if (!f) return NULL;
    fseek(f,0,SEEK_END);
    long size=ftell(f);
    fseek(f,0,SEEK_SET);
    char* buffer=malloc(size+1);
    fread(buffer,1,size,f);
    buffer[size]='\0';
    fclose(f);
    if(size>0&&buffer[size-1]=='\n'){
        buffer[size-1]='\0';
    }
    return buffer;
}

int main(int argc,char *argv[]){
    if (argc<3||strcmp(argv[1],"-m") !=0)
    {
        fprintf(stderr,"Usage: %s -m\"<commit message>\"\n", argv[0]);
        return 1;
    }
    const char* commit_message=argv[2];
    char* index_content=read_file_content(".myvcs/index");
    if(!index_content||strlen(index_content)==0){
        fprintf(stderr,"Nothing to commit, staging area is empty.\n");
        return 1;
    }
    char* tree_hash=sha1_hash_string(index_content);
    write_object(tree_hash,index_content);
    free(index_content);
    char* parent_hash=read_file_content(".myvcs/refs/main");
    char commit_content[1024];
    long timestamp=time(NULL);
    const char* author="Your Name <your.email@example.com>";
    if(parent_hash){
        sprintf(commit_content,"tree %s\nparent %s\nauthor %s %ld\n\n%s\n", tree_hash,parent_hash,author,timestamp,commit_message);
        free(parent_hash);
    }
    else{
        sprintf(commit_content,"tree %s\nauthor %s %ld\n\n%s\n", tree_hash,author,timestamp,commit_message);
    }
    char* commit_hash=sha1_hash_string(commit_content);
    write_object(commit_hash,commit_content);
    printf("Created commit with hash: %s\n",commit_hash);
    FILE* branch_ref=fopen(".myvcs/refs/main","w");
    if(!branch_ref){
        perror("Failed to update branch reference");
        exit(1);
    }
    fprintf(branch_ref,"%s\n",commit_hash);
    fclose(branch_ref);
    fclose(fopen(".myvcs/index","w"));
    printf("Committed to branch main\n");
    free(tree_hash);
    free(commit_hash);
    return 0;
}