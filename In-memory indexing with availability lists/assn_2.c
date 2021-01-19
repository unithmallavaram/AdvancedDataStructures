#include<stdio.h>
#include<string.h>

//STRUCTURES
typedef struct{
	int siz;
	long offset;
}avail_S;

typedef struct{
	int key;
	long offset;
}index_S;

//GLOBAL SECTION
int availlistorder;
char instruction[1000];
char dbfile[100];
char indexfile[100];
char availfile[100];
FILE *dptr;
FILE *iptr;
FILE *aptr;
avail_S availlist[10000];
long avail_len;
index_S indices[10000];
long index_len;
char *primkey, *tuple;
avail_S tmp_a;
index_S tmp_i;


//FUNCTION DECLARATIONS
void add();
void find();
void delete();
void end();
int getnextavailhole(int size_rec);
int binarysearch(index_S arr[], int start, int end, int key);
int cmp_bestfit(const void *a, const void *b);
int cmp_worstfit(const void *a, const void *b);

int cmp_bestfit(const void *a, const void *b){
	int sa = (*(avail_S*)a).siz;
	int sb = (*(avail_S*)b).siz;

	if(sa < sb)
		return -1;
	else if(sa > sb)
		return 1;
	else{
		int oa = (*(avail_S*)a).offset;
		int ob = (*(avail_S*)b).offset;
		if(oa<ob)
			return -1;
		else if(oa > ob)
			return 1;
		else
			return 0;
	}
}

int cmp_worstfit(const void *a, const void *b){
	int sa = (*(avail_S*)a).siz;
	int sb = (*(avail_S*)b).siz;

	if(sa < sb)
		return 1;
	else if(sa > sb)
		return -1;
	else{
		int oa = (*(avail_S*)a).offset;
		int ob = (*(avail_S*)b).offset;
		if(oa<ob)
			return -1;
		else if(oa > ob)
			return 1;
		else
			return 0;
	}
}

int binarysearch(index_S arr[], int start, int end, int key){
	//int i = start;
	//printf("%d\n", i);
	//for(i=start; i<=end; i++){
	//	printf("%d ", arr[i]);
	//}
	//printf("\n");
	//printf("%d\n", i);
	if(start == end){
		if(arr[start].key == key)
			return start;
	}
	if(start < end){
		int mid = start + (end-start+1)/2;
		//printf("%d\n", mid);
		if (arr[mid].key == key){
			return mid;
		}
		else if(key < arr[mid].key){
			return binarysearch(arr, start, mid-1, key);
		}
		else {
			return binarysearch(arr, mid+1, end, key);
		}
	}
	return -1;
}

