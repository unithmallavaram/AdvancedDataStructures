#include<stdio.h>
#include<string.h>
#include<sys/time.h>


void mem_lin();
void mem_bin();
void disk_lin();
void disk_bin();
int binary_search(int arr[], int start, int end, int key);
int binary_search_disk(FILE *kptr, int start, int end, int key);

char key[100];
char seek[100];


int binary_search(int arr[], int start, int end, int key){
	//int i = start;
	//printf("%d\n", i);
	//for(i=start; i<=end; i++){
	//	printf("%d ", arr[i]);
	//}
	//printf("\n");
	//printf("%d\n", i);
	if(start == end){
		if(arr[start] == key)
			return 1;
	}
	if(start < end){
		int mid = start + (end-start+1)/2;
		//printf("%d\n", mid);
		if (arr[mid] == key){
			return 1;
		}
		else if(key < arr[mid]){
			return binary_search(arr, start, mid-1, key);
		}
		else {
			return binary_search(arr, mid+1, end, key);
		}
	}
	return 0;
}



int binary_search_disk(FILE *kptr, int start, int end, int key){
	//printf("s%d e%d\n",start, end );
	if(start <= end){
		int mid = start + (end-start+1)/2 ;
		int k;
		fseek(kptr, mid*sizeof(int), SEEK_SET);
		fread(&k, sizeof(int), 1, kptr);
		//printf("m%d k%d\n", mid, k);
		if(k == key){
			return 1;
		}
		else if(key <= k){
			return binary_search_disk(kptr, start, mid-1, key);
		}
		else{
			return binary_search_disk(kptr, mid+1, end, key);
		}
	}
	return 0;
}

void mem_lin(){
	//printf("key is %s", key[]):
	FILE *sptr;
	int s;
	sptr = fopen(seek, "rb");
	//fread(&k1, sizeof(int), 1, inp);
	//printf("%d\n", k1);

	//Finding number of integers in the file
	fseek(sptr, 0, SEEK_END);
	long seek_len = ftell(sptr)/sizeof(int);

	//Moving to the top of the file and storing integers in S
	fseek(sptr, 0, SEEK_SET);
	int S[seek_len];
	int i = 0;
	while(fread(&s, sizeof(int), 1, sptr)){
		S[i] = s;
		i++;
	}
	//for(i = 0; i<seek_len; i++){
	//	printf("%d\n", S[7777]);
	//}

	//Copying key values to K
	FILE *kptr;
	int k;

	struct timeval tm1;
	gettimeofday(&tm1, NULL);
	//printf("TIme: %ld.%06ld\n", tm1.tv_sec, tm1.tv_usec);

	kptr = fopen(key, "rb");

	fseek(kptr, 0, SEEK_END);
	long key_len = ftell(kptr)/sizeof(int);

	fseek(kptr, 0, SEEK_SET);
	int K[key_len];
	i = 0;
	while(fread(&k, sizeof(int), 1, kptr)){
		K[i] = k;
		i++;
	}
	struct timeval tm2;
	gettimeofday(&tm2, NULL);
	//printf("TIme: %ld.%06ld\n", tm2.tv_sec, tm2.tv_usec);

	//hit
	int hit[seek_len];

	//Sequential search
	struct timeval tm3;
	gettimeofday(&tm3, NULL);
	//printf("TIme: %ld.%06ld\n", tm3.tv_sec, tm3.tv_usec);
	int j;
	for(i = 0; i<seek_len; i++){
		hit[i] = 0;
		for(j = 0; j<key_len; j++){
			if(S[i] == K[j]){
				hit[i] = 1;
				break;
			}
		}
	}
	struct timeval tm4;
	gettimeofday(&tm4, NULL);
	//printf("TIme: %ld.%06ld\n", tm4.tv_sec, tm4.tv_usec);

	long tc1_sec = tm2.tv_sec - tm1.tv_sec;
	long tc1_usec = tm2.tv_usec - tm1.tv_usec;

	if(tc1_usec < 0){
		tc1_usec = 1000000 + tc1_usec;
		tc1_sec = tc1_sec - 1;
	}

	long tc2_sec = tm4.tv_sec - tm3.tv_sec;
	long tc2_usec = tm4.tv_usec - tm3.tv_usec;

	if(tc2_usec < 0){
		tc2_usec = 1000000 + tc2_usec;
		tc2_sec = tc2_sec - 1;
	}

	long tc_sec = tc1_sec + tc2_sec;
	long tc_usec = tc1_usec + tc2_usec;

	if(tc_usec > 1000000){
		tc_usec = tc_usec - 1000000;
		tc_sec = tc_sec + 1;
	}

	//Printing the output
	for(i = 0; i<seek_len; i++){
		if(hit[i] == 1)
			printf("%12d: Yes\n", S[i]);
		else
			printf("%12d: No\n", S[i]);
	}

	//printf("Time: %ld.%06ld\n", tc1_sec, tc1_usec);
	//printf("Time: %ld.%06ld\n", tc2_sec, tc2_usec);
	printf("Time: %ld.%06ld\n", tc_sec, tc_usec);

}

