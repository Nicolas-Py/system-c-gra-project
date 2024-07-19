import csv
import math

mem_pointer = 0


def read(writer, op, address, data=''):
    writer.writerow([op, hex(address), data])


def write(writer, op, i, ii, data):
    for addr in range(i, ii + 1):
        writer.writerow([op, hex(addr), data[addr - i]])


def merge_sort(arr, writer):
    global mem_pointer

    if len(arr) <= 1:
        prev = mem_pointer
        mem_pointer += len(arr) - 1
        print(f"Write {prev}-{mem_pointer} {arr}")
        write(writer, "W", prev, mem_pointer, arr)
        mem_pointer += 1
        return arr

    prev = mem_pointer
    mem_pointer += len(arr) - 1
    print(f"Write {prev}-{mem_pointer} {arr}")
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
    # print(mem_pointer)
    mem_pointer -= (len(right) + len(left))
    # print(mem_pointer)
    l_pointer = mem_pointer
    r_pointer = mem_pointer + len(left)

    while i < len(left) and j < len(right):
        print(f"Read {l_pointer} {left[i]}")
        read(writer, 'R', l_pointer)
        print(f"Read {r_pointer} {right[j]}")
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
        print(f"Read {l_pointer} {left[i]}")
        read(writer, 'R', l_pointer)
        print(f"added from left array {left[i]}")
        result.append(left[i])
        l_pointer += 1
        i += 1

    while j < len(right):
        print(f"Read {r_pointer} {right[j]}")
        read(writer, 'R', r_pointer)
        print(f"added from right array {right[j]}")
        result.append(right[j])
        r_pointer += 1
        j += 1

    print(f"Write {mem_pointer - len(result)}-{mem_pointer - 1} {result}")
    write(writer, "W", mem_pointer - len(result), mem_pointer - 1, result)
    return result


if __name__ == '__main__':
    with open("test", 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)

        arr = [4, 3, 2, 1]
        print(f"Initial array: {arr}")
        sorted_arr = merge_sort(arr, writer)
        print(f"Sorted array: {sorted_arr}")
        print(f"Final memory usage: {mem_pointer}")
