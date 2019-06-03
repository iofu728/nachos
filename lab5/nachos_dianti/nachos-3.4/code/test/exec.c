/*
 * @Author: gunjianpan
 * @Date:   2019-06-03 22:23:32
 * @Last Modified by:   gunjianpan
 * @Last Modified time: 2019-06-03 22:23:39
 */

#include "syscall.h"

int main() {
    char a[5];
    int exitCode = -1;
    SpaceId sp;

    a[0] = 't';
    a[1] = 'e';
    a[2] = 's';
    a[3] = 't';
    a[4] = '\0';

    sp = Exec(a);

    exitCode = Join(sp);
    Exit(exitCode);
} 