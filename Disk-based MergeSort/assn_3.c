#include <stdio.h>
#include <string.h>
#include <sys/time.h>

//structures
typedef struct{
	int rid;
	int val;
}rprof;

typedef struct{
	FILE *rptr;
	int left;
}rdata;

//global variables
char input_file[100];
char output_file[100];
FILE *iptr;
FILE *optr;
long isize;
int total_runs;
int total_super_runs;
int final_run_complete = 1;
int lrl;
int lfri;
rprof rp[1000];
rdata rd[1000];
char rname[100];
char sname[100];
int smallest;

int total_super_runs;

int buffer[1000];
int buffer_out[1000];
int bufferout[100000];

//function declarations
void basic_disk_merge();
void multistep_disk_merge();
void replacement_disk_merge();
void initialize();
void split_to_runs();
void disk_merge_sort(int sind, int super, int rlen, FILE *optr);
void disk_merge_sort2(int sind, int super, int rlen, FILE *optr);
void disk_merge_sort3(int sind, int super, int rlen, FILE *optr);
void heapify(rprof A[], int i, int n);
int compare_heap(int a, int b);
//void sift(int(*weigh)(const void *, const void *), int weight, int length, char * );
void build_heap(rprof A[], int n);
//similar to qsort
void hsort(rprof A[], int n);

void heapify2(int A[], int i, int n);
void build_heap2(int A[], int n);
//similar to qsort
void hsort2(int A[], int n);
void relocation_read_buffer(int *sec_hp_ind, int *elem_read);
void relocation_write_exhaust(int *pr_hp_len, int *bo, int *elem_read, int *r, FILE* rptr);
void relocation_last(int elem_read, int r, FILE* rptr, int pr_hp_len);


void initialize(){
		
	//printf("\nokay\n");
	//printf("\n%s", input_file);
	iptr = fopen(input_file, "rb");
	optr = fopen(output_file, "wb");
	//printf("\n%d", iptr);

	//get the number of records
	fseek(iptr, 0, SEEK_END);
	isize = ftell(iptr)/sizeof(int);
	fseek(iptr, 0, SEEK_SET);

	//get the number of runs
	if(isize%1000 == 0){
		total_runs = isize/1000;
	}
	else{
		total_runs = (isize/1000) + 1;
		final_run_complete = 0;
	}

	lfri = isize/1000;
	lrl = isize % 1000;

	//printf("\n%d, %d", isize, total_runs);

	//sprintf(rname, "%s.%03d%", input_file, 1);
	//printf("\n%s\n", input_file);
	//printf("\n%s\n", rname);

}

int cmp_qsort(const void *a, const void *b){
	int sa = (*(int*)a);
	int sb = (*(int*)b);

	if(sa < sb)
		return -1;
	else if(sa > sb)
		return 1;
	else
		return 0;
}


void split_to_runs(){
	int r, i, j, key, val;
	FILE *rptr;
	int run[1000];

	fseek(iptr, 0, SEEK_SET);
	for(r = 0; r<total_runs; r++){
		sprintf(rname, "%s.%03d", input_file, r);
		//printf("Opening\n");
		rptr = fopen(rname, "wb");
		if(r == lfri){
			fread(run, sizeof(int), lrl, iptr);
			qsort(run, lrl, sizeof(int), cmp_qsort);
			fwrite(run, sizeof(int), lrl, rptr); 
			//rd[r].rptr = rptr;
			//printf("%d\n", rd[r].rptr);

		}
		else{
			fread(run, sizeof(int), 1000, iptr);
			qsort(run, 1000, sizeof(int), cmp_qsort);
			fwrite(run, sizeof(int), 1000, rptr);
			//rd[r].rptr = rptr;
			//printf("%d\n", rd[r].rptr);

		}

		fclose(rptr);
	}

	//int m = 0;
	//fseek(rd[0].rptr, 0, SEEK_SET);
	//fread(&m, sizeof(int), 1, rd[0].rptr);
	//printf("%d\n", m);

}

