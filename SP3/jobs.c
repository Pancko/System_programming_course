// Andreev S.V. 37 group
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

typedef struct job
{
    char request[256];
    struct job *next;
} job;

typedef struct queue_job
{
    job *head;
    job *tail;
} queue_job;

queue_job q = {NULL}; 
FILE* result_file;
sem_t semaphore;
pthread_mutex_t mutex;

void enqueue_job(queue_job *queue, job *new)
{
	pthread_mutex_lock(&mutex);
    job *new_job = malloc(sizeof(job));
    strcpy(new_job->request, new->request);
    new_job->next = NULL;

    if (queue->head == NULL)
    {
        queue->head = new_job;
        queue->tail = new_job;
    }
    else
    {
        queue->tail->next = new_job;
        queue->tail = new_job;
    }
    sem_post(&semaphore);
	pthread_mutex_unlock(&mutex);
}

job *dequeue_job(queue_job *queue)
{
	
    if (queue->head == NULL)
        return NULL;

    job *head = malloc(sizeof(job));
    strcpy(head->request, queue->head->request);
    head->next = queue->head->next;

    queue->head->next = NULL;
    free(queue->head);

    job *new_head = head->next;
    queue->head = new_head;

    return head;
}

void ProcessJob()
{	
	pthread_mutex_lock(&mutex);
	job* j = dequeue_job(&q);
	if (j != NULL)
		fputs(j->request, result_file);
    sem_post(&semaphore);
	pthread_mutex_unlock(&mutex);
}
void ThreadsQueue()
{
	sem_wait(&semaphore);
	while(q.head != NULL)
	{
		ProcessJob();
	}
}
int main(int argc, char const *argv[])
{	
	if (argc != 2)
	{
		printf("Need a number of threads to start\n");
		return -1;
	}
	int N = atoi(argv[1]);
	if (N <= 0)
	{
		printf("Invalid number!\n");
		exit(-1);
	}

	pthread_t* threads = malloc(N * sizeof(pthread_t));
	sem_init(&semaphore, 0, 0);
	pthread_mutex_init(&mutex, NULL);

	result_file = fopen("result.txt", "w");
	if (result_file == NULL)
	{
		printf("Can't open result file\n");
		exit(-1);
	}

	char buffer[256];
	job first;
	printf("To stop queuing data write \"stop\"\n");
	fgets(buffer, 256, stdin);
	while (strcmp(buffer, "stop\n") != 0)
	{
		strcpy(first.request, buffer);
		enqueue_job(&q, &first);
		fgets(buffer, 256, stdin);
	}
	for (int i = 0; i < N; ++i)
	{		
		pthread_create(&threads[i], NULL, (void *)ThreadsQueue, NULL);
	}

	for (int i = 0; i < N; ++i)
	{		
		pthread_join(threads[i], NULL);
	}
	fclose(result_file);
	sem_destroy(&semaphore);
	pthread_mutex_destroy(&mutex);
	return 0;
}