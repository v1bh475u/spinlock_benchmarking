# Spinlock Benchmark Report

Generated from `benchmark.json` by `scripts/generate_report.py`.

## Machine And Run Metadata

| Field | Value |
| --- | --- |
| Timestamp UTC | `2026-06-01T16:26:38.866648+00:00` |
| Git commit | `e48947b45bb24a401340a858599bb394c9fb3d08` |
| Git branch | `dev` |
| Platform | `Linux-7.0.10-zen1-1.1-zen-x86_64-with-glibc2.43` |
| Machine | `x86_64` |
| Processor | `12th Gen Intel(R) Core(TM) i5-12450HX` |
| Kernel | `Linux luna 7.0.10-zen1-1.1-zen #1 ZEN SMP PREEMPT_DYNAMIC Sun, 24 May 2026 22:42:04 +0000 x86_64 GNU/Linux` |
| Compiler | `g++ (GCC) 16.1.1 20260430` |
| CMake | `cmake version 4.3.3` |

## Plots

### AccumulatedUpdate

![AccumulatedUpdate real time](plots/accumulatedupdate_real_time_ns.png)

![AccumulatedUpdate throughput](plots/accumulatedupdate_items_per_second.png)

### DirectUpdate

![DirectUpdate real time](plots/directupdate_real_time_ns.png)

![DirectUpdate throughput](plots/directupdate_items_per_second.png)

## Fastest Implementation By Scenario

| Scenario | Threads | Implementation | Real time (ns) | Items/s |
| --- | ---: | --- | ---: | ---: |
| AccumulatedUpdate | 1 | DoubleCheckLock | 14549 | 133412457.503 |
| AccumulatedUpdate | 2 | DoubleCheckLock | 20770 | 139945452.142 |
| AccumulatedUpdate | 4 | DoubleCheckLock | 47475 | 99150790.334 |
| AccumulatedUpdate | 8 | DoubleCheckLock | 91588 | 91274171.126 |
| AccumulatedUpdate | 16 | DoubleCheckLock | 183195 | 91128787.646 |
| AccumulatedUpdate | 32 | DoubleCheckLock | 367772 | 90554896.672 |
| DirectUpdate | 1 | DoubleCheckLock2 | 33680 | 130886458.291 |
| DirectUpdate | 2 | DoubleCheckLock2 | 73527 | 130471282.322 |
| DirectUpdate | 4 | DoubleCheckLock2 | 306222 | 69022139.598 |
| DirectUpdate | 8 | TicketLock | 1441045 | 72157286.545 |
| DirectUpdate | 16 | DoubleCheckLock | 3071997 | 67764005.430 |
| DirectUpdate | 32 | DoubleCheckLock | 7953574 | 65194635.929 |

## Raw Mean Results

