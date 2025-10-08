# Spinlock Benchmarking

Measure and compare simple spinlock implementations under different contention patterns, and contrast them with `std::mutex` using Google Benchmark.

Implemented locks (in `include/spinlock.hpp`):
- `concurrency::base_lock` – minimal CAS loop
- `concurrency::ticket_lock` – FIFO fairness via ticketing
- `concurrency::double_check_lock` – test-then-CAS (aka TTAS)
- `concurrency::double_check_lock2` – TTAS with exponential backoff and `cpu_relax()`

Benchmarks live in `src/benchmark.cpp` and a simple correctness test in `src/test.cpp`.

## Prerequisites

- A C++17 compiler (GCC/Clang)
- CMake 3.14+
- Git (for fetching Google Benchmark)
- POSIX threads (pthread)

Google Benchmark is fetched automatically via CMake FetchContent.

## Build

```bash
# From repository root
mkdir -p build
cd build
cmake ..
cmake --build .
```

This produces the binaries in `build/`:
- `spinlock_bench` – Google Benchmark suite
- `spinlock_test` – small multi-threaded correctness test

## Run

### Benchmarks

Run the full suite:

```bash
./build/spinlock_bench
```

List available benchmarks:

```bash
./build/spinlock_bench --benchmark_list_tests
```

Filter by name (regex):

```bash
./build/spinlock_bench --benchmark_filter=DirectUpdate_.*
```

Change repetitions and output format:

```bash
./build/spinlock_bench --benchmark_repetitions=5 --benchmark_report_aggregates_only=true --benchmark_out=results.json --benchmark_out_format=json
```

The suite runs with thread counts 1, 2, 4, 8, 16 for each scenario:
- DirectUpdate: every iteration locks, increments, unlocks
- AccumulatedUpdate: local accumulation, single locked update

### Tests

```bash
./build/spinlock_test
```

Example output:

```
Testing base_lock...
PASSED: Counter value is 40000
...
```

## Notes on implementations

- `cpu_relax()` uses `_mm_pause` on x86 and `yield` on ARM to reduce contention; otherwise falls back to `std::this_thread::yield()`.
- `ticket_lock` provides fairness but can increase cache traffic.
- `double_check_lock2` adds exponential backoff to reduce bus locking under heavy contention.

## Troubleshooting

- If CMake can’t fetch Google Benchmark, ensure `git` and internet access are available, then re-run the configure step.
- If linking fails with pthread errors, make sure your toolchain provides `-pthread` (CMake uses it via target_link_libraries).
- For release-like measurements, consider building with `-DCMAKE_BUILD_TYPE=Release`:

```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

## Repository layout

```
include/        # spinlock.hpp – lock implementations
src/            # benchmark.cpp, test.cpp
CMakeLists.txt  # build config (FetchContent for Google Benchmark)
build/          # build artifacts and binaries (after building)
```

## Benchmark results (2025-10-08)

The following results were collected on this machine using Google Benchmark (means in nanoseconds). Note the warnings about CPU scaling and DEBUG build.

### System specs

- OS/Kernel: Linux 6.8.0-84-generic (Ubuntu) x86_64
- CPU: Intel(R) Core(TM) i9-14900KS, 32 logical CPUs (24 cores, 2 threads/core)
- Caches: L1d 896 KiB (24x), L1i 1.3 MiB (24x), L2 32 MiB (12x), L3 36 MiB (1x)
- Memory: 125 GiB
- Toolchain: GCC 13.3.0, CMake 3.28.3
- Build type: DEBUG (as indicated by benchmark output)

### Command

```fish
./build/spinlock_bench \
	--benchmark_repetitions=5 \
	--benchmark_report_aggregates_only=true \
	--benchmark_time_unit=ns
```

### Output (means)

```
2025-10-08T09:08:33+00:00
Running /home/vibhatsu/spinlock_benchmarking/build/spinlock_bench
Run on (32 X 5900 MHz CPU s)
CPU Caches:
	L1 Data 48 KiB (x16)
	L1 Instruction 32 KiB (x16)
	L2 Unified 2048 KiB (x16)
	L3 Unified 36864 KiB (x1)
