/*
 * @Author: gunjianpan
 * @Date:   2019-04-21 16:36:14
 * @Last Modified by:   gunjianpan
 * @Last Modified time: 2019-04-21 22:46:23
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

void *Up(void *b) {
  // up & down sent one ship once
  sem_wait(upBegin);
  if (!upNum) sem_wait(mutex);  // flow one direction(no care downNum)
  while (downNum || totalUpNum - totalDownNum > HUNGERNUM) {
    sem_post(mutex);
    sem_wait(mutex);
  }
  sem_wait(hungerUpper);
  sem_post(hungerDown);
  ++totalUpNum;
  ++upNum;
  pthread_t tid = pthread_self();
  printf(
      "Tid: %p \033[91mBegin ΔΔΔ>>>>UP>>>>ΔΔΔ\033[0m upNum %d downNum: "
      "%d upTotol %d downTotal %d\n",
      tid, upNum, downNum, totalUpNum, totalDownNum);

  for (int i = 0; i < LOCKNUM; ++i) {
    sem_wait(sl[i]);
    printf("Tid: %p \033[91m%d\033[0m Up\n", tid, i + 1);
    sem_post(sl[i]);
    if (!i) sem_post(upBegin);
  }
  --upNum;

  printf("Tid: %p \033[91mship C.I.F. >>>>Uppppp>>>>\033[0m\n", tid);
  if (!upNum) sem_post(mutex);
  return NULL;
}

void *Down(void *a) {
  // up & down sent one ship once
  sem_wait(downBegin);
  if (!downNum) sem_wait(mutex);  // flow one direction(no care upNum)
  while (upNum || totalDownNum - totalUpNum > HUNGERNUM) {
    sem_post(mutex);
    sem_wait(mutex);
  }
  sem_wait(hungerDown);
  sem_post(hungerUpper);
  ++totalDownNum;
  ++downNum;
  pthread_t tid = pthread_self();
  printf(
      "Tid: %p \033[93mBegin ↓↓↓<<<<Down<<<<↓↓↓\033[0m upNum %d downNum: %d "
      "upTotol %d downTotal %d\n",
      tid, upNum, downNum, totalUpNum, totalDownNum);

  for (int i = LOCKNUM - 1; i >= 0; --i) {
    sem_wait(sl[i]);
    printf("Tid: %p \033[93m%d\033[0m Down\n", tid, i + 1);
    sem_post(sl[i]);
    if (i == LOCKNUM - 1) sem_post(downBegin);
  }
  --downNum;
  printf("Tid: %p \033[93mship C.I.F. <<<<<Downnnnnn<<<<<\033[0m\n", tid);
  if (!downNum) sem_post(mutex);
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
