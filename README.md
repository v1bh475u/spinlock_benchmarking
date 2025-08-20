# Spinlock Benchmarking

This project implements and benchmarks various spinlock implementations:
- Base spinlock using atomic compare-exchange
- Ticket-based spinlock
- Comparison with std::mutex

## Building

```bash
# Create a build directory
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build
cmake --build .
```

## Running the benchmarks

```bash
./spinlock_bench
```

## Running the tests

```bash
./spinlock_test
```

## Implementations

- `base_lock`: Simple spinlock using compare-exchange operations
- `ticket_lock`: Ticket-based spinlock for fairness