Load Average: 1.00, 1.00, 1.00
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
***WARNING*** Library was built as DEBUG. Timings may be affected.
DirectUpdate_BaseLock/1_mean                      35879 ns         7526 ns            5 items_per_second=132.882M/s
DirectUpdate_BaseLock/2_mean                     105482 ns        14251 ns            5 items_per_second=140.353M/s
DirectUpdate_BaseLock/4_mean                     349280 ns        28439 ns            5 items_per_second=140.749M/s
DirectUpdate_BaseLock/8_mean                    1350595 ns       111304 ns            5 items_per_second=72.0518M/s
DirectUpdate_BaseLock/16_mean                   3716008 ns       302262 ns            5 items_per_second=52.9566M/s
DirectUpdate_TicketLock/1_mean                    30528 ns         7339 ns            5 items_per_second=136.273M/s
DirectUpdate_TicketLock/2_mean                   196855 ns        15197 ns            5 items_per_second=131.65M/s
DirectUpdate_TicketLock/4_mean                   544354 ns        38959 ns            5 items_per_second=102.781M/s
DirectUpdate_TicketLock/8_mean                  1404585 ns       132319 ns            5 items_per_second=60.4638M/s
DirectUpdate_TicketLock/16_mean                 4024003 ns       375577 ns            5 items_per_second=42.6106M/s
DirectUpdate_DoubleCheckLock/1_mean               38525 ns         7339 ns            5 items_per_second=136.27M/s
DirectUpdate_DoubleCheckLock/2_mean              122458 ns        13701 ns            5 items_per_second=145.991M/s
DirectUpdate_DoubleCheckLock/4_mean              402684 ns        28761 ns            5 items_per_second=139.152M/s
DirectUpdate_DoubleCheckLock/8_mean             1790281 ns       108708 ns            5 items_per_second=73.8004M/s
DirectUpdate_DoubleCheckLock/16_mean            4481217 ns       294824 ns            5 items_per_second=54.2726M/s
DirectUpdate_DoubleCheckLock2/1_mean              39342 ns         7564 ns            5 items_per_second=132.239M/s
DirectUpdate_DoubleCheckLock2/2_mean             125237 ns        13915 ns            5 items_per_second=143.763M/s
DirectUpdate_DoubleCheckLock2/4_mean             404059 ns        28326 ns            5 items_per_second=141.27M/s
DirectUpdate_DoubleCheckLock2/8_mean            1045066 ns       115376 ns            5 items_per_second=69.3641M/s
DirectUpdate_DoubleCheckLock2/16_mean           2303170 ns       243615 ns            5 items_per_second=65.6792M/s
DirectUpdate_StdMutex/1_mean                      35312 ns         7354 ns            5 items_per_second=135.983M/s
DirectUpdate_StdMutex/2_mean                     114646 ns        14085 ns            5 items_per_second=142.03M/s
DirectUpdate_StdMutex/4_mean                     285902 ns        30103 ns            5 items_per_second=133.016M/s
DirectUpdate_StdMutex/8_mean                     876442 ns       113695 ns            5 items_per_second=70.3772M/s
DirectUpdate_StdMutex/16_mean                   1913356 ns       292113 ns            5 items_per_second=54.7737M/s
AccumulatedUpdate_BaseLock/1_mean                 14930 ns         7610 ns            5 items_per_second=131.427M/s
AccumulatedUpdate_BaseLock/2_mean                 21463 ns        14644 ns            5 items_per_second=136.589M/s
AccumulatedUpdate_BaseLock/4_mean                 33992 ns        27007 ns            5 items_per_second=148.125M/s
AccumulatedUpdate_BaseLock/8_mean                 79725 ns        75047 ns            5 items_per_second=106.601M/s
AccumulatedUpdate_BaseLock/16_mean               173793 ns       167899 ns            5 items_per_second=95.3117M/s
AccumulatedUpdate_TicketLock/1_mean               14082 ns         7661 ns            5 items_per_second=130.547M/s
AccumulatedUpdate_TicketLock/2_mean               20720 ns        14572 ns            5 items_per_second=137.268M/s
AccumulatedUpdate_TicketLock/4_mean               33326 ns        26758 ns            5 items_per_second=149.503M/s
AccumulatedUpdate_TicketLock/8_mean               79981 ns        76244 ns            5 items_per_second=104.93M/s
AccumulatedUpdate_TicketLock/16_mean             170851 ns       165196 ns            5 items_per_second=96.8705M/s
AccumulatedUpdate_DoubleCheckLock/1_mean          14218 ns         7661 ns            5 items_per_second=130.56M/s
AccumulatedUpdate_DoubleCheckLock/2_mean          20587 ns        14487 ns            5 items_per_second=138.061M/s
AccumulatedUpdate_DoubleCheckLock/4_mean          33201 ns        26640 ns            5 items_per_second=150.174M/s
AccumulatedUpdate_DoubleCheckLock/8_mean          79587 ns        75847 ns            5 items_per_second=105.478M/s
AccumulatedUpdate_DoubleCheckLock/16_mean        172516 ns       167716 ns            5 items_per_second=95.4075M/s
AccumulatedUpdate_DoubleCheckLock2/1_mean         14184 ns         7589 ns            5 items_per_second=131.78M/s
AccumulatedUpdate_DoubleCheckLock2/2_mean         20570 ns        14448 ns            5 items_per_second=138.428M/s
AccumulatedUpdate_DoubleCheckLock2/4_mean         33481 ns        26902 ns            5 items_per_second=148.689M/s
AccumulatedUpdate_DoubleCheckLock2/8_mean         81383 ns        76961 ns            5 items_per_second=103.967M/s
AccumulatedUpdate_DoubleCheckLock2/16_mean       173321 ns       168152 ns            5 items_per_second=95.1686M/s
AccumulatedUpdate_StdMutex/1_mean                 14570 ns         7537 ns            5 items_per_second=132.683M/s
AccumulatedUpdate_StdMutex/2_mean                 20946 ns        14420 ns            5 items_per_second=138.713M/s
AccumulatedUpdate_StdMutex/4_mean                 33399 ns        26589 ns            5 items_per_second=150.451M/s
AccumulatedUpdate_StdMutex/8_mean                 81083 ns        76560 ns            5 items_per_second=104.502M/s
AccumulatedUpdate_StdMutex/16_mean               175207 ns       171282 ns            5 items_per_second=93.4214M/s
```