/*
 * @Author: gunjianpan
 * @Date:   2019-06-03 22:23:46
 * @Last Modified by:   gunjianpan
 * @Last Modified time: 2019-06-03 22:23:54
 */

#include "syscall.h"

void ThreadA() {
    char a[6], ch = 'a';
    int i;
    OpenFileId fd;    
    a[0] = 'a';
    a[1] = '.';
    a[2] = 't';
    a[3] = 'x';
    a[4] = 't';
    a[5] = '\0';
    fd = Open(a);
    for(i = 0; i < 10; i ++) {
        Write(&ch, 1, fd);
        Yield();
    }
}

void ThreadB() {
    char a[6], ch = 'b';
    int i;
    OpenFileId fd;    
    a[0] = 'a';
    a[1] = '.';
    a[2] = 't';
    a[3] = 'x';
    a[4] = 't';
    a[5] = '\0';
    fd = Open(a);
    for(i = 0; i < 10; i ++) {
        Write(&ch, 1, fd);
        Yield();
    }
}

int main() {
    Fork(ThreadA);
    Fork(ThreadB);
} 