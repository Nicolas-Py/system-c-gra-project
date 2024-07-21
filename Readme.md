#

# Urls
### Artemis:
Nico:
```
https://go98mir@artemis.cit.tum.de/git/GRA24CDAPROJECT/gra24cdaproject-g116.git
```
Daniel:
```
https://go98puj@artemis.cit.tum.de/git/GRA24CDAPROJECT/gra24cdaproject-g116.git
```
Ziang:
```
https://go56siq@artemis.cit.tum.de/git/GRA24CDAPROJECT/gra24cdaproject-g116.git
```
### [GitHub](https://github.com/Nicolas-Py/system-c-gra-project.git):
```
https://github.com/Nicolas-Py/system-c-gra-project.git
```

# How to run
1. Clean old executables
    ``` bash
    make clean
    ```
2. Build the project
    ``` bash
    make
    ```
3. Test the program
    ``` bash
    ./cache_simulation "./examples/random_small.csv"
    ```
4. get familiar with the options
    ``` bash
    ./cache_simulation -h
5. Run the program with further customisation e.g. cycle count 
    ``` bash
    ./cache_simulation -c 1000 "./examples/random_small.csv"
    ```
# /examples/
This dir contains some .csv files that you can use for testing. 
## Random Operations
These are instructions generated with `csv_factory_test.py`
### random_small.csv
100 lines of random read/write instructions
### random_medium.csv
1000 lines of random read/write instructions
### random_big.csv
10000 lines of random read/write instructions

## Merge Sort
These are instructions generated with `merge_sort.py`. It simulates the classic mergesort but every sub array that gets
passed down in the recursion tree is written in the heap to better simulate the effects on the heap. When sub-arrays get merged
the resulting array is written in the location of the previously unsorted array and all no longer needed memory is "freed"
### merge_sort_small.csv
simulates merge sort with the array 

`[2, 0, 1, 3]`

the steps are
1. Write at: 0-3, the data [2, 0, 1, 3]
2. Write at: 4-5, the data [2, 0]
3. Write at: 6-6, the data [2]
4. Write at: 7-7, the data [0]
5. Read at address: 6, the datum: 2
6. Read at address: 7, the datum: 0
7. added from right array 0
8. Read at address: 6, the datum: 2
9. added from left array 2
10. Write at: 4-5, the data [0, 2]
11. Write at: 6-7, the data [1, 3]
12. Write at: 8-8, the data [1]
13. Write at: 9-9, the data [3]
14. Read at address: 8, the datum: 1
15. Read at address: 9, the datum: 3
16. added from left array 1
17. Read at address: 9, the datum: 3
18. added from right array 3
19. Write at: 6-7, the data [1, 3]
20. Read at address: 4, the datum: 0
21. Read at address: 6, the datum: 1
22. added from left array 0
23. Read at address: 5, the datum: 2
24. Read at address: 6, the datum: 1
25. added from right array 1
26. Read at address: 5, the datum: 2
27. Read at address: 7, the datum: 3
28. added from left array 2
29. Read at address: 7, the datum: 3
30. added from right array 3
31. Write at: 0-3, the data [0, 1, 2, 3]
32. Sorted array: [0, 1, 2, 3]

### merge_sort_big.csv
simulates merge sort with the array

`[64, 18, 22, 226, 116, 241, 208, 144, 44, 147, 229, 201, 80, 69, 174, 239, 250, 166, 220, 213, 186, 172, 23, 107, 77, 152, 185, 141, 108, 143, 72, 170, 184, 197, 179, 161, 52, 30, 120, 65, 135, 236, 118, 26, 17, 233, 93, 153, 207, 255, 86, 119, 132, 206, 129, 60, 182, 40, 48, 248, 51, 160, 20, 177, 83, 235, 168, 3, 137, 151, 187, 78, 253, 84, 194, 254, 101, 232, 102, 205, 92, 125, 200, 7, 240, 36, 46, 238, 149, 243, 55, 159, 190, 221, 211, 224, 209, 111, 127, 140, 34, 231, 138, 12, 109, 47, 180, 163, 157, 162, 58, 123, 112, 100, 171, 214, 188, 145, 16, 94, 242, 146, 196, 113, 124, 106, 156, 164, 122, 67, 251, 87, 136, 90, 150, 104, 89, 8, 227, 121, 28, 66, 45, 21, 99, 54, 74, 219, 228, 25, 6, 237, 50, 98, 13, 35, 115, 247, 199, 178, 230, 252, 43, 49, 198, 73, 191, 59, 165, 222, 155, 173, 97, 223, 114, 10, 218, 32, 139, 126, 57, 134, 37, 215, 167, 53, 105, 176, 5, 142, 249, 117, 75, 61, 14, 181, 96, 91, 71, 27, 39, 56, 29, 154, 175, 133, 41, 19, 81, 204, 62, 217, 169, 246, 85, 189, 234, 192, 148, 88, 130, 63, 4, 245, 103, 15, 2, 38, 216, 0, 195, 193, 82, 212, 244, 95, 1, 110, 76, 42, 9, 33, 202, 210, 225, 131, 11, 183, 24, 31, 128, 203, 79, 68, 158, 70]`

if you want to see the read/write steps in a more readable way run `test_files/merge_sort.py`