void heapify(rprof A[], int i, int n){

	//printf("heapify\n");

    int left = 2*i+1;
    int right = left + 1;
    int largest;
    rprof temp;
    if(left < n && A[left].val > A[i].val ){
    	largest = left;
    }
    else{
    	largest = i;
    }
    if(right < n && A[right].val > A[largest].val){
    	largest = right;
    }
    if( largest != i){
    	temp.val = A[i].val;
    	temp.rid = A[i].rid;

    	A[i].val = A[largest].val;
    	A[i].rid = A[largest].rid;

    	A[largest].val = temp.val;
    	A[largest].rid = temp.rid;
    	heapify(A, largest, n);
    }
}

int compare_heap(int a, int b){
	if(a > b)
		return 1;
	else
		return 0;
}

void build_heap(rprof A[], int n){

	//printf("build heap\n");

    int i;
    for(i = n/2-1; i>=0; i--){
    	heapify(A, i, n);
    }
}

void hsort(rprof A[], int n){

	//printf("hsort\n");

   	rprof temp;	

   	int i;
   	build_heap(A, n);
   	for(i = n-1; i>=1; i--){
   		temp.val = A[0].val;
    	temp.rid = A[0].rid;

    	A[0].val = A[i].val;
    	A[0].rid = A[i].rid;

    	A[i].val = temp.val;
    	A[i].rid = temp.rid;

    	n -= 1;
    	heapify(A, 0, n);
   	}
}



void heapify2(int A[], int i, int n){

	//printf("heapify\n");
    int left = 2*i+1;
    int right = left + 1;
    int largest;
    int temp;
    if(left < n && A[left] > A[i] ){
    	largest = left;
    }
    else{
    	largest = i;
    }
    if(right < n && A[right] > A[largest]){
    	largest = right;
    }
    if( largest != i){
    	temp = A[i];
    	A[i] = A[largest];
    	A[largest] = temp;
    	heapify2(A, largest, n);
    }
}

void build_heap2(int A[], int n){

	//printf("build heap\n");

    int i;
    for(i = n/2-1; i>=0; i--){
    	heapify2(A, i, n);
    }
}

void hsort2(int A[], int n){

	//printf("hsort\n");
   	int temp;	
   	int i;
   	build_heap2(A, n);
   	for(i = n-1; i>=1; i--){
   		temp = A[0];
    	A[0] = A[i];
    	A[i] = temp;

    	n -= 1;
    	heapify2(A, 0, n);
   	}
}


