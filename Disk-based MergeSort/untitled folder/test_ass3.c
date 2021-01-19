#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>

typedef enum{false,true} bool;
/* maximum buffer size*/
#define MAX_LENGTH 1000
#define SUPER_LENGTH 15
#define PART_IND 750
#define REPLACE_LEN 250

typedef struct{
  int elem;
  int run_num;
}runelem;

typedef struct{
  int cnt;/*how many elem the runset left */
  FILE *fp;/* the pointer to the run file */
}runinfo;

int input[MAX_LENGTH];
int output[MAX_LENGTH];
char run_fname_prefix[]="input.bin.";
char super_run_prefix[]="input.bin.super.";

void generate_runfiles(FILE *fp,runinfo *rinfo_ptr);

int i,j,k;
int smallest;
int total_length;
int last_run_length;
int last_full_run_ind;
char *fname;
char *output_fname;
int run_number=0;

FILE *fp;/* input.bin */

void basic_mergesort(runinfo *rinfo,runelem *relem,int start_ind,int run_number,FILE *fp_output,char *prefix,char *run_fname);

void memswap(char *m1,char *m2,int extent);
void heapify(char *input,int size,int len, int(*compare)(const void*,const void*));
void heap_sort(char *input,int size,int len,int(*compare)(const void*,const void*));
int compare_runelem(const void *a,const void *b);
int compare_int(const void *a,const void *b);
int compare_int_ascend(const void *a,const void *b);

