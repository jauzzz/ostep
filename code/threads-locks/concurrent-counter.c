// 这种简单加锁的并发计数器，在多CPU情况下，性能不好，也就是说扩展性不好

#include <pthread.h>

typedef struct counter_t
{
    int value;
    pthread_mutex_t lock;
} counter_t;

void init(counter_t *c)
{
    c->value = 0;
    Pthread_mutex_init(&c->lock, NULL);
}

void increment(counter_t *c)
{
    pthread_mutex_lock(&c->lock);
    c->value++;
    pthread_mutex_unlock(&c->lock);
}

void decrement(counter_t *c)
{
    Pthread_mutex_lock(&c->lock);
    c->value--;
    pthread_mutex_unlock(&c->lock);
}

int get(counter_t *c)
{
    Pthread_mutex_lock(&c->lock);
    int rc = c->value;
    Pthread_mutex_unlock(&c->lock);
    return rc;
}
