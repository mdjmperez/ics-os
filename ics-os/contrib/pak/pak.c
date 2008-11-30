/**
 * File packer which packs many files into a single file.
 * written by Joseph Anthony C. Hermocilla
 */ 


#include "../../sdk/dexsdk.h"

#define MAXPAK 10

struct _pak_header{
   char magic[2];  /* 'P' and 'K' */
   int trailer_pos; /* The position of the trailer */
};

struct _pak_entry{
   char fname[15]; /* The filename */
   int pos;    /* Offset of this entry in the pak */
   int size;      /* The size of this entry */
};

struct _pak_trailer{
   int num_entries; /* The number of entries */   
   struct _pak_entry entries[MAXPAK]; /* Entries */
};

/* extract a pak file */
void extract(char *pak){
   FILE *fp_pak;
   FILE *fp;
   char *buf;
   struct _pak_header pak_header;
   struct _pak_trailer pak_trailer;
   int pak_found=0;  
   unsigned long fsize;
   int i;
 
   /*Open the pak file if it exists */
   fp_pak = fopen(pak,"r");
   if (fp_pak != NULL){
     fread(&pak_header,sizeof(pak_header),1,fp_pak);
     if (pak_header.magic[0] != 'P' && pak_header.magic[1] != 'K'){
        printf("Not a valid pak file!");
        exit(1);
     }
     pak_found=1;
   }

   if (pak_found){
     fseek(fp_pak,pak_header.trailer_pos,SEEK_SET);
     fread(&pak_trailer,sizeof(pak_trailer),1,fp_pak);
     printf("Found %d files in pak.\n",pak_trailer.num_entries);
     for (i=0;i<pak_trailer.num_entries;i++){
       printf("Extracting %s with %d bytes...",pak_trailer.entries[i].fname,
               (int)pak_trailer.entries[i].size);
       fp=fopen(pak_trailer.entries[i].fname,"w");
       buf=(char *)malloc(pak_trailer.entries[i].size);
       fseek(fp_pak,pak_trailer.entries[i].pos,SEEK_SET);
       fread(buf,pak_trailer.entries[i].size,1,fp_pak);  
       fwrite(buf,pak_trailer.entries[i].size,1,fp);
       fclose(fp);
       
       printf("done\n");;
     }
   }else{
     printf("Pack file not found!");
   }

}


/* Adds a file to a pak */
void add(char *pak, char *fname){
   FILE *fp_pak;
   FILE *fp;
   char *buf;
   struct _pak_header pak_header;
   struct _pak_trailer pak_trailer;
   int pak_found=0;  
   int fsize;
   int old_trailer_pos;
 
 
   /*Open the pak file if it exists */
   fp_pak = fopen(pak,"r");
   if (fp_pak != NULL){
     fread(&pak_header,sizeof(pak_header),1,fp_pak);
     if (pak_header.magic[0] != 'P' && pak_header.magic[1] != 'K'){
        printf("Not a valid pak file!");
        exit(1);
     }
     pak_found=1;
   }

   if (!pak_found){
     pak_header.magic[0]='P'; 
     pak_header.magic[1]='K'; 
     fp_pak=fopen(pak,"w");
     fp = fopen(fname,"r");
     if (fp == NULL){
       printf("%s not found!\n",fname);
       exit(1);
     }
     fseek(fp,0,SEEK_END);
     fsize=ftell(fp);    
     fseek(fp,0,SEEK_SET);
     buf=(char *)malloc(fsize);
     fread(buf,fsize,1,fp);
     fclose(fp);
     
     pak_trailer.num_entries=1;
     strcpy(pak_trailer.entries[0].fname,fname);
     pak_trailer.entries[0].pos=sizeof(pak_header);
     pak_trailer.entries[0].size=fsize;

     pak_header.trailer_pos=fsize+sizeof(pak_header);

     printf("Adding %s with %d bytes...",fname,fsize);
     fwrite(&pak_header,sizeof(pak_header),1,fp_pak);
     fwrite(buf,fsize,1,fp_pak);
     fwrite(&pak_trailer,sizeof(pak_trailer),1,fp_pak);
     fclose(fp_pak);
     printf("done\n");
   }else{
     fclose(fp_pak);
     fp_pak=fopen(pak,"rw+");
     fseek(fp_pak,pak_header.trailer_pos,SEEK_SET);
     fread(&pak_trailer,sizeof(pak_trailer),1,fp_pak);
     fp = fopen(fname,"r");
     if (fp == NULL){
       printf("%s not found!\n",fname);
       exit(1);
     }
     fseek(fp,0,SEEK_END);
     fsize=ftell(fp);    
     fseek(fp,0,SEEK_SET);
     buf=(char *)malloc(fsize);
     fread(buf,fsize,1,fp);
     fclose(fp);

     old_trailer_pos=pak_header.trailer_pos;

     pak_trailer.num_entries++;
     strcpy(pak_trailer.entries[pak_trailer.num_entries-1].fname,fname);
     pak_trailer.entries[pak_trailer.num_entries-1].pos=old_trailer_pos;
     pak_trailer.entries[pak_trailer.num_entries-1].size=fsize;
    
     pak_header.trailer_pos+=fsize;

     printf("Adding %s with %d bytes...",fname,fsize);

     fseek(fp_pak,0,SEEK_SET);
     fwrite(&pak_header,sizeof(pak_header),1,fp_pak);
     fseek(fp_pak,0,SEEK_SET);
     fseek(fp_pak,old_trailer_pos,SEEK_SET);
     fwrite(buf,fsize,1,fp_pak);
     fwrite(&pak_trailer,sizeof(pak_trailer),1,fp_pak);
     fclose(fp_pak);
     printf("done\n");

   }

}


int main(int argc, char **argv) 
{

   printf("PAK 1.0 for ICS-OS\n");
   if (argc >= 3){
     if (argv[1][0]=='a'){
       add(argv[2],argv[3]);
     }
     else if (argv[1][0]=='x'){
       extract(argv[2]);
     }else{
       printf("Usage: pak.exe [a/x] <pak file> <file>\n");
     }
   }else{
     printf("Usage: pak.exe [a/x] <pak file> <file>\n");
   }
   return 0;
}