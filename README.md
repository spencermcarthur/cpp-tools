# C++ Tooling Library

## Dependencies
- libpthread
- libgtest (unit tests only)
- libbenchmark (benchmarks only)

## Building & Installing
```
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
sudo make install
```

### Tests
```
make build_tests -j
make run_tests
```

### Benchmarks
```
make build_benchmarks -j
make run_benchmarks
```

## Classes
- `spinlock_mutex`: a fast mutex class
- `thread`: a nameable, core-assignable thread class