int main(int argc,char *argv[]){

  if(argc!=4){
      printf("Command line arguments are incorrect.\n");
      exit(1);
  }
  fname=argv[2];
  output_fname=argv[3];
  if((fp=fopen(fname,"r+b"))==NULL){
    perror("file not exist");
    exit(1);
  }

  fseek(fp, 0, SEEK_END);
  total_length = (ftell(fp))/sizeof(int);
  fseek(fp,0,SEEK_SET);
  last_run_length=total_length%MAX_LENGTH;
  last_full_run_ind=total_length/MAX_LENGTH;
  run_number=(last_run_length==0?last_full_run_ind:(last_full_run_ind+1));
  runinfo rinfo[MAX_LENGTH];
  runelem relem[MAX_LENGTH];

  char *run_fname=malloc(sizeof(run_fname_prefix)+sizeof(4));


    struct timeval start,end,diff;
    gettimeofday(&start, NULL);

  if(strcmp(argv[1],"--basic")==0){

    generate_runfiles(fp,rinfo);
      FILE *fp_output=fopen(output_fname,"a+b");

      basic_mergesort(rinfo, relem, 0,run_number,fp_output,run_fname_prefix,run_fname);
  }
  else if(strcmp(argv[1],"--multistep")==0){

    generate_runfiles(fp,rinfo);
    int file_ind=0;
    int super_ind=0;
    int files_around;
    char *superrun_fname=malloc(sizeof(super_run_prefix)+sizeof(4));

    while(file_ind<run_number){
        FILE *fp_output;
        sprintf(superrun_fname, "%s%03d", super_run_prefix,super_ind++);
        fp_output=fopen(superrun_fname,"a+b");
        if((file_ind+SUPER_LENGTH)>run_number){
          files_around=run_number-file_ind;
        }
        else{
          files_around=SUPER_LENGTH;
        }

      basic_mergesort(rinfo, relem, file_ind,files_around,fp_output,run_fname_prefix,run_fname);
      file_ind+=SUPER_LENGTH;

    }

    FILE *fp_output;/* sort.bin */
    fp_output=fopen(output_fname,"a+b");

    runinfo sinfo[super_ind];
    basic_mergesort(sinfo, relem, 0, super_ind, fp_output, super_run_prefix, superrun_fname);

  }
  else if(strcmp(argv[1],"--replacement")==0){
    fread(input, sizeof(int), MAX_LENGTH, fp);
    int heap_length=PART_IND;
    int heap_head=0;
    int buf_ind=PART_IND;
    int output_ind=0;
    int file_ind=0;

    FILE *fp_output;
    int blocksize=REPLACE_LEN;
    int q;
    int writes = 0;
    while(true){

    heapify(input, sizeof(int), heap_length, compare_int);

    q++;

    //printf("%d\t%d\n",q ,input[0]);
    //if(q == 10)
      //break;

    smallest=input[heap_head];
    output[output_ind++]=smallest;

    if(smallest<=input[buf_ind]){/* If H1 ≤ B1, replace H1 with B1. */
      input[heap_head]=input[buf_ind++];
    }
    else{ /* If H1 > B1, replace H1 with H750, reducing the size of the heap by one. Replace H750 with B1, increasing the size of the secondary heap by one. */
      heap_length--;
      input[heap_head]=input[heap_length];
      input[heap_length]=input[buf_ind++];
    }
    if(buf_ind==MAX_LENGTH){
      blocksize=fread(input+PART_IND, sizeof(int), REPLACE_LEN, fp);
      buf_ind=PART_IND;
    }

    if(output_ind==MAX_LENGTH || heap_length==0 || blocksize==0){

      sprintf(run_fname, "%s%03d", run_fname_prefix,file_ind);
      fp_output=fopen(run_fname, "a+b");
      fwrite(output, sizeof(int), output_ind, fp_output);
      writes += output_ind;
      fclose(fp_output);
      output_ind=0;
      if(heap_length==0 || blocksize==0){
        file_ind++;
        heap_length=PART_IND;
      }
    }
    if(blocksize==0){
      sprintf(run_fname, "%s%03d", run_fname_prefix,file_ind);
      fp_output=fopen(run_fname, "a+b");
      heap_sort(input, sizeof(int), heap_length,compare_int_ascend);
      fwrite(input, sizeof(int), heap_length, fp_output);
      writes += heap_length;
      fclose(fp_output);
      file_ind++;
      break;
    }
  }

  fp_output=fopen(output_fname, "a+b");
  basic_mergesort(rinfo, relem, 0,file_ind,fp_output,run_fname_prefix,run_fname);/* merge the run files we created in basic method. */
  printf("%d\n", writes);
  }
  else{
    printf("mergesort-method not correct. --basic  --multistep  --replacement\n");
    fclose(fp);
    return 0;
  }
  fclose(fp);

  gettimeofday(&end, NULL);

  diff.tv_sec=end.tv_sec-start.tv_sec;
  diff.tv_usec=end.tv_usec-start.tv_usec;
  if(diff.tv_usec<0){
    diff.tv_usec+=1000000;
    diff.tv_sec--;
  }

  printf("Time: %ld.%06ld\n",diff.tv_sec,diff.tv_usec);

  return 0;
}

void generate_runfiles(FILE *fp,runinfo *rinfo_ptr){
    int i=0;
    char *run_fname=malloc(sizeof(run_fname_prefix)+sizeof(4));
    while(i<run_number){
    fread(input,sizeof(int),i==last_full_run_ind?last_run_length:MAX_LENGTH,fp);
    heap_sort(input, sizeof(int),i==last_full_run_ind?last_run_length:MAX_LENGTH , compare_int_ascend);
    sprintf(run_fname,"%s%03d",run_fname_prefix,i++);
    FILE *fp_run=fopen(run_fname,"w+b");
    fseek(fp_run,0,SEEK_SET);
    fwrite(input,sizeof(int),MAX_LENGTH,fp_run);
    fclose(fp_run);
    }
}