void disk_merge_sort(int sind, int super, int rlen, FILE* optr){
	//copy parts of runs into the input buffer

	//printf("\nSorting\n");
	int len = 0;
	int keys_per_run = 1000/rlen;
	
	int r;
	int epr, elem_read;

	//printf("%d\n", rlen);
	//printf("%d\n", keys_per_run);
	//printf("%d\n", sind);
	for(r = 0; r<rlen; r++){
		if(!super)
			sprintf(rname, "%s.%03d", input_file, r+sind);
		else
			sprintf(rname, "%s.super.%03d", input_file, r+sind);		
		//printf("Opening\n");

		rd[r+sind].rptr = fopen(rname, "rb");

    	if(rd[r+sind].rptr == NULL){
    		//printf("diesn't exist\n");
    	}
    	else{
    		//printf("exists\n");
    	}

    	//printf("Opening\n");

		//reading elements from run into the input buffer
		elem_read = fread(buffer+len, sizeof(int), keys_per_run, rd[r+sind].rptr);

		//for all the elements read,
		//store their details in the corresponding global variables
		//int epr;
		for(epr = 0; epr<elem_read; epr++){
			//storing the run number for the key
			rp[len+epr].rid = r+sind;
			//storing the key in its corresponding location
			rp[len+epr].val = buffer[len+epr];
		}

		//increase the length by the number of elements read
		len = len + elem_read;
		//number of elements left in the run
		rd[r+sind].left = elem_read;

	}

	//printf("%d\n", len);
	
	//int h;
	//for(h=990; h<1000; h++){
	//	printf("%d\n", buffer[h]);
	//}

	//printf("%d\n", rd[r].rptr);
	//printf("%d\n", keys_per_run);
	//printf("%d\n", elem_read);
	//printf("%d\n", len);
	//now that the input buffer is initially populated, 
	//merge sort is carried out
	
	int bo = 0;
	int x = 1;
	int prev = 2147483647;
	fseek(optr, 0, SEEK_SET);
	while(1){
		//printf("Everything's fine\n");
		//printf("%d\n", len);
		//break;
		//printf("running\n");
		hsort(rp, len);
		buffer_out[bo] = rp[0].val;
		bo++;
		
		//printf("%d\n", rp[0].val);
		/*//////
		//int x =0;

		printf("%d\n", buffer_out[bo-1]);
		rprof temp;
		temp.val = rp[0].val;
    	temp.rid = rp[0].rid;

    	rp[0].val = rp[len - 1].val;
    	rp[0].rid = rp[len - 1].rid;

    	rp[len-1 ].val = temp.val;
    	rp[len-1].rid = temp.rid;

    	len -= 1;
    	//x++;

    	if(bo == 100)
    		break;*/

		//////
		int run = rp[0].rid;		
		//swap the smallest value with the last value
		rprof temp;
		temp.val = rp[0].val;
    	temp.rid = rp[0].rid;

    	rp[0].val = rp[len - 1].val;
    	rp[0].rid = rp[len - 1].rid;

    	rp[len-1 ].val = temp.val;
    	rp[len-1].rid = temp.rid;

    	//the smallest element is in the last position
    	//decrease the length
    	len -= 1;
    	//printf("%d\n", len);

    	//adjust the other global array
    	rd[run].left -= 1;

    	//if the elements of a run are exhausted, read again from its run file
    	int epra[1000];
    	//int epr;
    	int eread;
    	if(rd[run].left == 0){
    		//printf("%d exhausted\n", run);
    		eread = fread(epra, sizeof(int), keys_per_run, rd[run].rptr);
    		for(epr=0; epr<eread; epr++){
    			rp[len].val = epra[epr];
    			rp[len].rid = run;
    			len += 1;
    			//printf("%d\n", len);
    		}
    		rd[run].left = eread;
    	}

    	//if output buffer has 1000 elements, write to the output file
    	if(bo == 1000){
    		bo = 0;
    		//printf("\nbo is 1000\n");
    		fwrite(buffer_out, sizeof(int), 1000, optr);
    		int x;
    		for(x=0; x<1000; x++){
    			//printf("%d\t%d\n", x, buffer_out[x]);
    		}
    		//break;
    		
    		//printf("%d\n", x);
    		//x++;
    		/*int x;
    		for(x = 0; x<999; x++){
    			if(x!=0){
    				if(buffer_out[x]>buffer_out[x+1]){
    					printf("Error\n");
    				}	
    			}
    			else{
    				if(buffer_out[x] > prev){
    					printf("Error here\n");
    					printf("\t%d\n", buffer_out[x]);
    					printf("\t%d\n", prev);
    				}
    			}
    			
    			//printf("%d\n", buffer_out[x]);
    		}
    		prev = buffer_out[999];*/
    	}

    	//if there aren't anymore elements in the buffer, exit the loop
    	if(!len){
    		//write the left over elements to the output file
    		//printf("\n%d\n", bo);

    		fwrite(buffer_out, sizeof(int), bo, optr);
    		fclose(optr);
    		break;
    	}
	}


} 

