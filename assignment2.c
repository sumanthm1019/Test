/*
 * assignment2.c
 *
 *  Created on: Feb 3, 2016
 *      Author: sumanth
 */

#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#define N 8
typedef struct structure1 {

	char pool[20][20];

	int count;
} buf1_t;

typedef struct structure2 {

	char spool[20][20];

	int scount[20];

	int length;
} buf3_t;

char lettercounttable[20][20];
int items = 0;
int m_in = 0;
int m_out = 0;
int r_in = 0; // change k to m_in
int r_out = 0; //change l to m_out
int j = 0;
int w_in = 0; // change k to m_in
int w_out = 0; //change l to m_out
int s_in = 0;
int s_out = 0;
buf1_t *buffer1[N];
buf1_t *buffer2[N];
buf1_t *buffer4[N];
buf3_t *buffer3[N];
int mslots = 0;
int rslots = 0;
int wslots = 0;
int sslots = 0;
int lslots = 0;
int l_in = 0;
int l_out = 0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full1 = PTHREAD_COND_INITIALIZER;
int thread1flag = 0;
int thread2flag = 0;
int thread3flag = 0;
int thread4flag = 0;
int thread5flag = 0;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full2 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty3 = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full3 = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex4 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t start = PTHREAD_COND_INITIALIZER;
pthread_cond_t end = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex5 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty5 = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full5 = PTHREAD_COND_INITIALIZER;
void* mapperupdate(void *args);
void* mapper(void *args);
void* reducer(void *args);
void* summarizer(void *args);
void* wordcountwriter(void *args);
void* lettercountwriter(void *args);

int main(int argc, char *argv[]) {
	if (argc != 5) {
		printf(
				"Usage: %s <input file> <number of mapper threads> < number of reducer threads> <number of summarizer threads>\n",
				argv[0]);
		exit(-1);
	}
	int mapper_threads = atoi(argv[2]);
	int reducer_threads = atoi(argv[3]);
	int summarizer_threads = atoi(argv[4]);
	pthread_t mpudate_tid;
	pthread_t mapper_tid[mapper_threads];
	pthread_t reducer_tid[reducer_threads];
	pthread_t wordcountwriter_tid;
	pthread_t summarizer_tid[summarizer_threads];
	pthread_t lettercountwriter_tid;
	int k;

	pthread_create(&mpudate_tid, NULL, mapperupdate, argv[1]);
	for (k = 0; k < mapper_threads; k++) {
		pthread_create(&mapper_tid[k], NULL, mapper, NULL);
	}
	for (k = 0; k < reducer_threads; k++) {
		pthread_create(&reducer_tid[k], NULL, reducer, NULL);
	}
	pthread_create(&wordcountwriter_tid, NULL, wordcountwriter, NULL);
	for (k = 0; k < summarizer_threads; k++) {
		pthread_create(&summarizer_tid[k], NULL, summarizer, NULL);
	}
	
	pthread_create(&lettercountwriter_tid, NULL, lettercountwriter, NULL);
	pthread_join(mpudate_tid, NULL);
	for (k = 0; k < mapper_threads; k++) {
		pthread_join(mapper_tid[k], NULL);
	}
	for (k = 0; k < reducer_threads; k++) {
		pthread_join(reducer_tid[k], NULL);
	}
	for (k = 0; k < summarizer_threads; k++) {
		pthread_join(summarizer_tid[k], NULL);
	}

	pthread_join(wordcountwriter_tid, NULL);
	return 0;
}