void mem_bin(){
    FILE *sptr;
    int s;
    sptr = fopen(seek, "rb");
    //fread(&k1, sizeof(int), 1, inp);
    //printf("%d\n", k1);

    //Finding number of integers in the file
    fseek(sptr, 0, SEEK_END);
    long seek_len = ftell(sptr)/sizeof(int);

    //Moving to the top of the file and storing integers in S
    fseek(sptr, 0, SEEK_SET);
    int S[seek_len];
    int i = 0;
    while(fread(&s, sizeof(int), 1, sptr)){
            S[i] = s;
            i++;
    }
    //for(i = 0; i<seek_len; i++){
            //printf("%d\n", S[7777]);
    //}

    //Copying key values to K
	FILE *kptr;
	int k;

	struct timeval tm1;
	gettimeofday(&tm1, NULL);
	//printf("TIme: %ld.%06ld\n", tm1.tv_sec, tm1.tv_usec);

	kptr = fopen(key, "rb");

	fseek(kptr, 0, SEEK_END);
	long key_len = ftell(kptr)/sizeof(int);

	fseek(kptr, 0, SEEK_SET);
	int K[key_len];
	i = 0;
	while(fread(&k, sizeof(int), 1, kptr)){
		K[i] = k;
		i++;
	}
	struct timeval tm2;
	gettimeofday(&tm2, NULL);
	//printf("TIme: %ld.%06ld\n", tm2.tv_sec, tm2.tv_usec);

	//hit
	int hit[seek_len];

	//Binary search
	struct timeval tm3;
	gettimeofday(&tm3, NULL);
	printf("TIme: %ld.%06ld\n", tm3.tv_sec, tm3.tv_usec);
	int j;
	for(i = 0; i<seek_len; i++){
		int result = binary_search(K, 0, key_len-1, S[i]);
		if(result){
			hit[i] = 1;
		}
		else{
			hit[i] = 0;
		}
	}
	struct timeval tm4;
	gettimeofday(&tm4, NULL);
	//printf("TIme: %ld.%06ld\n", tm4.tv_sec, tm4.tv_usec);

	long tc1_sec = tm2.tv_sec - tm1.tv_sec;
	long tc1_usec = tm2.tv_usec - tm1.tv_usec;

	if(tc1_usec < 0){
		tc1_usec = 1000000 + tc1_usec;
		tc1_sec = tc1_sec - 1;
	}

	long tc2_sec = tm4.tv_sec - tm3.tv_sec;
	long tc2_usec = tm4.tv_usec - tm3.tv_usec;

	if(tc2_usec < 0){
		tc2_usec = 1000000 + tc2_usec;
		tc2_sec = tc2_sec - 1;
	}

	long tc_sec = tc1_sec + tc2_sec;
	long tc_usec = tc1_usec + tc2_usec;

	if(tc_usec > 1000000){
		tc_usec = tc_usec - 1000000;
		tc_sec = tc_sec + 1;
	}

	//Printing the output
	for(i = 0; i<seek_len; i++){
		if(hit[i] == 1)
			printf("%12d: Yes\n", S[i]);
		else
			printf("%12d: No\n", S[i]);
	}

	//printf("TIme: %ld.%06ld\n", tc1_sec, tc1_usec);
	//printf("TIme: %ld.%06ld\n", tc2_sec, tc2_usec);
	printf("Time: %ld.%06ld\n", tc_sec, tc_usec);
}