int getnextavailhole(int size_rec){
	int i, hole = -1;
	if(avail_len == 0){
		return hole;		//no holes available, insert record at the end of the file
	}
	else{
		//first fit
		if(availlistorder == 1){

			//search for the first hole that fits the record
			for(i = 0; i<avail_len; i++){
				if(availlist[i].siz >= size_rec){
					hole = i;
					break;
				}
			}

			//if there is no hole of appropriate size, insert record at the end of the file
			if(hole == -1){
				return hole;
			}
			else{
				//store the hole value in a global variable.
				tmp_a.siz = availlist[hole].siz;
				tmp_a.offset = availlist[hole].offset;

				//filling the gap created in the availability list by using up this hole
				for(i = hole; i<avail_len; i++){
					if(i == avail_len -1 ){
						availlist[i].siz = 0;
						availlist[i].offset = 0;
					}
					else{
						availlist[i].siz = availlist[i+1].siz;
						availlist[i].offset = availlist[i+1].offset;
					}

				}
				avail_len -= 1;

				//adding the remaining piece of the hole to the end of the avail list
				if(size_rec < tmp_a.siz){
					//add a new hole to the end of the availability list
					availlist[avail_len].siz = tmp_a.siz - size_rec;
					availlist[avail_len].offset = tmp_a.offset + size_rec;
					avail_len += 1;
				}

				return hole;
			}

		}
		//best fit
		if(availlistorder == 2){
			//sort the list first
			//qsort(availlist, avail_len, sizeof(avail_S), cmp_bestfit);

			//search for the first hole that fits the record
			for(i = 0; i<avail_len; i++){
				if(availlist[i].siz >= size_rec){
					hole = i;
					break;
				}
			}

			//if there is no hole of appropriate size, insert record at the end of the file
			if(hole == -1){
				return hole;
			}
			else{
				//store the hole value in a global variable.
				tmp_a.siz = availlist[hole].siz;
				tmp_a.offset = availlist[hole].offset;

				//filling the gap created in the availability list by using up this hole
				if(size_rec < tmp_a.siz){
					//replace the used hole by the resulting hole
					availlist[hole].siz = tmp_a.siz - size_rec;
					availlist[hole].offset = tmp_a.offset + size_rec;

					//sort this array
					qsort(availlist, avail_len, sizeof(avail_S), cmp_bestfit);
				}
				else{	//only enters if the sizes are equal
					//fill up the gap created by the used up hole 
					for(i = hole; i<avail_len; i++){
						if(i == avail_len -1 ){
							availlist[i].siz = 0;
							availlist[i].offset = 0;
						}
						else{
							availlist[i].siz = availlist[i+1].siz;
							availlist[i].offset = availlist[i+1].offset;
						}
					}
					avail_len -= 1;

				}

				return hole;

			}
		}
		//worst fit
		if(availlistorder == 3){

			//sort the list first
			//qsort(availlist, avail_len, sizeof(avail_S), cmp_worstfit);

			//search for the first hole that fits the record
			for(i = 0; i<avail_len; i++){
				if(availlist[i].siz >= size_rec){
					hole = i;
					break;
				}
			}

			//if there is no hole of appropriate size, insert record at the end of the file
			if(hole == -1){
				return hole;
			}
			else{
				//store the hole value in a global variable.
				tmp_a.siz = availlist[hole].siz;
				tmp_a.offset = availlist[hole].offset;

				//filling the gap created in the availability list by using up this hole
				if(size_rec < tmp_a.siz){
					//replace the used hole by the resulting hole
					availlist[hole].siz = tmp_a.siz - size_rec;
					availlist[hole].offset = tmp_a.offset + size_rec;

					//sort this array
					qsort(availlist, avail_len, sizeof(avail_S), cmp_worstfit);
				}
				else{	//only enters if the sizes are equal
					//fill up the gap created by the used up hole 
					for(i = hole; i<avail_len; i++){
						if(i == avail_len -1 ){
							availlist[i].siz = 0;
							availlist[i].offset = 0;
						}
						else{
							availlist[i].siz = availlist[i+1].siz;
							availlist[i].offset = availlist[i+1].offset;
						}
					}
					avail_len -= 1;

				}

				return hole;

			}
			
		}
	}
}

void add(){
	int i;
	//total record size
	int size_rec = sizeof(int)+sizeof(char)*strlen(tuple);
	
	//get the next available hole
	int hole = getnextavailhole(size_rec);

	//insert the record into the file and the corresponding key into the index file
	//FILE *dptr;

	//if((dptr = fopen(dbfile, "r+b")) == NULL){
	//		dptr = fopen(dbfile, "w+b");
	//}

	int k = atoi(primkey);
	int l = (int)strlen(tuple);
	int s = (sizeof(char))*l;
	//printf("%d\n", s);
	//printf("%s: %s\n", primkey, tuple);
	//printf("%d %d %d\n", sizeof(primkey), sizeof(tuple), sizeof(primkey)+sizeof(tuple));

	if(hole == -1){
		//add the record at the end of the file
		fseek(dptr, 0, SEEK_SET);
		fseek(dptr, 0, SEEK_END);
		long off = ftell(dptr);
		//fseek(dptr, off, SEEK_SET);
		fwrite(&s, sizeof(int), 1, dptr);
		fwrite(tuple, sizeof(char), l, dptr);

		//add the record to the index file and adjust it's position
		indices[index_len].key = k;
		indices[index_len].offset = off;
		index_len += 1;

		for(i = index_len-1; i>=0; i--){
			if(indices[i].key < indices[i-1].key){
				//swap
				tmp_i.key = indices[i-1].key;
				tmp_i.offset = indices[i-1].offset;

				indices[i-1].key = indices[i].key;
				indices[i-1].offset = indices[i].offset;

				indices[i].key = tmp_i.key;
				indices[i].offset = tmp_i.offset;
			}
			else{
				break;
			}
		}
		
	}
	else{
		fseek(dptr, tmp_a.offset, SEEK_SET);
		fwrite(&s, sizeof(int), 1, dptr);
		fwrite(tuple, sizeof(char), l, dptr);

		//add the record to the index file
		indices[index_len].key = k;
		indices[index_len].offset = tmp_a.offset;
		index_len += 1;

		for(i = index_len-1; i>=0; i--){
			if(indices[i].key < indices[i-1].key){
				//swap
				tmp_i.key = indices[i-1].key;
				tmp_i.offset = indices[i-1].offset;

				indices[i-1].key = indices[i].key;
				indices[i-1].offset = indices[i].offset;

				indices[i].key = tmp_i.key;
				indices[i].offset = tmp_i.offset;
			}
			else{
				break;
			}
		}
	}

}


