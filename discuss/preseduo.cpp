/*
 * @Author: gunjianpan
 * @Date:   2019-04-21 16:36:14
 * @Last Modified by:   gunjianpan
 * @Last Modified time: 2019-04-22 00:38:00
 */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#define THREADNUM 100
#define LOCKNUM 5
#define HUNGERNUM 5
int upNum = 0, downNum = 0;
int totalUpNum = 0, totalDownNum = 0;
sem_t *sl[LOCKNUM], *mutex, *upBegin, *downBegin, *hungerUpper, *hungerDown;
pthread_t tidList[THREADNUM];

void *Up() {
  sem_wait(upBegin);            // 抢占大坝外队首
  if (!upNum) sem_wait(mutex);  // 进入大坝临界区
  while (downNum || different > HUNGERNUM) {
    sem_post(mutex);  // 如果对面有来船，或者进入饥饿条件
    sem_wait(mutex);  // 则主动让出大坝临界区
  }
  ++totalUpNum, ++upNum;  // 计数

  for (int i = 0; i < LOCKNUM; ++i) {
    sem_wait(sl[i]);  // 依次获取第i级船闸临界区
    // do some work //
    sem_post(sl[i]);            // 释放第i级船闸临界区
    if (!i) sem_post(upBegin);  // 释放未进入船舶队首临界区
  }
  --upNum;
  if (!upNum) sem_post(mutex);  // 释放大坝临界区
  return NULL;
}

void *Down(void *a) {
  sem_wait(downBegin);            // 抢占大坝外队首
  if (!downNum) sem_wait(mutex);  // 进入大坝临界区
  while (upNum || different > HUNGERNUM) {
    sem_post(mutex);  // 如果对面有来船，或者进入饥饿条件
    sem_wait(mutex);  // 则主动让出大坝临界区
  }
  ++totalDownNum, ++downNum;  // 计数

  for (int i = LOCKNUM - 1; i >= 0; --i) {
    sem_wait(sl[i]);  // 依次获取第i级船闸临界区
    // do some work //
    sem_post(sl[i]);                            // 释放第i级船闸临界区
    if (i == LOCKNUM - 1) sem_post(downBegin);  // 释放未进入船舶队首临界区
  }
  --downNum;
  if (!downNum) sem_post(mutex);  // 释放大坝临界区
  return NULL;
}

void thread_one_create(pthread_t nowTid, int types) {
  if (types)
    pthread_create(&nowTid, NULL, Up, NULL);
  else
    pthread_create(&nowTid, NULL, Down, NULL);
}

void thread_one_join(pthread_t nowTid) { pthread_join(nowTid, NULL); }

int main(int argc, char *argv[]) {
  printf(
      "\033[01;34m Write by Jiang Huiqiang 1801210840 in 2019-04-21 \033[0m\n");
  sem_unlink("sem");
  for (int i = 0; i < LOCKNUM; ++i) sl[i] = sem_open("sem", O_CREAT, 0, 1);
  upBegin = sem_open("sem", O_CREAT, 0, 1);
  downBegin = sem_open("sem", O_CREAT, 0, 1);
  mutex = sem_open("sem", O_CREAT, 0, 1);
  hungerDown = sem_open("sem", O_CREAT, 0, HUNGERNUM);
  hungerUpper = sem_open("sem", O_CREAT, 0, HUNGERNUM);

  for (int i = 0; i < THREADNUM; ++i) thread_one_create(tidList[i], i % 2);
  for (int i = 0; i < THREADNUM; ++i) thread_one_join(tidList[i]);

  for (int i = 0; i < LOCKNUM; ++i) sem_destroy(sl[i]);

  pthread_exit(NULL);
}
