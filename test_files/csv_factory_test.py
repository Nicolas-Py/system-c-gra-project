import subprocess
import random
import csv
import os


def generate_csv(filename, num_instructions):
    operations = ['R', 'W']
    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        for _ in range(num_instructions):
            op = random.choice(operations)
            address = random.randint(0, 0xFFFF)  # 16-bit address space
            data = random.randint(0, 0xFFFF) if op == 'W' else ''
            writer.writerow([op, hex(address), data])


def run_simulation(executable, cycles, direct_mapped, cache_lines, cache_line_size, cache_latency, memory_latency, input_file):
    cmd = [
        executable,
        f'--cycles={cycles}',
        f'--{"directmapped" if direct_mapped else "fullassociative"}',
        f'--cachelines={cache_lines}',
        f'--cacheline-size={cache_line_size}',
        f'--cache-latency={cache_latency}',
        f'--memory-latency={memory_latency}',
        input_file
    ]
    result = subprocess.run(cmd, capture_output=True, text=True)
    return result.stdout, result.stderr


def parse_results(output):
    results = {}
    lines = output.split('\n')
    if "Simulation Results:" in lines:
        start_index = lines.index("Simulation Results:") + 1
        for line in lines[start_index:]:
            if ':' in line:
                key, value = line.split(':')
                key = key.strip()
                value = int(value.strip())
                results[key] = value
    return results


def run_tests(executable, num_tests):
    for test_num in range(num_tests):
        print(f"Running test {test_num + 1}/{num_tests}")

        # Generate random parameters
        cycles = random.randint(100000, 1000000)
        direct_mapped = random.choice([True, False])
        cache_lines = random.choice([32, 64, 128, 256])
        cache_line_size = random.choice([32, 64, 128])
        cache_latency = random.randint(1, 10)
        memory_latency = random.randint(50, 200)
        num_instructions = random.randint(100, 1000)

        # Generate input file
        input_file = f"test_input_{test_num}.csv"
        generate_csv(input_file, num_instructions)

        # Run simulation
        stdout, stderr = run_simulation(
            executable, cycles, direct_mapped, cache_lines,
            cache_line_size, cache_latency, memory_latency, input_file
        )

        if stderr:
            print(f"Error in test {test_num + 1}:")
            print(stderr)
            continue

        # Parse results
        results = parse_results(stdout)

        # Verify results
        try:
            expected_keys = ['Cycles', 'Cache hits', 'Cache misses', 'Primitive gate count']
            for key in expected_keys:
                assert key in results, f"{key} not found in output"


            assert results['Cache hits'] + results['Cache misses'] <= num_instructions, "Total cache accesses exceed number of instructions"
            assert results['Primitive gate count'] > 0, "Primitive gate count should be positive"

            hit_ratio = results['Cache hits'] / (results['Cache hits'] + results['Cache misses'])
            print(f"Cache hit ratio: {hit_ratio:.2f}")

            # Additional checks based on cache configuration
            if direct_mapped:
                assert results['Cache hits'] + results['Cache misses'] <= cache_lines * cycles, "Total accesses exceed maximum possible for direct-mapped cache"

            print(f"Test {test_num + 1} passed")
            print(f"Results: {results}")
        except AssertionError as e:
            print(f"Test {test_num + 1} failed: {str(e)}")
        except Exception as e:
            print(f"Unexpected error in test {test_num + 1}: {str(e)}")

        print("---")

        # Clean up input file
        os.remove(input_file)


if __name__ == "__main__":
    executable_path = "../cache_simulation"  # Update this with the path to your compiled C program
    num_tests = 100000  # Number of tests to run

    run_tests(executable_path, num_tests)