void disk_merge_sort2(int sind, int super, int rlen, FILE* optr){
	//copy parts of runs into the input buffer

	//printf("\nSorting\n");
	int len = 0;
	int keys_per_run = 1000/rlen;
	

	int r;
	int epr, elem_read;

	//store run details in the global variables


	for(r = 0; r<rlen; r++){
		if(!super)
			sprintf(rname, "%s.%03d", input_file, r+sind);
		else
			sprintf(rname, "%s.super.%03d", input_file, r+sind);		
		//printf("Opening\n");
		rd[r+sind].rptr = fopen(rname, "rb");

    	if(rd[r+sind].rptr == NULL){
    		//printf("diesn't exist\n");
    	}
    	else{
    		//printf("exists\n");
    	}

		//reading elements from run into the input buffer
		elem_read = fread(buffer+len, sizeof(int), keys_per_run, rd[r+sind].rptr);

		//for all the elements read,
		//store their details in the corresponding global variables
		//int epr;
		for(epr = 0; epr<elem_read; epr++){
			//storing the run number for the key
			rp[len+epr].rid = r+sind;
			//storing the key in its corresponding location
			rp[len+epr].val = buffer[len+epr];
		}

		//increase the length by the number of elements read
		len = len + elem_read;
		//number of elements left in the run
		rd[r+sind].left = elem_read;

	}
	
	//int h;
	//for(h=990; h<1000; h++){
	//	printf("%d\n", buffer[h]);
	//}

	//printf("%d\n", rd[r].rptr);
	//printf("%d\n", keys_per_run);
	//printf("%d\n", elem_read);
	//printf("%d\n", len);
	//now that the input buffer is initially populated, 
	//merge sort is carried out
	
	int bo = 0;
	int x = 1;
	int prev = 2147483647;
	fseek(optr, 0, SEEK_SET);
	while(1){
		//printf("Everything's fine\n");
		//printf("%d\n", len);
		//break;
		hsort(rp, len);
		bufferout[bo] = rp[0].val;
		bo++;
		
		//printf("%d\n", rp[0].val);
		/*//////
		//int x =0;

		printf("%d\n", buffer_out[bo-1]);
		rprof temp;
		temp.val = rp[0].val;
    	temp.rid = rp[0].rid;

    	rp[0].val = rp[len - 1].val;
    	rp[0].rid = rp[len - 1].rid;

    	rp[len-1 ].val = temp.val;
    	rp[len-1].rid = temp.rid;

    	len -= 1;
    	//x++;

    	if(bo == 100)
    		break;*/

		//////
		int run = rp[0].rid;		
		//swap the smallest value with the last value
		rprof temp;
		temp.val = rp[0].val;
    	temp.rid = rp[0].rid;

    	rp[0].val = rp[len - 1].val;
    	rp[0].rid = rp[len - 1].rid;

    	rp[len-1 ].val = temp.val;
    	rp[len-1].rid = temp.rid;

    	//the smallest element is in the last position
    	//decrease the length
    	len -= 1;
    	//printf("%d\n", len);

    	//adjust the other global array
    	rd[run].left -= 1;

    	//if the elements of a run are exhausted, read again from its run file
    	int epra[1000];
    	//int epr;
    	int eread;
    	if(rd[run].left == 0){
    		//printf("%d exhausted\n", run);
    		eread = fread(epra, sizeof(int), keys_per_run, rd[run].rptr);
    		for(epr=0; epr<eread; epr++){
    			rp[len].val = epra[epr];
    			rp[len].rid = run;
    			len += 1;
    			//printf("%d\n", len);
    		}
    		rd[run].left = eread;
    	}

    	//if output buffer has 1000 elements, write to the output file
    	if(bo == 1000){
    		bo = 0;
    		//printf("\nbo is 1000\n");
    		fwrite(bufferout, sizeof(int), 1000, optr);
    		int x;
    		for(x=0; x<1000; x++){
    			//printf("%d\t%d\n", x, buffer_out[x]);
    		}
    		//break;
    		
    		//printf("%d\n", x);
    		//x++;
    		/*int x;
    		for(x = 0; x<999; x++){
    			if(x!=0){
    				if(buffer_out[x]>buffer_out[x+1]){
    					printf("Error\n");
    				}	
    			}
    			else{
    				if(buffer_out[x] > prev){
    					printf("Error here\n");
    					printf("\t%d\n", buffer_out[x]);
    					printf("\t%d\n", prev);
    				}
    			}
    			
    			//printf("%d\n", buffer_out[x]);
    		}
    		prev = buffer_out[999];*/
    	}

    	//if there aren't anymore elements in the buffer, exit the loop
    	if(!len){
    		//write the left over elements to the output file
    		//printf("\n%d\n", bo);

    		fwrite(bufferout, sizeof(int), bo, optr);
    		fclose(optr);
    		break;
    	}
	}


} 

