# 三峡大坝问题(典型 IPC 问题讨论)

<p align="right">1801210840 姜慧强</p>

## 问题描述

在三峡大坝上下游各有若干船舶准备借助三峡大坝的船闸来渡过三峡。

三峡大坝 海拔相差 100 余米，通过五级船闸来实现调度.

每个船闸有两个状态(上行/下行), 状态之间相互互斥。

船舶需要依次通过五级船闸才能顺利度过三峡大坝,

<center><img width="320" src="https://cdn.nlark.com/yuque/0/2019/png/104214/1555835056972-90caf820-1644-4755-979d-c0c42d9ed93b.png"></center>

## 问题关键点

在保证不发生死锁的情况下尽量提高运行效率，以及尽量避免饥饿的出现。

该问题中出现了以下的互斥关系：

- 每个船闸的进出是互斥的
- 船闸两个状态之间是互斥的

## 解法 PV

构造

- 五级船闸各一个信号量, sl0, sl1, sl2, sl3, sl4
- 向上向下开始的各一个信号量保证队首唯一, upBegin, downBegin
- 决定向上还是向下, mutex

* 设置 while 判断对方方向有没有来船，若有则主动让出 MUTEX

### Presudo code

```cpp
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
```

### Operation Code

```c
/*
 * @Author: gunjianpan
 * @Date:   2019-04-21 16:36:14
 * @Last Modified by:   gunjianpan
 * @Last Modified time: 2019-04-22 00:41:15
 */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#define THREADNUM 100
#define LOCKNUM 5
int upNum = 0, downNum = 0;
sem_t *sl[LOCKNUM], *mutex, *upBegin, *downBegin;
pthread_t tidList[THREADNUM];

void *Up(void *b) {
  // up & down sent one ship once
  sem_wait(upBegin);
  if (!upNum) sem_wait(mutex);  // flow one direction(no care downNum)
  while (downNum) {
    sem_post(mutex);
    sem_wait(mutex);
  }
  ++upNum;
  pthread_t tid = pthread_self();
  printf("Tid: %p \033[91mBegin ΔΔΔ>>>>UP>>>>ΔΔΔ\033[0m upNum %d downNum: %d\n", tid, upNum, downNum);

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
  while (upNum) {
    sem_post(mutex);
    sem_wait(mutex);
  }
  ++downNum;
  pthread_t tid = pthread_self();
  printf("Tid: %p \033[93mBegin ↓↓↓<<<<Down<<<<↓↓↓\033[0m upNum %d downNum: %d\n", tid, upNum, downNum);

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
  printf("\033[01;34m Write by Jiang Huiqiang 1801210840 in 2019-04-21 \033[0m\n");
  sem_unlink("sem");
  for (int i = 0; i < LOCKNUM; ++i) sl[i] = sem_open("sem", O_CREAT, 0, 1);
  upBegin = sem_open("sem", O_CREAT, 0, 1);
  downBegin = sem_open("sem", O_CREAT, 0, 1);
  mutex = sem_open("sem", O_CREAT, 0, 1);

  for (int i = 0; i < THREADNUM; ++i) thread_one_create(tidList[i], i % 2);
  for (int i = 0; i < THREADNUM; ++i) thread_one_join(tidList[i]);

  for (int i = 0; i < LOCKNUM; ++i) sem_destroy(sl[i]);

  pthread_exit(NULL);
}
```

![image](https://cdn.nlark.com/yuque/0/2019/png/104214/1555858416067-fb4812e5-49c5-4cb2-8489-a03a1129c20b.png)

![image](https://cdn.nlark.com/yuque/0/2019/png/104214/1555858418552-a634f05e-c26c-4807-874d-d97eebfdae8f.png)

## For Hunger

设置 totalUpNum, totalDownNum

```c
/*
 * @Author: gunjianpan
 * @Date:   2019-04-21 16:36:14
 * @Last Modified by:   gunjianpan
 * @Last Modified time: 2019-04-21 22:25:48
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
sem_t *sl[LOCKNUM], *mutex, *upBegin, *downBegin;
pthread_t tidList[THREADNUM];

void *Up(void *b) {
  // up & down sent one ship once
  sem_wait(upBegin);
  if (!upNum) sem_wait(mutex);  // flow one direction(no care downNum)
  while (downNum || totalUpNum - totalDownNum > HUNGERNUM) {
    sem_post(mutex);
    sem_wait(mutex);
  }
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

  for (int i = 0; i < THREADNUM; ++i) thread_one_create(tidList[i], i % 2);
  for (int i = 0; i < THREADNUM; ++i) thread_one_join(tidList[i]);

  for (int i = 0; i < LOCKNUM; ++i) sem_destroy(sl[i]);

  pthread_exit(NULL);
}
```

![image](https://cdn.nlark.com/yuque/0/2019/png/104214/1555858451683-8be38bae-1c32-43e3-9caa-0da495572fbf.png)

## 解决 信号量维护 hugger

```cpp
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

```

![image](https://cdn.nlark.com/yuque/0/2019/png/104214/1555858706563-9fe11b7f-004f-4c9a-8512-a2a0a2316115.png)
