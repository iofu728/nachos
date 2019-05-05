/* sort.c 
 *    Test program to sort a large number of integers.
 *
 *    Intention is to stress virtual memory system.
 *
 *    Ideally, we could read the unsorted array off of the file system,
 *	and store the result back to the file system!
 */

#include "syscall.h"

int partition (int arr[], int low, int high)
{
    int pivot = arr[high];  
 
    int i = low, temp, j;

    for (j = low; j <= high - 1; ++j)
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


int main()
{
    int A[4];
    int i, j, tmp;
    for (i = 0; i < 4; ++i)
        A[i] = 4 - i;
    quickSort(A, 0, 3);
    // for (int i = 0; i < 4; ++i)
    //     cout << A[i];
    Halt();
}