void disk_merge_sort3(int sind, int super, int rlen, FILE *optr){
	//copy parts of runs into the input buffer

	//printf("\nSorting\n");
	int keys_per_run = 1000/rlen;
	int len = 0;

	int r;
	int epr, elem_read;
	for(r = 0; r<total_runs; r++){
		sprintf(rname, "%s.%03d", input_file, r);
		//printf("Opening\n");
		rd[r+sind].rptr = fopen(rname, "rb");

    	if(rd[r+sind].rptr == NULL){
    		//printf("diesn't exist\n");
    	}
    	else{
    		//printf("exists\n");
    	}

		//reading elements from run into the input buffer
		elem_read = fread(buffer+len, sizeof(int), keys_per_run, rd[r+sind].rptr);

		//for all the elements read,
		//store their details in the corresponding global variables
		//int epr;
		for(epr = 0; epr<elem_read; epr++){
			//storing the run number for the key
			rp[len+epr].rid = r+sind;
			//storing the key in its corresponding location
			rp[len+epr].val = buffer[len+epr];
		}

		//increase the length by the number of elements read
		len = len + elem_read;
		//number of elements left in the run
		rd[r+sind].left = elem_read;

	}
	
	//int h;
	//for(h=990; h<1000; h++){
	//	printf("%d\n", buffer[h]);
	//}

	//printf("%d\n", rd[r].rptr);
	//printf("%d\n", keys_per_run);
	//printf("%d\n", elem_read);
	//printf("%d\n", len);
	//now that the input buffer is initially populated, 
	//merge sort is carried out
	
	int bo = 0;
	int x = 1;
	int prev = 2147483647;
	fseek(optr, 0, SEEK_SET);
	while(1){
		//printf("Everything's fine\n");
		//printf("%d\n", len);
		//break;
		hsort(rp, len);
		buffer_out[bo] = rp[0].val;
		bo++;
		
		//printf("%d\n", rp[0].val);
		/*//////
		//int x =0;

		printf("%d\n", buffer_out[bo-1]);
		rprof temp;
		temp.val = rp[0].val;
    	temp.rid = rp[0].rid;

    	rp[0].val = rp[len - 1].val;
    	rp[0].rid = rp[len - 1].rid;

    	rp[len-1 ].val = temp.val;
    	rp[len-1].rid = temp.rid;

    	len -= 1;
    	//x++;

    	if(bo == 100)
    		break;*/

		//////
		int run = rp[0].rid;		
		//swap the smallest value with the last value
		rprof temp;
		temp.val = rp[0].val;
    	temp.rid = rp[0].rid;

    	rp[0].val = rp[len - 1].val;
    	rp[0].rid = rp[len - 1].rid;

    	rp[len-1 ].val = temp.val;
    	rp[len-1].rid = temp.rid;

    	//the smallest element is in the last position
    	//decrease the length
    	len -= 1;
    	//printf("%d\n", len);

    	//adjust the other global array
    	rd[run].left -= 1;

    	//if the elements of a run are exhausted, read again from its run file
    	int epra[1000];
    	//int epr;
    	int eread;
    	if(rd[run].left == 0){
    		//printf("%d exhausted\n", run);
    		eread = fread(epra, sizeof(int), keys_per_run, rd[run].rptr);
    		for(epr=0; epr<eread; epr++){
    			rp[len].val = epra[epr];
    			rp[len].rid = run;
    			len += 1;
    			//printf("%d\n", len);
    		}
    		rd[run].left = eread;
    	}

    	//if output buffer has 1000 elements, write to the output file
    	if(bo == 1000){
    		bo = 0;
    		//printf("\nbo is 1000\n");
    		fwrite(buffer_out, sizeof(int), 1000, optr);
    		int x;
    		for(x=0; x<1000; x++){
    			//printf("%d\t%d\n", x, buffer_out[x]);
    		}
    		//break;
    		
    		//printf("%d\n", x);
    		//x++;
    		/*int x;
    		for(x = 0; x<999; x++){
    			if(x!=0){
    				if(buffer_out[x]>buffer_out[x+1]){
    					printf("Error\n");
    				}	
    			}
    			else{
    				if(buffer_out[x] > prev){
    					printf("Error here\n");
    					printf("\t%d\n", buffer_out[x]);
    					printf("\t%d\n", prev);
    				}
    			}
    			
    			//printf("%d\n", buffer_out[x]);
    		}
    		prev = buffer_out[999];*/
    	}

    	//if there aren't anymore elements in the buffer, exit the loop
    	if(!len){
    		//write the left over elements to the output file
    		//printf("\n%d\n", bo);

    		fwrite(buffer_out, sizeof(int), bo, optr);
    		fclose(optr);
    		break;
    	}
	}


} 



