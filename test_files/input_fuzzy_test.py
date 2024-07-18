import subprocess
import random
import string
import csv


def generate_random_string(length):
    return ''.join(random.choice(string.ascii_letters + string.digits) for _ in range(length))


def create_valid_input_file(filename, num_instructions):
    operations = ['R', 'W']
    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        for _ in range(num_instructions):
            op = random.choice(operations)
            address = random.randint(0, 0xFFFF)  # 16-bit address space
            data = random.randint(0, 0xFFFF) if op == 'W' else ''
            writer.writerow([op, hex(address), data])

def run_simulation(executable, args):
    cmd = [executable] + args
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=10)
        return result.returncode, result.stdout, result.stderr
    except subprocess.TimeoutExpired:
        return -1, "", "Timeout"

def fuzz_test(executable, input_file, num_tests):
    for test_num in range(num_tests):
        print(f"Running fuzz test {test_num + 1}/{num_tests}")

        # Generate random arguments
        args = []
        num_args = random.randint(0, 10)
        for _ in range(num_args):
            arg_type = random.choice(['normal', 'extreme', 'normal', 'extreme', 'normal', 'extreme', 'invalid'])
            if arg_type == 'normal':
                param = random.choice(['cycles', 'cachelines', 'cacheline-size', 'cache-latency', 'memory-latency'])
                value = random.randint(1, 1000)
                args.append(f"--{param}={value}")
            elif arg_type == 'extreme':
                param = random.choice(['cycles', 'cachelines', 'cacheline-size', 'cache-latency', 'memory-latency'])
                value = random.randint(1000000, 1000000000)
                args.append(f"--{param}={value}")
            else:  # invalid
                args.append(f"--{generate_random_string(5)}={generate_random_string(5)}")

        # Randomly choose between valid and invalid cache types
        cache_type = random.choice(['directmapped', 'fullassociative', 'invalid'])
        if cache_type == 'invalid':
            args.append(f"--{generate_random_string(10)}")
        else:
            args.append(f"--{cache_type}")

        # Add the valid input file
        args.append(input_file)

        # Run simulation
        return_code, stdout, stderr = run_simulation(executable, args)

        # Analyze results
        if return_code == 0:
            print(f"Test {test_num + 1}: Program completed successfully")
            print(f"Output: {stdout}")
        elif return_code == -1:
            print(f"Test {test_num + 1}: Program timed out")
        else:
            print(f"Test {test_num + 1}: Program exited with code {return_code}")
            if stderr:
                print(f"Error output: {stderr}")

        print(f"Arguments used: {' '.join(args)}")
        print("---")


if __name__ == "__main__":
    executable_path = "../cache_simulation"  # Update this with the path to your compiled C program
    input_file = "valid_input.csv"  # This will be our fixed, valid input file
    num_tests = 100  # Number of fuzz tests to run

    # Create a valid input file once, to be used for all tests
    create_valid_input_file(input_file, num_instructions=1000)

    fuzz_test(executable_path, input_file, num_tests)