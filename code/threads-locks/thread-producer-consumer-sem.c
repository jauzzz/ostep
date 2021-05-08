#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

#define MAX 4
#define loops 8

int buffer[MAX];
int fill = 0;
int use = 0;
int count = 0;

void put(int value)
{
    buffer[fill] = value;
    fill = (fill + 1) % MAX;
    count++;
}

int get()
{
    int tmp = buffer[use];
    use = (use + 1) % MAX;
    count--;
    return tmp;
}

// 信号量版本的 "生产者-消费者" 有界缓冲区问题
sem_t empty;
sem_t full;
sem_t mutex;

void *producer()
{
    int i;
    for (i = 0; i < loops; i++)
    {
        sem_wait(&empty);
        sem_wait(&mutex);
        put(i);
        sem_post(&mutex);
        sem_post(&full);
    }
}

void *consumer()
{
    int i, tmp = 0;
    while (tmp != -1)
    {
        sem_wait(&full);
        sem_wait(&mutex);
        tmp = get();
        sem_post(&mutex);
        sem_post(&empty);
        printf("%d\n", tmp);
    }
}

int main(int argc, char *argv[])
{
    // ...
    sem_init(&empty, 0, MAX);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);
    // ...
}