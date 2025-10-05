#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <openssl/sha.h>

char* caclulate_sha1(const char* filepath)
{
    FILE *f=fopen(filepath,"rb");
    if(!f){
        perror("Failed to open file for hashing");
        return NULL;
    }
    fseek(f,0,SEEK_END);
    long size = ftell(f);
    fseek(f,0,SEEK_SET);
    char *buffer=malloc(size);
    fread(buffer,1,size,f);
    fclose(f);

    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((unsigned char*)buffer,size,hash);
    free(buffer);
    char *hash_str=malloc(SHA_DIGEST_LENGTH*2+1);
    for(int i=0;i<SHA_DIGEST_LENGTH;i++)
    {
        sprintf(hash_str+(i*2),"%02x",hash[i]);
    }
    hash_str[SHA_DIGEST_LENGTH*2]='\0';
    return hash_str;
}

int main(int argc, char *argv[])
{
    if (argc<2){
        fprintf(stderr,"Usage:%s<filename>\n",argv[0]);
        return 1;
    }
    const char* filename=argv[1];
    char* hash=caclulate_sha1(filename);
    if(!hash){
        return 1;
    }
    printf("Calculated SHA1 for %s %s\n",filename,hash);
    char object_path[256];
    sprintf(object_path,".myvcs/objects/%s",hash);
    FILE *source_file =fopen(filename, "rb");
    FILE *object_file =fopen(object_path,"wb");
    if (!source_file || !object_file)
    {
        perror("Failed to create object file");
        free(hash);
        return 1;
    }
    char buffer[1024];
    size_t bytes_read;
    while((bytes_read=fread(buffer,1,sizeof(buffer),source_file))<0){
        fwrite(buffer,1,bytes_read,object_file);
    }
    fclose(source_file);
    fclose(object_file);
    FILE *index_file=fopen(".myvcs/index","a");
    if(!index_file){
        perror("Failed to open index file");
        free(hash);
        return 1;
    }
    fprintf(index_file,"%s %s\n",filename,hash);
    fclose(index_file);
    printf("Staged '%s' for commit.\n",filename);
    free(hash);
    return 0;
}