void disk_lin(){
	FILE *sptr;
    int s;
    sptr = fopen(seek, "rb");
    //fread(&k1, sizeof(int), 1, inp);
    //printf("%d\n", k1);

    //Finding number of integers in the file
    fseek(sptr, 0, SEEK_END);
    long seek_len = ftell(sptr)/sizeof(int);

    //Moving to the top of the file and storing integers in S
    fseek(sptr, 0, SEEK_SET);
    int S[seek_len];
    int i = 0;
    while(fread(&s, sizeof(int), 1, sptr)){
            S[i] = s;
            i++;
    }
    //for(i = 0; i<seek_len; i++){
            //printf("%d\n", S[7777]);
    //}

    //hit
	int hit[seek_len];

    FILE *kptr;
    int k;

    struct timeval tm1;
	gettimeofday(&tm1, NULL);
	//printf("TIme: %ld.%06ld\n", tm1.tv_sec, tm1.tv_usec);
    
    for(i=0; i<seek_len; i++){
    	hit[i] = 0;
    	kptr = fopen(key, "rb");
    	while(fread(&k, sizeof(int), 1, kptr)){
    		//printf("%d\n", k);
    		if(S[i] == k){
    			hit[i] = 1;
    			break;
    		}
    	}
    	fclose(kptr);
    }

    struct timeval tm2;
	gettimeofday(&tm2, NULL);
	//printf("TIme: %ld.%06ld\n", tm2.tv_sec, tm2.tv_usec);

	long tc1_sec = tm2.tv_sec - tm1.tv_sec;
	long tc1_usec = tm2.tv_usec - tm1.tv_usec;

	if(tc1_usec < 0){
		tc1_usec = 1000000 + tc1_usec;
		tc1_sec = tc1_sec - 1;
	}	

	//Printing the output
	for(i = 0; i<seek_len; i++){
		if(hit[i] == 1)
			printf("%12d: Yes\n", S[i]);
		else
			printf("%12d: No\n", S[i]);
	}

	printf("Time: %ld.%06ld\n", tc1_sec, tc1_usec);

}

void disk_bin(){
	FILE *sptr;
    int s;
    sptr = fopen(seek, "rb");
    //fread(&k1, sizeof(int), 1, inp);
    //printf("%d\n", k1);

    //Finding number of integers in the file
    fseek(sptr, 0, SEEK_END);
    long seek_len = ftell(sptr)/sizeof(int);

    //Moving to the top of the file and storing integers in S
    fseek(sptr, 0, SEEK_SET);
    int S[seek_len];
    int i = 0;
    while(fread(&s, sizeof(int), 1, sptr)){
            S[i] = s;
            i++;
    }
    //for(i = 0; i<seek_len; i++){
            //printf("%d\n", S[7777]);
    //}

    //hit
	int hit[seek_len];
	FILE *kptr = fopen(key, "rb");

	fseek(kptr, 0, SEEK_END);
	long key_len = ftell(kptr)/sizeof(int);

	fseek(kptr, 0, SEEK_SET);

	struct timeval tm1;
	gettimeofday(&tm1, NULL);
	//printf("TIme: %ld.%06ld\n", tm1.tv_sec, tm1.tv_usec);

	for(i=0; i<seek_len; i++){
		int result = binary_search_disk(kptr, 0, key_len-1, S[i]);
		if(result){
			hit[i] = 1;
		}
		else{
			hit[i] = 0;
		}
	}

	struct timeval tm2;
	gettimeofday(&tm2, NULL);
	//printf("TIme: %ld.%06ld\n", tm2.tv_sec, tm2.tv_usec);

	long tc1_sec = tm2.tv_sec - tm1.tv_sec;
	long tc1_usec = tm2.tv_usec - tm1.tv_usec;

	if(tc1_usec < 0){
		tc1_usec = 1000000 + tc1_usec;
		tc1_sec = tc1_sec - 1;
	}

	//Printing the output
	for(i = 0; i<seek_len; i++){
		if(hit[i] == 1)
			printf("%12d: Yes\n", S[i]);
		else
			printf("%12d: No\n", S[i]);
	}

	printf("Time: %ld.%06ld\n", tc1_sec, tc1_usec);
}

void main(int argc, char *argv[]){
	int i = 0;
	strcpy(key, argv[2]);
	strcpy(seek, argv[3]);
	if (argc == 4){
		if( !strcmp(argv[1], "--mem-lin" ) ){
			//printf("The argument passed is %s\n", argv[1]);
			mem_lin();
		}
		if( !strcmp(argv[1], "--mem-bin" ) ){
			//printf("The argument passed is %s\n", argv[1]);
			mem_bin();
			//int arr[] = {1,2,5,5,6,8,10,12,22,43,54,101,505};
			//int result = binary_search(arr, 0, 12, 101);
			//printf("Result: %d\n", result);
		}
		if( !strcmp(argv[1], "--disk-lin" ) ){
			disk_lin();
		}
		if( !strcmp(argv[1], "--disk-bin" ) ){
			disk_bin();
			/*FILE *kptr;
			kptr = fopen(key, "rb");

			fseek(kptr, 0, SEEK_END);
			long key_len = ftell(kptr)/sizeof(int);
			int K[key_len];
			i = 0;
			int k;
			fseek(kptr, 0, SEEK_SET);
			//K = {0,1,2,3,5,8,11,13}
			int result = binary_search_disk(kptr, 0, key_len-1, 13);
			printf("%d\n", result);*/
		}
	}
	
	
}