void basic_disk_merge(){
	//split the file to runs
	split_to_runs();
	disk_merge_sort3(0, 0, 1000/total_runs, optr);
}

void multistep_disk_merge(){
	split_to_runs();

	//merge sets of 15 runs to get super runs

	//get the number of super runs
	if(total_runs%15 != 0){
		total_super_runs = total_runs/15 +1;
	}
	else{
		total_super_runs = total_runs/15;
	}

	//printf("%d\n", total_super_runs);

	int sind = 0;
	int sr = 0;
	int rlen;

	for(sr=0; sr<total_super_runs; sr++){
		sprintf(sname, "%s.super.%03d", input_file, sr);

		if(sr != total_super_runs-1){
			optr = fopen(sname, "wb");
			disk_merge_sort(sind, 0, 15, optr);
		}
		else{
			if(total_runs%15 == 0){
				optr = fopen(sname, "wb");
				disk_merge_sort(sind, 0, 15, optr);
			}
			else{
				optr = fopen(sname, "wb");
				disk_merge_sort(sind, 0, total_runs%15, optr);
			}
		}

		sind = (sr+1)*15;
	}
	optr = fopen(output_file, "wb");
	/*while(1){
		if(total_runs < sind){
			break;
		}
		sprintf(sname, "%s.super.%03d", input_file, sr);
		if()


		optr = fopen(sname, "wb");

		disk_merge_sort(sind, 0, rlen, optr);

		sr += 1;
	}*/
	disk_merge_sort(0, 1, total_super_runs, optr);
	//fclose(optr);
}