void* mapperupdate(void *args) {

	char *fname = (char *) args;
	char item[20], first;
	int k = 0;
	int iteration = 0;
	int i = 0;
	FILE *fptr = fopen(fname, "r");

	first = '\0';

	while (fscanf(fptr, "%s", item)) {

		if (item[0] != first) {

			if (iteration != 0) {
				m_in = (m_in + 1) % N;
				mslots++;

				if (mslots == 1)
					pthread_cond_signal(&not_empty1);
				pthread_mutex_unlock(&mutex1);

			}
			pthread_mutex_lock(&mutex1);
			while (mslots == N) {
				pthread_cond_wait(&not_full1, &mutex1);
			}
			first = item[0];
			free(buffer1[m_in]);
			buffer1[m_in] = malloc(sizeof(buf1_t));
			buffer1[m_in]->count = 0;
			i = 0;

		}
		strcpy(buffer1[m_in]->pool[i], item);
		buffer1[m_in]->count++;
		i++;
		iteration++;

		if (feof(fptr)) {
			mslots++;
			pthread_cond_signal(&not_empty1);
			thread1flag = 1;
			pthread_mutex_unlock(&mutex1);
			pthread_exit(0);
		}

	}
	fclose(fptr);
	pthread_exit(0);
}

void* mapper(void *args) {

	int i = 0;
	char item[20];
	int k = 0;
	j++;
	while (1) {

		pthread_mutex_lock(&mutex1);
		while (mslots == 0) {
			if (thread1flag == 1) {
				pthread_cond_signal(&not_empty1);
				pthread_mutex_unlock(&mutex1);
				pthread_mutex_lock(&mutex2);
				thread2flag = 1;
				pthread_mutex_unlock(&mutex2);
				pthread_cond_broadcast(&not_empty2);
				pthread_exit(0);
			}

			pthread_cond_wait(&not_empty1, &mutex1);
		}
		pthread_mutex_lock(&mutex2);
		while (rslots == N) {
			pthread_cond_wait(&not_full2, &mutex2);
		}
		free(buffer2[r_in]);
		buffer2[r_in] = malloc(sizeof(buf1_t));
		buffer2[r_in]->count = 0;
		for (i = 0; i < buffer1[m_out]->count; i++) {

			sprintf(buffer2[r_in]->pool[i], "(%s, 1)", buffer1[m_out]->pool[i]);
			buffer2[r_in]->count++;

		}
		r_in = (r_in + 1) % N;
		rslots++;
		if (rslots == 1) {
			pthread_cond_broadcast(&not_empty2);
		}
		pthread_mutex_unlock(&mutex2);
		m_out = (m_out + 1) % N;
		mslots--;
		if (mslots == N - 1)
			pthread_cond_broadcast(&not_full1);
		pthread_mutex_unlock(&mutex1);
	}
	pthread_exit(0);
}

void* reducer(void *args) {
	int i = 0;
	int flag = 0;
	int k = 0;
	char *token;

	while (1) {
		pthread_mutex_lock(&mutex2);
		while (rslots == 0) {
			if (thread2flag == 1) {
				pthread_cond_broadcast(&not_empty2);
				pthread_mutex_unlock(&mutex2);
				pthread_mutex_lock(&mutex3);
				thread3flag = 1;
				pthread_mutex_unlock(&mutex3);
				pthread_cond_broadcast(&not_empty3);
				pthread_exit(0);
			}
			pthread_cond_wait(&not_empty2, &mutex2);
		}

		pthread_mutex_lock(&mutex3);
		while (wslots == N) {
			pthread_cond_wait(&not_full3, &mutex3);
		}
		free(buffer3[w_in]);
		buffer3[w_in] = malloc(sizeof(buf3_t));
		buffer3[w_in]->length = 0;

		for (i = 0; i < buffer2[r_out]->count; i++) {
			flag = 0;
			for (k = 0; k < buffer3[w_in]->length; k++) {
				if (strcmp(buffer2[r_out]->pool[i], buffer3[w_in]->spool[k])
						== 0) {
					flag = 1;
					buffer3[w_in]->scount[k]++;
				}
			}

			if (!flag) {
				strcpy(buffer3[w_in]->spool[k], buffer2[r_out]->pool[i]);
				buffer3[w_in]->scount[k] = 1;
				buffer3[w_in]->length++;

			}
		}

		for (i = 0; i < buffer3[w_in]->length; i++) {
			token = strtok(buffer3[w_in]->spool[i], "(,");
			sprintf(buffer3[w_in]->spool[i], "(%s, %d)", token,
					buffer3[w_in]->scount[i]);
		}

		w_in = (w_in + 1) % N;
		wslots++;
		if (wslots == 1)
			pthread_cond_broadcast(&not_empty3);
		pthread_mutex_unlock(&mutex3);
		r_out = (r_out + 1) % N;
		rslots--;

		if (rslots == N - 1) {
			pthread_cond_broadcast(&not_full2);
		}
		pthread_mutex_unlock(&mutex2);

	}

}

