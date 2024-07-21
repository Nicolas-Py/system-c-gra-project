import csv
import random

mem_pointer = 0


def read(writer, op, address, data=''):
    writer.writerow([op, hex(address * 32), data])


def write(writer, op, i, ii, data):
    for addr in range(i, ii + 1):
        writer.writerow([op, hex(addr * 32), data[addr - i]])


def merge_sort(arr, writer):
    """
    Sorts an array using the mergesort algorithm and generates instructions in the csv format

    Args:
        arr: The input array to be sorted. (length of the array must be a power of 2)
        writer: An instance of csv.writer(csvfile)
    """
    global mem_pointer

    if len(arr) <= 1:
        prev = mem_pointer
        mem_pointer += len(arr) - 1
        print(f"Write at: {prev}-{mem_pointer}, the data {arr}")
        write(writer, "W", prev, mem_pointer, arr)
        mem_pointer += 1
        return arr

    prev = mem_pointer
    mem_pointer += len(arr) - 1
    print(f"Write at: {prev}-{mem_pointer}, the data {arr}")
    write(writer, "W", prev, mem_pointer, arr)
    mem_pointer += 1

    mid = len(arr) // 2
    left = merge_sort(arr[:mid], writer)
    right = merge_sort(arr[mid:], writer)

    return merge(left, right, writer)


def merge(left, right, writer):
    global mem_pointer
    result = []
    i, j = 0, 0
    mem_pointer -= (len(right) + len(left))  # starts at the beginning
    l_pointer = mem_pointer  # pointer for the left sub array
    r_pointer = mem_pointer + len(left)  # pointer for the right sub array

    # the print statements give insight into the performed operations
    while i < len(left) and j < len(right):
        print(f"Read at address: {l_pointer}, the datum: {left[i]}")
        read(writer, 'R', l_pointer)
        print(f"Read at address: {r_pointer}, the datum: {right[j]}")
        read(writer, 'R', r_pointer)
        if left[i] <= right[j]:
            print(f"added from left array {left[i]}")
            result.append(left[i])
            l_pointer += 1
            i += 1
        else:
            print(f"added from right array {right[j]}")
            result.append(right[j])
            r_pointer += 1
            j += 1

    while i < len(left):
        print(f"Read at address: {l_pointer}, the datum: {left[i]}")
        read(writer, 'R', l_pointer)
        print(f"added from left array {left[i]}")
        result.append(left[i])
        l_pointer += 1
        i += 1

    while j < len(right):
        print(f"Read at address: {r_pointer}, the datum: {right[j]}")
        read(writer, 'R', r_pointer)
        print(f"added from right array {right[j]}")
        result.append(right[j])
        r_pointer += 1
        j += 1

    print(f"Write at: {mem_pointer - len(result)}-{mem_pointer - 1}, the data {result}")
    write(writer, "W", mem_pointer - len(result), mem_pointer - 1, result)
    return result


if __name__ == '__main__':
    with open("../examples/merge_sort_out.csv", 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        n = 5
        arr = []

        # generates an test array of length 2^n
        for i in range(2 ** n):
            arr.append(i)

        # shuffles the array
        random.shuffle(arr)
        print(f"Initial array: {arr}")
        sorted_arr = merge_sort(arr, writer)
        print(f"Sorted array: {sorted_arr}")
