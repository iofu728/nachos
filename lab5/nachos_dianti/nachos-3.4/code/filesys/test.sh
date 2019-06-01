#!/bin/bash
# @Author: gunjianpan
# @Date:   2019-06-01 14:54:14
# @Last Modified by:   gunjianpan
# @Last Modified time: 2019-06-01 14:54:44


make depend && make nachos

./nachos -f

./nachos -cp test/big big

./nachos -D