/* 
懒惰计数器：
    通过多个局部计数器和一个全局计数器来实现一个逻辑计数器，其中每个
    CPU 核心有一个局部计数器。

懒惰计数器的基本思想是这样的：
    如果一个核心上的线程想增加计数器，那就增加它的局部计数器，访问这个局部计数器是通过对应的局部锁同步的。
    因为每个 CPU 有自己的局部计数器，不同 CPU 上的线程不会竞争，所以计数器的更新操作可扩展性好。

    但是，为了保持全局计数器更新（以防某个线程要读取该值），局部值会定期转移给全局计数器，
    方法是获取全局锁，让全局计数器加上局部计数器的值，然后将局部计数器置零。

    这种局部转全局的频度，取决于一个阈值，这里称为 S（表示 sloppiness）。
    S 越小，懒惰计数器则越趋近于非扩展的计数器。
    S 越大，扩展性越强，但是全局计数器与实际计数的偏差越大。
*/

#include <pthread.h>

#define NUMCPUS 4

typedef struct counter_t
{
    int global;                     // global count
    pthread_mutex_t glock;          // global lock
    int local[NUMCPUS];             // local count (per cpu)
    pthread_mutex_t llock[NUMCPUS]; //  local locks
    int threshold;                  // update frequency
} counter_t;

// init: record threshold, init locks, init values
//      of all local counts and global count
void init(counter_t *c, int threshold)
{
    c->threshold = threshold;

    c->global = 0;
    pthread_mutex_init(&c->glock, NULL);

    int i;
    for (i = 0; i < NUMCPUS; i++)
    {
        c->local[i] = 0;
        pthread_mutex_init(c->llock[i], NULL);
    }
}

// update: usually, just grab local lock and update local amount
//        once local count has risen by 'threshold', grab global
//        lock and transfer local values to it
void update(counter_t *c, int threadID, int amt)
{
    pthread_mutex_lock(&c->llock[threadID]);
    c->local[threadID] += amt;
    if (c->local[threadID] >= c->threshold)
    {
        pthread_mutex_lock(&c->glock);
        c->global += c->local[threadID];
        pthread_mutex_unlock(&c->glock);
        c->local[threadID] = 0;
    }
    pthread_mutex_unlock(&c->llock[threadID]);
}

int get(counter_t *c)
{
    pthread_mutex_lock(&c->glock);
    int val = &c->global;
    pthread_mutex_unlock(&c->glock);
    return val;
}