void replacement_disk_merge(){

	int pr_hp_len = 750;
	int sec_hp_len = 250;
	fseek(iptr, 0, SEEK_SET);
	int elem_read = fread(buffer, sizeof(int), 1000, iptr);

	int pr_hp_ind = 0;
	int sec_hp_ind = 750;

	optr = fopen(output_file, "ab");

	int r=0, bo = 0;
	FILE *rptr;
	int writes = 0;
	int q=0;
	while(1){
		//printf("Error here\n");
		//hsort2(buffer, pr_hp_len);
		qsort(buffer, pr_hp_len, sizeof(int), cmp_qsort);
		q++;

		//printf("%d\t%d\n",q ,buffer[0]);
		//if(q == 10)
		//	break;
		
		bufferout[bo] = buffer[pr_hp_ind];
		bo++;

		if(buffer[pr_hp_ind] <= buffer[sec_hp_ind] ){
			//replace h1 with b1
			buffer[pr_hp_ind] = buffer[sec_hp_ind];
			sec_hp_ind += 1;
		}
		else{
			buffer[pr_hp_ind] = buffer[pr_hp_len-1];
			buffer[pr_hp_len-1] = buffer[sec_hp_ind];
			pr_hp_len -= 1;
			sec_hp_ind += 1;
			sec_hp_len += 1;

		}
	  	

		/*if(bo == 1000){
			sprintf(rname, "%s.%03d", input_file, r);
			rptr = fopen(rname, "ab");
			if(!pr_hp_len){
				pr_hp_len = 750;
				r += 1;
			}
			fwrite(bufferout, sizeof(int), bo, rptr);
			writes += bo;
			bo = 0;
			fclose(rptr);
		}

		if(pr_hp_len == 0){
			//write to the run file
			sprintf(rname, "%s.%03d", input_file, r);
			r += 1;
			rptr = fopen(rname, "ab");
			fwrite(bufferout, sizeof(int), bo, rptr);
			writes += bo;
			bo = 0;
			fclose(rptr);
			pr_hp_len = 750;
			sec_hp_len = 250;
		}

		if(sec_hp_ind == 1000){
			elem_read = fread(buffer+750, sizeof(int), 250, iptr);
			sec_hp_ind = 750;
			if(elem_read == 0){
				//write to the output file to finish the run
				//pr_hp_len = 750;
				sprintf(rname, "%s.%03d", input_file, r);
				r += 1;
				rptr = fopen(rname, "ab");
				fwrite(bufferout, sizeof(int), bo, rptr);
				writes += bo;
				bo = 0;
				fclose(rptr);

				//sort the rest of the elements in the input buffer and 
				//add to a run
				//hsort2(buffer, 750);
				pr_hp_len = 750;

				printf("%d\t%d\n", 750, sec_hp_len);

				qsort(buffer, 750, sizeof(int), cmp_qsort);
				sprintf(rname, "%s.%03d", input_file, r);
				r += 1;
				rptr = fopen(rname, "ab");
				fwrite(bufferout, sizeof(int), pr_hp_len, rptr);
				bo = 0;
				writes += bo;
				fclose(rptr);

				break;
			}
		}*/
        //when the buffer gets exhausted, fill it
		relocation_read_buffer(&sec_hp_ind, &elem_read);

        //if the primary heap is exhausted or output is full or input is exhausted
        //write to the output buffer and set the global values accordingly
		relocation_write_exhaust(&pr_hp_len, &bo, &elem_read, &r, rptr);

        if(elem_read == 0){
            sprintf(rname, "%s.%03d", input_file, r);
            rptr = fopen(rname, "a+b");
            qsort(buffer, pr_hp_len, sizeof(int), cmp_qsort);
            fwrite(buffer, sizeof(int), pr_hp_len, rptr);
            fclose(rptr);
            r+=1;
            break;
        }
        //relocation_last(&elem_read, &r, rptr, &pr_hp_len)
	}

	//printf("%d\n", r);
	disk_merge_sort2(0, 0, r, optr);

	//printf("%d\n", writes);

}


void relocation_read_buffer(int *sec_hp_ind, int *elem_read){
    if(*sec_hp_ind == 1000){
        *sec_hp_ind = 750;
        *elem_read = fread(buffer+750, sizeof(int), 250, iptr);
    }
}

