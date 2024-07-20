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

`[29, 84, 89, 51, 0, 24, 112, 37, 56, 74, 9, 61, 118, 120, 5, 80, 117, 31, 81, 23, 20, 54, 8, 68, 123, 115, 39, 45, 106, 107, 119, 35, 109, 41, 15, 96, 4, 124, 73, 86, 113, 65, 18, 85, 44, 121, 122, 36, 69, 60, 46, 66, 42, 16, 88, 50, 3, 38, 27, 97, 62, 2, 14, 63, 30, 57, 21, 98, 100, 49, 93, 64, 72, 13, 34, 43, 90, 52, 77, 76, 126, 87, 99, 11, 10, 53, 116, 7, 17, 1, 48, 6, 101, 78, 28, 110, 55, 83, 26, 40, 82, 58, 25, 114, 111, 79, 102, 92, 104, 59, 70, 12, 22, 75, 47, 91, 19, 33, 94, 125, 67, 32, 105, 108, 71, 95, 127, 103]`

if you want to see the read/write steps in a more readable way run `test_files/merge_sort.py`