| Scenario | Implementation | Threads | Real time (ns) | CPU time (ns) | Items/s |
| --- | --- | ---: | ---: | ---: | ---: |
| AccumulatedUpdate | BaseLock | 1 | 15766 | 8467 | 118715141.221 |
| AccumulatedUpdate | BaseLock | 2 | 21949 | 15433 | 129805721.430 |
| AccumulatedUpdate | BaseLock | 4 | 49515 | 42127 | 94990664.579 |
| AccumulatedUpdate | BaseLock | 8 | 94403 | 90200 | 88704010.877 |
| AccumulatedUpdate | BaseLock | 16 | 187073 | 180308 | 88739330.237 |
| AccumulatedUpdate | BaseLock | 32 | 377122 | 363916 | 87936889.357 |
| AccumulatedUpdate | DoubleCheckLock | 1 | 14549 | 7499 | 133412457.503 |
| AccumulatedUpdate | DoubleCheckLock | 2 | 20770 | 14293 | 139945452.142 |
| AccumulatedUpdate | DoubleCheckLock | 4 | 47475 | 40347 | 99150790.334 |
| AccumulatedUpdate | DoubleCheckLock | 8 | 91588 | 87652 | 91274171.126 |
| AccumulatedUpdate | DoubleCheckLock | 16 | 183195 | 175580 | 91128787.646 |
| AccumulatedUpdate | DoubleCheckLock | 32 | 367772 | 353384 | 90554896.672 |
| AccumulatedUpdate | DoubleCheckLock2 | 1 | 15578 | 8322 | 120261837.275 |
| AccumulatedUpdate | DoubleCheckLock2 | 2 | 22194 | 15376 | 130112636.864 |
| AccumulatedUpdate | DoubleCheckLock2 | 4 | 51408 | 43554 | 92080549.923 |
| AccumulatedUpdate | DoubleCheckLock2 | 8 | 106146 | 99792 | 80466098.263 |
| AccumulatedUpdate | DoubleCheckLock2 | 16 | 198696 | 189989 | 84226989.442 |
| AccumulatedUpdate | DoubleCheckLock2 | 32 | 389562 | 373044 | 85818572.549 |
| AccumulatedUpdate | StdMutex | 1 | 15208 | 7967 | 125629731.002 |
| AccumulatedUpdate | StdMutex | 2 | 21671 | 14986 | 133504786.457 |
| AccumulatedUpdate | StdMutex | 4 | 49980 | 42042 | 95199227.998 |
| AccumulatedUpdate | StdMutex | 8 | 95649 | 90682 | 88242271.516 |
| AccumulatedUpdate | StdMutex | 16 | 189836 | 180905 | 88452077.396 |
| AccumulatedUpdate | StdMutex | 32 | 380547 | 365044 | 87672067.686 |
| AccumulatedUpdate | TicketLock | 1 | 14844 | 7738 | 129282911.314 |
| AccumulatedUpdate | TicketLock | 2 | 22481 | 15126 | 132708708.513 |
| AccumulatedUpdate | TicketLock | 4 | 49739 | 42256 | 94730116.091 |
| AccumulatedUpdate | TicketLock | 8 | 93756 | 89632 | 89261022.209 |
| AccumulatedUpdate | TicketLock | 16 | 186917 | 179541 | 89121778.461 |
| AccumulatedUpdate | TicketLock | 32 | 378033 | 364004 | 87918860.444 |
| DirectUpdate | BaseLock | 1 | 36744 | 9155 | 109804586.625 |
| DirectUpdate | BaseLock | 2 | 129259 | 18577 | 107730898.887 |
| DirectUpdate | BaseLock | 4 | 538649 | 66180 | 60503682.809 |
| DirectUpdate | BaseLock | 8 | 1859924 | 125199 | 63998495.178 |
| DirectUpdate | BaseLock | 16 | 5621069 | 345279 | 46344614.525 |
| DirectUpdate | BaseLock | 32 | 19835021 | 709042 | 45138922.984 |
| DirectUpdate | DoubleCheckLock | 1 | 38873 | 9885 | 101904963.033 |
| DirectUpdate | DoubleCheckLock | 2 | 196382 | 20317 | 98535759.899 |
| DirectUpdate | DoubleCheckLock | 4 | 485040 | 66619 | 60058378.302 |
| DirectUpdate | DoubleCheckLock | 8 | 1319656 | 114590 | 69823752.217 |
| DirectUpdate | DoubleCheckLock | 16 | 3071997 | 236121 | 67764005.430 |
| DirectUpdate | DoubleCheckLock | 32 | 7953574 | 490998 | 65194635.929 |
| DirectUpdate | DoubleCheckLock2 | 1 | 33680 | 7643 | 130886458.291 |
| DirectUpdate | DoubleCheckLock2 | 2 | 73527 | 15338 | 130471282.322 |
| DirectUpdate | DoubleCheckLock2 | 4 | 306222 | 57992 | 69022139.598 |
| DirectUpdate | DoubleCheckLock2 | 8 | 923175 | 124348 | 64379810.884 |
| DirectUpdate | DoubleCheckLock2 | 16 | 2132856 | 285065 | 56159989.809 |
| DirectUpdate | DoubleCheckLock2 | 32 | 3946932 | 600016 | 53591131.794 |
| DirectUpdate | StdMutex | 1 | 46825 | 10901 | 92523107.858 |
| DirectUpdate | StdMutex | 2 | 170973 | 20209 | 99045453.839 |
| DirectUpdate | StdMutex | 4 | 412710 | 76596 | 52254880.204 |
| DirectUpdate | StdMutex | 8 | 1010194 | 157206 | 51393694.045 |
| DirectUpdate | StdMutex | 16 | 2346189 | 352865 | 45707818.935 |
| DirectUpdate | StdMutex | 32 | 4414674 | 627780 | 51155037.525 |
| DirectUpdate | TicketLock | 1 | 38592 | 9952 | 100720349.095 |
| DirectUpdate | TicketLock | 2 | 171373 | 19843 | 100964049.740 |
| DirectUpdate | TicketLock | 4 | 560381 | 60625 | 66067406.160 |
| DirectUpdate | TicketLock | 8 | 1441045 | 110886 | 72157286.545 |
| DirectUpdate | TicketLock | 16 | 2630470975 | 432592 | 37089466.107 |
| DirectUpdate | TicketLock | 32 | 33248358707 | 877788 | 36983768.985 |