void find(){
	int k = atoi(primkey);
	//printf("%d\n",k);
	int x = binarysearch(indices, 0, index_len-1, k);
	//printf("%d\n",x);
	if(x != -1){
		
		//FILE *dptr;
		int k;

		//if((dptr = fopen(dbfile, "r+b")) == NULL){
		//		dptr = fopen(dbfile, "w+b");
		//}

		fseek(dptr, indices[x].offset, SEEK_SET);
		fread(&k, sizeof(int), 1, dptr);
		char *buf = malloc(k);
		fread(buf, k, 1, dptr);
		buf[k] = '\0';
		printf("%s\n", buf);
	}
	else{
		printf("No record with SID=%d exists\n", k);
	}
}

void delete(){
	int size, i;
	int k = atoi(primkey);
	int x = binarysearch(indices, 0, index_len-1, k);
	if(x != -1){
		//read the size of the record and add a hole to the end of the availlist
		fseek(dptr, indices[x].offset, SEEK_SET);
		fread(&size, sizeof(int), 1, dptr);

		//size of the hole to be added
		size = size + sizeof(int);

		//add hole to the availability list based on the availlistorder variable

		//first fit
		if(availlistorder == 1){
			//add the hole to the end of the list
			availlist[avail_len].siz = size;
			availlist[avail_len].offset = indices[x].offset;
			avail_len += 1;
			
		}
		//bestfit
		else if(availlistorder == 2){
			//add the hole at the appropriate position and adjust the list.
			if(avail_len == 0){
				availlist[avail_len].siz = size;
				availlist[avail_len].offset = indices[x].offset;
				avail_len += 1;
			}
			else{
				//add the hole to the end of the list and sort the list
				availlist[avail_len].siz = size;
				availlist[avail_len].offset = indices[x].offset;
				avail_len += 1;

				qsort(availlist, avail_len, sizeof(avail_S), cmp_bestfit);
			}
		
		}
		//worst fit
		else if(availlistorder == 3){
			//add the hole at the appropriate position and adjust the list.
			if(avail_len == 0){
				availlist[avail_len].siz = size;
				availlist[avail_len].offset = indices[x].offset;
				avail_len += 1;
			}
			else{
				//add the hole to the end of the list and sort the list
				availlist[avail_len].siz = size;
				availlist[avail_len].offset = indices[x].offset;
				avail_len += 1;

				qsort(availlist, avail_len, sizeof(avail_S), cmp_worstfit);
			}
		}

		//remove the record from the index file and adjust it
		for(i = x; i<index_len; i++){
			if(i == index_len-1){
				indices[i+1].key = 0;
				indices[i+1].offset = 0;
				index_len -= 1;
			}else{
				indices[i].key = indices[i+1].key;
				indices[i].offset = indices[i+1].offset;
			}
		}

	}
	else{
		printf("No record with SID=%d exists\n", k);
	}
}

void end(){
	int i,j;

	//close the db file
	fclose(dptr);

	//close the index file
	
	fclose(iptr);
	iptr = fopen(indexfile, "wb+");
	
	printf("Index:\n");
	for(i = 0; i<index_len; i++){
		fwrite(&indices[i], sizeof(index_S), 1, iptr);
		printf( "key=%d: offset=%ld\n", indices[i].key, indices[i].offset );
	}

	//close the availability list file
	int hole_siz = 0;

	fclose(aptr);
	aptr = fopen(availfile, "wb+");

	printf("Availability:\n");
	for(j = 0; j<avail_len; j++){
		fwrite(&availlist[j], sizeof(avail_S), 1, aptr);
		printf( "size=%d: offset=%ld\n", availlist[j].siz, availlist[j].offset );
		hole_siz += availlist[j].siz;
	}

	printf( "Number of holes: %d\n", avail_len );
	printf( "Hole space: %d\n", hole_siz );

}

