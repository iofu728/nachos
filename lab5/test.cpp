/*
 * @Author: gunjianpan
 * @Date:   2019-05-05 19:33:58
 * @Last Modified by:   gunjianpan
 * @Last Modified time: 2019-06-02 13:10:37
 */

#include <iostream>
#include <ctime>
#include <string.h>
using namespace std;

int partition(int arr[], int low, int high)
{
    int pivot = arr[high];

    int i = low, temp;

    for (int j = low; j <= high - 1; ++j)
    {
        if (arr[j] <= pivot)
        {
            ++i;
            temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    return i + 1;
}

void quickSort(int arr[], int low, int high)
{
    if (low < high)
    {
        int pi = partition(arr, low, high);

        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int first_bit(int n)
{
    int count = 0;
    while ((n >> 1 << 1) == n)
    {
        cout << (n ^ 1) << endl;
        n >>= 1;
        count++;
        cout << n << ' ' << count << endl;
    }
    return (1 << count);
}

int main()
{

    time_t timep;
    time (&timep);
    printf("%s", asctime(gmtime(&timep)));
    char *name = (char *)"test";
    char *endPath = strchr(name, '/');
    cout << endPath << endl;

    return 0;
}