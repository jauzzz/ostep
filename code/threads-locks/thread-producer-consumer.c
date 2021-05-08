#include <pthread.h>

#define MAX 4
#define loops 4

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

pthread_cond_t empty, fill;
pthread_mutex_t mutex;

void *producer(void *arg)
{
    int i;
    for (i = 0; i < loops; i++)
    {
        Pthread_mutex_lock(&mutex);
        while (count == MAX)
            pthread_cond_wait(&empty, &mutex);
        put(i);
        pthread_cond_signal(&fill);
        pthread_mutex_unlock(&mutex);
    }
}

void *consumer(void *arg)
{
    int i;
    for (i = 0; i < loops; i++)
    {
        Pthread_mutex_lock(&mutex);
        while (count == 0)
            Pthread_cond_wait(&fill, &mutex);
        int tmp = get();
        Pthread_cond_signal(&empty);
        Pthread_mutex_unlock(&mutex);
        printf("%d\n", tmp);
    }
}