void relocation_write_exhaust(int *pr_hp_len, int *bo, int *elem_read, int *r, FILE* rptr){
    if( *pr_hp_len == 0 || *bo == 1000 ||  *elem_read== 0){

        sprintf(rname, "%s.%03d", input_file, *r);
        rptr = fopen(rname, "a+b");
        fwrite(bufferout, sizeof(int), *bo, rptr);
        *bo = 0;
        fclose(rptr);
        if(*elem_read == 0 || *pr_hp_len == 0){
            *r += 1;
            *pr_hp_len = 750;
        }
    }
}



void relocation_last(int elem_read, int r, FILE* rptr, int pr_hp_len){
    
}

int main(int argc, char *argv[]){

	rprof A[10];

	A[0].val = 3;
	A[0].rid = 3;

	A[1].val = 6;
	A[1].rid = 2;

	A[2].val = 1;
	A[2].rid = 2;

	A[3].val = 4;
	A[3].rid = 3;

	A[4].val = 5;
	A[4].rid = 3;

	A[5].val = 34;
	A[5].rid = 3;

	A[6].val = 4;
	A[6].rid = 2;

	A[7].val = 3;
	A[7].rid = 2;

	A[8].val = 25;
	A[8].rid = 3;

	A[9].val = 1;
	A[9].rid = 3;

	int i;
	hsort(A, 10);
	for(i = 0; i<10; i++){
		//printf("\t%d\t", A[i].val);
	}
	//hsort(A, 10);
	//hsort(A, 10);
	//printf("%d\n", A[0].val);

	int tmp;
	tmp = A[0].val;
	A[0].val = A[9].val;
	A[9].val = tmp;

	//hsort(A, 9);
	//printf("%d\n", A[0].val);

	int b[10] = {3,6,1,4,5,34,4,3,25,1};
	hsort2(b, 10);
	//printf("\n");
	for(i = 0; i<10; i++){
		//printf("\t%d\t", b[i]);
	}
	if(argc == 4){
		//number of arguments should be four
		strcpy(input_file, argv[2]);
		strcpy(output_file, argv[3]);
		//printf("\n%s", input_file);

		initialize();
		
		if( !strcmp("--basic", argv[1]) ){
            struct timeval tm1;
            gettimeofday(&tm1, NULL);

			basic_disk_merge();

            struct timeval tm2;
            gettimeofday(&tm2, NULL);

            long tc1_sec = tm2.tv_sec - tm1.tv_sec;
            long tc1_usec = tm2.tv_usec - tm1.tv_usec;

            if(tc1_usec < 0){
                tc1_usec = 1000000 + tc1_usec;
                tc1_sec = tc1_sec - 1;
            }
            printf("Time: %ld.%06ld\n", tc1_sec, tc1_usec);
		}
		else if( !strcmp("--multistep", argv[1]) ){
			
            struct timeval tm1;
            gettimeofday(&tm1, NULL);

            multistep_disk_merge();

            struct timeval tm2;
            gettimeofday(&tm2, NULL);

            long tc1_sec = tm2.tv_sec - tm1.tv_sec;
            long tc1_usec = tm2.tv_usec - tm1.tv_usec;

            if(tc1_usec < 0){
                tc1_usec = 1000000 + tc1_usec;
                tc1_sec = tc1_sec - 1;
            }
            printf("Time: %ld.%06ld\n", tc1_sec, tc1_usec);
		}
		else if( !strcmp("--replacement", argv[1]) ){
            struct timeval tm1;
            gettimeofday(&tm1, NULL);

            replacement_disk_merge();

            struct timeval tm2;
            gettimeofday(&tm2, NULL);

            long tc1_sec = tm2.tv_sec - tm1.tv_sec;
            long tc1_usec = tm2.tv_usec - tm1.tv_usec;

            if(tc1_usec < 0){
                tc1_usec = 1000000 + tc1_usec;
                tc1_sec = tc1_sec - 1;
            }
            printf("Time: %ld.%06ld\n", tc1_sec, tc1_usec);
		}
	}
	return 0;
}