void* wordcountwriter(void *args) {

	FILE *out = fopen("wordCount.txt", "w+");
	int i = 0;
	while (1) {

		pthread_mutex_lock(&mutex3);
		while (wslots == 0) {
			if (thread3flag == 1) {
				pthread_mutex_unlock(&mutex3);
				pthread_cond_broadcast(&start);
				pthread_mutex_lock(&mutex4);
				thread4flag = 1;
				pthread_mutex_unlock(&mutex4);
				pthread_cond_broadcast(&start);
				pthread_exit(0);
			}
			pthread_cond_wait(&not_empty3, &mutex3);
		}
		pthread_mutex_lock(&mutex4);
		for (i = 0; i < buffer3[w_out]->length; i++) {
			fprintf(out, "%s\n", buffer3[w_out]->spool[i]);
		}
		items++;
		pthread_cond_broadcast(&start);
		pthread_cond_wait(&end, &mutex4);
		pthread_mutex_unlock(&mutex4);

		w_out = (w_out + 1) % N;
		wslots--;

		if (wslots == N - 1) {
			pthread_cond_broadcast(&not_full3);
		}
		pthread_mutex_unlock(&mutex3);

	}

}

void* summarizer(void *args) {
	int i = 0;
	int totalsum = 0;
	char token;
	while (1) {

		pthread_mutex_lock(&mutex4);

		while (items == 0) {
			
			if (thread4flag == 1){
				pthread_cond_signal(&start);
				pthread_mutex_unlock(&mutex4);
				pthread_mutex_lock(&mutex5);
				thread5flag = 1;
				pthread_mutex_unlock(&mutex5);
				pthread_cond_signal(&not_empty5);
				pthread_exit(0);
			}
				
			pthread_cond_wait(&start, &mutex4);
		}
		totalsum = 0;
		for (i = 0; i < buffer3[s_out]->length; i++) {
			token = buffer3[s_out]->spool[i][strlen(buffer3[s_out]->spool[i]) - 2];
			token = token - '0';
			totalsum += token;
			
		}
		pthread_mutex_lock(&mutex5);
		while(lslots == N){
			
			pthread_cond_wait(&not_full5, &mutex5);
		}
		sprintf(lettercounttable[l_in],"(%c, %d)",buffer3[s_out]->spool[0][1], totalsum);
		l_in = (l_in + 1) % N;
		lslots++;
		if(lslots == 1)
			pthread_cond_signal(&not_empty5);
		pthread_mutex_unlock(&mutex5);
		items--;
		s_out = (s_out + 1) % N;
		pthread_cond_broadcast(&end);
		pthread_mutex_unlock(&mutex4);

	}

}

void *lettercountwriter(void *args)
{
	FILE *lout = fopen("letterCount.txt", "w+");
	
	
	while(1){
		
		
		pthread_mutex_lock(&mutex5);
		while(lslots == 0){
			if(thread5flag == 1){
				pthread_mutex_unlock(&mutex5);
				pthread_exit(0);
			}
			pthread_cond_wait(&not_empty5, &mutex5);
		}
		
		fprintf(lout, "%s\n", lettercounttable[l_out]);
		l_out = (l_out + 1) % N;
		lslots --;
		if(lslots == N - 1)
			pthread_cond_signal(&not_empty5);
		pthread_mutex_unlock(&mutex5);
		
	}
}