void basic_mergesort(runinfo *rinfo,runelem *relem,int start_ind,int run_number,FILE *fp_output,char *prefix,char *run_fname){
  int count_everyrun=MAX_LENGTH/run_number;
  int i=0,heap_length=0;
  int blocksize;
  for(;i<run_number;++i){
    sprintf(run_fname,"%s%03d",prefix,start_ind+i);
    rinfo[start_ind+i].fp=fopen(run_fname,"r+b");
    /*
    if(rinfo[start_ind+i].fp==NULL){
      perror("run file not exist.\n");
      return;
    }
    */
    blocksize=fread(input+heap_length, sizeof(int),count_everyrun, rinfo[start_ind+i].fp);
    k=0;
    for(;k<blocksize;++k){
      relem[heap_length+k].elem=input[heap_length+k];
      relem[heap_length+k].run_num=start_ind+i;
    }
    rinfo[start_ind+i].cnt=blocksize;
    heap_length+=rinfo[start_ind+i].cnt;
  }

  int heap_head=0;
  int output_ind=0;
  int tmpblock[count_everyrun];
  int run_ind;

  while(true){
    heapify(relem, sizeof(runelem), heap_length, compare_runelem);
    smallest=relem[heap_head].elem;
    output[output_ind++]=smallest;
    if(output_ind==MAX_LENGTH){
      fwrite(output, sizeof(int), output_ind, fp_output);
      output_ind=0;
      
      int x;
      for(x=0; x<1000; x++){
        //printf("%d\t%d\n",x ,output[x]);
      }
      //fclose(fp_output);
      //break;
    }
    run_ind=relem[heap_head].run_num;

    relem[heap_head].elem=relem[heap_length-1].elem;
    relem[heap_head].run_num=relem[heap_length-1].run_num;
    heap_length--;

    rinfo[run_ind].cnt--;
    if(rinfo[run_ind].cnt==0){
      blocksize=fread(tmpblock,sizeof(int),count_everyrun,rinfo[run_ind].fp);
      if(blocksize!=0){
        k=0;
        for(;k<blocksize;++k){
          relem[heap_length].elem=tmpblock[k];
          relem[heap_length].run_num=run_ind;
          heap_length++;
        }
        rinfo[run_ind].cnt=blocksize;
      }
      else{
        fclose(rinfo[run_ind].fp);
        if(heap_length==0){
          break;
        }
      }
    }
  }
  fclose(fp_output);

}

void sift(char * input,int size,int i,int len, int(*compare)(const void*,const void*)){
  while(i<len/2){
    int j=i*2+1;
    int k=j+1;
    int lg;
    if(k<len&&compare(input+k*size,input+j*size)>=0){
      lg=k;
    }
    else{
      lg=j;
    }
    if(compare(input+i*size,input+lg*size)>=0){
      return;
    }
    memswap(input+i*size,input+lg*size,size);
    i=lg;
  }
}

/*swap to memory block, extent is the size */
void memswap(char *m1,char *m2,int extent){
  char *c1=(char *)(m1);
  char *c2=(char *)(m2);
  if(m1!=m2){
    while(extent--){
      char tmp=*c1;
      *c1++=*c2;
      *c2++=tmp;
    }
  }
}

void heapify(char *input,int size,int len, int(*compare)(const void*,const void*)){
  int i=len/2-1;
  while(i>=0){
    sift(input, size, i, len, compare);
    i--;
  }
}

void heap_sort(char *input,int size,int len,int(*compare)(const void*,const void*)){
  heapify(input,size,len,compare);
  int i=len-1;
  while(i>=1){
    memswap(input, input+i*size, size);
    i--;
    sift(input, size, 0, i+1, compare);
  }
}

/*to generate min heap */
int compare_runelem(const void *a,const void *b){
  const runelem *ia=(const runelem*)a;
  const runelem *ib=(const runelem*)b;
  return (*ib).elem-(*ia).elem;
}

/* to generate min heap*/
int compare_int(const void *a,const void *b){
  const int *ia=(const int*)a;
  const int *ib=(const int*)b;
  return *ib - * ia;
}
/* heap sort ascendingly */
int compare_int_ascend(const void *a,const void *b){
  const int *ia=(const int*)a;
  const int *ib=(const int*)b;
  return *ia-*ib;
}