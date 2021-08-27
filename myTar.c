#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<math.h>
void error(int e,char c){
    if(e==-1){
        if(c=='c'){
            printf("Failed to complete creation operation\n");
            exit(-1);
        }
        else if(c=='d'||c=='e'){
            printf("Failed to complete extraction operation\n");
            exit(-1);
        }
        else{
            printf("Failed to complete list operation\n");
            exit(-1);
        }
    }
}
int main(int argc,char* argv[])
{
    //check if atleast 3 arguments
 
    if(strcmp("-c",argv[1])==0){ //create tar
        if(argc!=4){ //error
            error(-1,'c');
        }
        int size=strlen(argv[2])+1+strlen(argv[3])+1;
        char *path=argv[2];
        char *actualpath=malloc(size);
        strcpy(actualpath,argv[2]);strcat(actualpath,"/");strcat(actualpath,argv[3]);
        int src_fd,dst_fd,n,err;
        struct dirent *pDirent;
        DIR *pDir;
        pDir=opendir(argv[2]);
        if(pDir==NULL){
            printf("Error Check\n");
            error(-1,'c');
        }
        
        dst_fd=open(actualpath,O_CREAT|O_WRONLY,0644);
        error(dst_fd,'c');
        int filecount=0;
        unsigned char buffer[4096];
        sprintf(buffer,"%d",0);
        err=write(dst_fd,buffer,1);
        error(err,'c');
        sprintf(buffer,"%s","\n");
        err=write(dst_fd,buffer,strlen("\n"));
        error(err,'c');
        while((pDirent=readdir(pDir))!=NULL){
            if(strcmp(pDirent->d_name,".")==0||strcmp(pDirent->d_name,"..")==0||strcmp(pDirent->d_name,argv[3])==0){
                continue;
            }
            else{
                filecount++;
                int sz=strlen(argv[2])+1+strlen(pDirent->d_name)+1;
                char *p=malloc(sz);
                strcpy(p,argv[2]);strcat(p,"/");strcat(p,pDirent->d_name);
                src_fd=open(p,O_RDONLY);
                error(src_fd,'c');
                int ending=lseek(src_fd,0,SEEK_END);
                int begin=lseek(src_fd,0,SEEK_SET);
                sprintf(buffer,"%d",ending-begin);
                err=write(dst_fd,buffer,1+(int)log10(ending-begin));
                error(err,'c');
                sprintf(buffer,"%s","\n");
                err=write(dst_fd,buffer,strlen("\n"));
                error(err,'c');
                sprintf(buffer,"%s",pDirent->d_name);
                err=write(dst_fd,buffer,strlen(pDirent->d_name));
                error(err,'c');
                sprintf(buffer,"%s","\n");
                err=write(dst_fd,buffer,strlen("\n"));
                error(err,'c');
                while(1){
                    err=read(src_fd,buffer,4096);
                    error(err,'c');
                    n=err;
                    if(n==0) break;
                    err=write(dst_fd,buffer,n);
                    error(err,'c');
                }
                close(src_fd);
            }
        }
        closedir(pDir);
        int pos=lseek(dst_fd,0,SEEK_SET);
        sprintf(buffer,"%d",filecount);
        int val=1+(int)log10(filecount);
        err=write(dst_fd,buffer,val);
        error(err,'c');
        close(dst_fd);
    }

    else if(strcmp("-d",argv[1])==0){ //extract tar
        if(argc!=3){ //error
            error(-1,'d');
        }
        struct dirent *pDirent;
        DIR *pDir;
        char *folderpath=malloc(strlen(argv[2]));
        int pos=-1;
        for(int i=strlen(argv[2])-1;i>=0;i--){
            if(argv[2][i]=='/'){
                pos=i-1;
                break;
            }
        }
        for(int i=0;i<=pos;i++){
            folderpath[i]=argv[2][i];
        }
        pDir=opendir(folderpath);
        if(pDir==NULL){
            error(-1,'d');
        }
        char *name=malloc(strlen(argv[2])+10);
        for(int i=pos+2;i<strlen(argv[2])-4;i++){
            name[i-pos-2]=argv[2][i];
        }
        char *store = (char*)malloc( (strlen(name)+4) * sizeof(char));
        strcpy(store,name);strcat(name,"Dump");
        
        int size=strlen(folderpath)+1+strlen(name)+1;
        char *loc=malloc(size);
        strcpy(loc,folderpath);strcat(loc,"/");strcat(loc,name);
        error(mkdir(loc,0777),'d'); 
        
        int src_fd,dst_fd,err,n;
        char *path=malloc(strlen(folderpath)+1+strlen(store)+5);
        strcpy(path,folderpath);strcat(path,"/");strcat(path,store);strcat(path,".tar");
        src_fd=open(path,O_RDONLY);
        char* filecount=malloc(6);
        char* buffer = (char*) malloc(1 * sizeof(char));
        err=read(src_fd,buffer,1);
        error(err,'d');
        strcpy(filecount,buffer);
        free(buffer);
        while(1){
            char* buffer = (char*) malloc(1 * sizeof(char));
            err=read(src_fd,buffer,1);
            error(err,'d');
            if(buffer[0]=='\n'){
                free(buffer);
                break;
            }
            strcat(filecount,buffer);
            free(buffer);
        }
        int fcount=atoi(filecount);
        while(1){
            char* filesize=(char*)malloc(15*sizeof(char));
            char* buffer = (char*) malloc(1 * sizeof(char));
            err=read(src_fd,buffer,1);
            error(err,'d');
            if(err==0) break;
            strcpy(filesize,buffer);
            free(buffer);
            while(1){
                char* buffer = (char*) malloc(1 * sizeof(char));
                err=read(src_fd,buffer,1);
                error(err,'d');
                if(buffer[0]=='\n'){
                    free(buffer);
                    break;
                }
                strcat(filesize,buffer);
                free(buffer);
            }
            int fsize=atoi(filesize);
            char* filename = (char*) malloc(20*sizeof(char));
            buffer = (char*) malloc(1 * sizeof(char));
            err=read(src_fd,buffer,1);
            error(err,'d');
            strcpy(filename,buffer);
            free(buffer);
            while(1){
                char* buffer = (char*) malloc(1 * sizeof(char));
                err=read(src_fd,buffer,1);
                error(err,'d');
                if(buffer[0]=='\n'){
                    free(buffer);
                    break;
                }
                strcat(filename,buffer);
                free(buffer);
            }

            int remaining=fsize;
            char* DumpFilePath = (char*) malloc((strlen(loc) + 1 + strlen(filename))*sizeof(char));
            strcpy(DumpFilePath, loc);
            strcat(DumpFilePath, "/");
            strcat(DumpFilePath, filename);
            dst_fd = open(DumpFilePath, O_CREAT|O_WRONLY, 0644);
            while(remaining>0){
                char* buffer2 = (char*) malloc(4096 * sizeof(char));
                if(remaining<=4096){
                    err=read(src_fd,buffer2,remaining);
                    remaining=0;
                }
                else{
                    err=read(src_fd,buffer2,4096);
                    remaining-=4096;
                }
                error(err,'d');
                n=err;
                if(n==0) break;
                
                err=write(dst_fd,buffer2,n);
                error(err,'d');
                free(buffer2);
            }
            free(filesize);
            free(filename);
        }
    }
    else if(strcmp("-e",argv[1])==0){ //extract single file
        if(argc!=4){
            error(-1,'e');
        }
        struct dirent *pDirent;
        DIR *pDir;
        int pos=-1;
        for(int i=strlen(argv[2])-1;i>=0;i--){
            if(argv[2][i]=='/'){
                pos=i-1;break;
            }
        }
        char *foldername=malloc(pos+10);
        for(int i=0;i<=pos;i++){
            foldername[i]=argv[2][i];
        }
        
        pDir=opendir(foldername);
        if(pDir==NULL){
            error(-1,'e');
        }
        char *p=malloc(strlen(foldername)+35);
        strcpy(p,foldername);strcat(p,"/");strcat(p,"IndividualDump");
        DIR* dir=opendir(p);
        if(dir){
            closedir(dir);
        }
        else{
            error(mkdir(p,0777),'e');
        }
        
        int src_fd,dst_fd,err,n;
        int flag=0;
        char *path=malloc(strlen(argv[2])+1);
        strcpy(path,argv[2]);
        src_fd=open(path,O_RDONLY);
        char* filecount=malloc(6);
        char* buffer = (char*) malloc(1*sizeof(char));
        err=read(src_fd,buffer,1);
        error(err,'e');
        strcpy(filecount,buffer);
        free(buffer);
        while(1){
            char* buffer = (char*) malloc(1*sizeof(char));
            err=read(src_fd,buffer,1);
            error(err,'e');
            if(buffer[0]=='\n'){
                free(buffer);
                break;
            }
            strcat(filecount,buffer);
            free(buffer);
        }
        int fcount=atoi(filecount);
        for(int i=1;i<=fcount;i++){
            char* filesize=(char*)malloc(11*sizeof(char));
            char* buffer = (char*) malloc(1*sizeof(char));
            err=read(src_fd,buffer,1);
            error(err,'e');
            strcpy(filesize,buffer);
            free(buffer);
            while(1){
                char* buffer = (char*) malloc(1*sizeof(char));
                err=read(src_fd,buffer,1);
                error(err,'e');
                if(buffer[0]=='\n'){
                    free(buffer);
                    break;
                }
                strcat(filesize,buffer);
                free(buffer);
            }
            int fsize=atoi(filesize);
            char *fname = (char*)malloc(20*sizeof(char));
            buffer = (char*) malloc(1*sizeof(char));
            err=read(src_fd,buffer,1);
            error(err,'e');
            strcpy(fname,buffer);
            free(buffer);
            while(1){
                char* buffer = (char*) malloc(1*sizeof(char));
                err=read(src_fd,buffer,1);
                error(err,'e');
                if(buffer[0]=='\n'){
                    free(buffer);
                    break;
                }
                strcat(fname,buffer);
                free(buffer);
            } 
            if(strcmp(fname,argv[3])==0){
                flag=1;
                
                char *finalpath=malloc( (strlen(p)+1+strlen(argv[3])) * sizeof(char));
                strcpy(finalpath,p);strcat(finalpath,"/");strcat(finalpath,argv[3]);
                dst_fd=open(finalpath,O_CREAT|O_WRONLY,0644);
                error(dst_fd,'e');
            }
            else flag = 0;
            int remaining = fsize;
            while(remaining>0){
                char* buffer2 = (char*) malloc(4096 * sizeof(char));
                if(remaining<=4096){
                    err=read(src_fd,buffer2,remaining);
                    remaining=0;
                }
                else{
                    err=read(src_fd,buffer2,4096);
                    remaining-=4096;
                }
                error(err,'e');
                n=err;
                if(n==0) break;
                if(flag){
                    err=write(dst_fd,buffer2,n);
                    error(err,'e');
                }
                free(buffer2);
            }
            free(filesize);
            free(fname);
            if(flag) break;
        }
        if(flag==0){
            printf("%s\n","No such file is present in tar file.");
            exit(0);
        }
        close(src_fd);
        closedir(pDir);
    }
    else if(strcmp("-l",argv[1])==0){ //list the contents
        if(argc!=3){ //error
            error(-1,'l');
        }
        struct dirent *pDirent;
        DIR *pDir;
        char *folderpath=malloc(strlen(argv[2]));
        int pos=-1;
        for(int i=strlen(argv[2])-1;i>=0;i--){
            if(argv[2][i]=='/'){
                pos=i-1;
                break;
            }
        }
        for(int i=0;i<=pos;i++){
            folderpath[i]=argv[2][i];
        }
        pDir=opendir(folderpath);
        if(pDir==NULL){
            error(-1,'l');
        }
        char *name=malloc(strlen(argv[2])+10);
        for(int i=pos+2;i<strlen(argv[2])-4;i++){
            name[i-pos-2]=argv[2][i];
        }
        char *store = (char*)malloc( (strlen(folderpath)+1+12+1) * sizeof(char));
        strcpy(store,folderpath);strcat(store,"/tarStructure");
        int src_fd,dst_fd,err,n;
        dst_fd = open(store, O_CREAT|O_WRONLY, 0644);
        char *path=malloc(strlen(folderpath)+1+strlen(name)+5);
        strcpy(path,folderpath);strcat(path,"/");strcat(path,name);strcat(path,".tar");
        src_fd=open(path,O_RDONLY);
        char* tarsize = (char*) malloc(12 * sizeof(char));
        int tarsizeVal = lseek(src_fd, 0, SEEK_END);
        sprintf(tarsize, "%d", tarsizeVal);
        err = write(dst_fd, tarsize, 1+(int)log10(tarsizeVal));
        error(err, 'l');
        err = write(dst_fd, "\n", 1);
        error(err, 'l');
        lseek(src_fd, 0, SEEK_SET);
        char* filecount = (char*)malloc(6 * sizeof(char));
        char* buffer = (char*) malloc(1 * sizeof(char));
        err=read(src_fd,buffer,1);
        error(err,'l');
        strcpy(filecount,buffer);
        free(buffer);
        while(1){
            char* buffer = (char*) malloc(1 * sizeof(char));
            err=read(src_fd,buffer,1);
            error(err,'l');
            if(buffer[0]=='\n'){
                free(buffer);
                break;
            }
            strcat(filecount,buffer);
            free(buffer);
        }
        while(1){
            char* filesize=(char*)malloc(15*sizeof(char));
            char* buffer = (char*) malloc(1 * sizeof(char));
            err=read(src_fd,buffer,1);
            error(err,'d');
            if(err==0) break;
            strcpy(filesize,buffer);
            free(buffer);
            while(1){
                char* buffer = (char*) malloc(1 * sizeof(char));
                err=read(src_fd,buffer,1);
                error(err,'d');
                if(buffer[0]=='\n'){
                    free(buffer);
                    break;
                }
                strcat(filesize,buffer);
                free(buffer);
            }
            int fsize=atoi(filesize);
            char* filename = (char*) malloc(20*sizeof(char));
            buffer = (char*) malloc(1 * sizeof(char));
            err=read(src_fd,buffer,1);
            error(err,'d');
            strcpy(filename,buffer);
            free(buffer);
            while(1){
                char* buffer = (char*) malloc(1 * sizeof(char));
                err=read(src_fd,buffer,1);
                error(err,'d');
                if(buffer[0]=='\n'){
                    free(buffer);
                    break;
                }
                strcat(filename,buffer);
                free(buffer);
            }
            int otherinfoLen = (strlen(filename) + 1 + strlen(filesize) + 1);
            char* otherinfo = (char*) malloc(otherinfoLen * sizeof(char));
            strcpy(otherinfo, filename);
            strcat(otherinfo, " ");
            strcat(otherinfo, filesize);
            strcat(otherinfo, "\n");
            err = write(dst_fd, otherinfo, otherinfoLen);
            error(err, 'l');
            int remaining=fsize;
            while(remaining>0){
                char* buffer2 = (char*) malloc(4096 * sizeof(char));
                if(remaining<=4096){
                    err=read(src_fd,buffer2,remaining);
                    remaining=0;
                }
                else{
                    err=read(src_fd,buffer2,4096);
                    remaining-=4096;
                }
                error(err,'l');
                n=err;
                if(n==0) break;
                free(buffer2);
            }
            free(filesize);
            free(filename);
        }
    }
    else{
        //handle error
    }
    return 0;
}