import os
import random
import subprocess
import time


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


def run_tests(executable, num_tests, test_files, direct_mapped):
    for test_num in range(num_tests):
        print(f"Running test {test_num + 1}/{num_tests}")

        # Generate random parameters
        cycles = 100000
        cache_lines = 128
        cache_line_size = 128
        cache_latency = 5
        memory_latency = 100


        # Generate input file
        input_file = test_files[test_num]

        # Run simulation and measure time
        start_time = time.time()
        stdout, stderr = run_simulation(
            executable, cycles, direct_mapped, cache_lines,
            cache_line_size, cache_latency, memory_latency, input_file
        )
        end_time = time.time()
        test_duration = end_time - start_time

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

            assert results['Primitive gate count'] > 0, "Primitive gate count should be positive"

            hit_ratio = results['Cache hits'] / (results['Cache hits'] + results['Cache misses'])

            # Print test details
            print(f"Test {test_num + 1} details:")
            print(f"  Duration: {test_duration:.2f} seconds")
            print(f"  Input file: {input_file}")
            print(f"  Cache type: {'Direct-mapped' if direct_mapped else 'Fully Associative'}")
            print(f"  Cache hit ratio: {hit_ratio:.2f}")
            print(f"Results: {results}")

            print(f"Test {test_num + 1} passed")
        except AssertionError as e:
            print(f"Test {test_num + 1} failed: {str(e)}")
        except Exception as e:
            print(f"Unexpected error in test {test_num + 1}: {str(e)}")

        print("---")

        # Clean up input file
        # os.remove(input_file)


if __name__ == '__main__':
    executable_path = "../cache_simulation"
    test_files = ["../test_files/merge_sort_out.csv"]

    run_tests(executable_path, len(test_files), test_files, False)
    run_tests(executable_path, len(test_files), test_files, True)
