# Cache profiling

WPR reported `CacheMisses`, `LLCReference` and `LLCMisses` as available sources on this machine; see `available-counters.txt`. A hardware-counter recording was attempted with `bench/cache_profile.wprp`, but WPR returned `0xd0000061`: a required privilege was not held. No cache trace was collected. `attempt.txt` contains the command and error.

To try the recording, open PowerShell as administrator in the project folder. First build the maximum benchmark as described in `bench/README.md`. The examples record A and B in separate processes with the same workload. Check each WPR start succeeds before running the benchmark. Use a new output file name for each trace.

```powershell
wpr -status -instancename D6Cache
wpr -start "$PWD\bench\cache_profile.wprp!Cache" -filemode -instancename D6Cache
.\.bench-deps\max_benchmark.exe 1000000 1000000 1024 50 253 20 random time-a 50 | Set-Content -Encoding utf8 results\cache\a-profile-run.csv
wpr -stop "$PWD\results\cache\a-cache.etl" -instancename D6Cache

wpr -start "$PWD\bench\cache_profile.wprp!Cache" -filemode -instancename D6Cache
.\.bench-deps\max_benchmark.exe 1000000 1000000 1024 50 253 20 random time-b 50 | Set-Content -Encoding utf8 results\cache\b-profile-run.csv
wpr -stop "$PWD\results\cache\b-cache.etl" -instancename D6Cache
```

The WPR profile samples the Windows `CacheMisses` source once per 65,536 events. Open the ETL files in Windows Performance Analyzer, filter to `max_benchmark.exe`, and report the sampled event type and sample counts along with the settings. Samples are not exact cache-miss totals or a cache-miss rate. The trace also includes workload generation and tree construction, plus activity from other processes, so filter carefully and state those limits. Do not use timings gathered during profiling as the normal benchmark timings.

WPR traces can include system-wide process information. ETL files in this folder are ignored by Git; review them before sharing. Until usable profiler output has been inspected, the report makes no cache-performance claim.

Reference: [Microsoft: Recording hardware performance events](https://learn.microsoft.com/en-us/windows-hardware/test/wpt/recording-pmu-events).