int main(int argc, char *argv[]){
	int i = 0;
	//strcpy(avail_list_order, argv[1]);
	
	if (argc == 3){

		strcpy(dbfile, argv[2]);

		strcat(indexfile, "index.db");
		strcat(availfile, "availlist.db");

		//open the db file
		if((dptr = fopen(dbfile, "r+b")) == NULL){
			dptr = fopen(dbfile, "w+b");
		}

		//load the index file into memory
		if((iptr = fopen(indexfile, "r+b")) == NULL){
			iptr = fopen(indexfile, "w+b");
		}
		fseek(iptr, 0, SEEK_END);
		index_len = ftell(iptr)/sizeof(index_S);

		fseek(iptr, 0, SEEK_SET);
		index_S ind;
		while(fread(&ind, sizeof(index_S), 1, iptr)){
			indices[i].key = ind.key;
			indices[i].offset = ind.offset;
			i++;
		}

		//load the availability list into memory
		if((aptr = fopen(availfile, "r+b")) == NULL){
			aptr = fopen(availfile, "w+b");
		}

		fseek(aptr, 0, SEEK_END);
		avail_len = ftell(aptr)/sizeof(avail_S);

		fseek(aptr, 0, SEEK_SET);
		avail_S avl;
		i = 0;
		while(fread(&avl, sizeof(avail_S), 1, aptr)){
			availlist[i].siz = avl.siz;
			availlist[i].offset = avl.offset;
			i++;
		}


		if( !strcmp(argv[1], "--first-fit" ) ){
			availlistorder = 1;
		}
		if( !strcmp(argv[1], "--best-fit" ) ){
			availlistorder = 2;
			qsort(availlist, avail_len, sizeof(avail_S), cmp_bestfit);
		}
		if( !strcmp(argv[1], "--worst-fit" ) ){
			availlistorder = 3;
			qsort(availlist, avail_len, sizeof(avail_S), cmp_worstfit);
		}

		//waiting for the commands and executing them in order
		while(1){
			fgets(instruction, sizeof(instruction), stdin);

			//printf("%s\n", instruction);
			char inst[10];
			//int i;
			//char delim[] = {" "};
			char *tok = strtok(instruction, " ");
			
			if(!strcmp(tok, "add")){
				primkey = strtok(NULL, " ");
				tuple = strtok(NULL, " ");
				primkey[strlen(primkey)] = '\0';
				//printf("%s\n", primkey);
				tuple[strlen(tuple)-1] = '\0';
				//printf("%s\n", tuple);
				int k = atoi(primkey);
				int x = binarysearch(indices, 0, index_len-1, k);
				if(x == -1){
					add();
				}
				else{
					printf("Record with SID=%d exists\n", k);
				}
			}
			if(!strcmp(tok, "find")){	
				primkey = strtok(NULL, " ");
				find();			
			}
			if(!strcmp(tok, "del")){
				primkey = strtok(NULL, " ");
				delete();			
			}
			if(!strcmp(tok, "end\n")){
				end();		
				break;	
			}
			if(!strcmp(tok, "testb")){
				primkey = strtok(NULL, " ");
				int k = atoi(primkey);
				index_S i[10];
				int j;
				for(j = 0; j<10; j++){
					i[j].key = j;
					i[j].offset = j;
				}

				int result = binarysearch(i, 0, 9, k);
				printf("%d\n", result);

			}
			if(!strcmp(tok, "testq\n")){
				avail_S a[8];
				int i;

				a[0].siz = 6;
				a[0].offset = 20;

				a[1].siz = 7;
				a[1].offset = 20;

				a[2].siz = 33;
				a[2].offset = 10;

				a[3].siz = 45;
				a[3].offset = 40;

				a[4].siz = 27;
				a[4].offset = 12;

				a[5].siz = 89;
				a[5].offset = 12;

				a[6].siz = 41;
				a[6].offset = 12;

				a[7].siz = 6;
				a[7].offset = 12;

				for(i = 0; i<8; i++){
					printf("%d,%d\t", a[i].siz, a[i].offset);
				}
				printf("\n");
				qsort(a, 8, sizeof(avail_S), cmp_worstfit);

				for(i = 0; i<8; i++){
					printf("%d,%d\t", a[i].siz, a[i].offset);
				}
				printf("\n");
				qsort(a, 8, sizeof(avail_S), cmp_bestfit);
				for(i = 0; i<8; i++){
					printf("%d,%d\t", a[i].siz, a[i].offset);
				}

			}
		}
	}
	
	return 0;	
	
}