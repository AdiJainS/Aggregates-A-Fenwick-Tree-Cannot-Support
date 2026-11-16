# Cache profiling

WPR reported `CacheMisses`, `LLCReference` and `LLCMisses` as available sources on this machine; see `available-counters.txt`. The first attempt failed because the session lacked a required privilege; see `attempt.txt`. Running WPR as administrator produced traces for A and B.

For the workload shown below, `tracerpt` exported the traces to CSV. Filtering `PmcCounterProf` events for source 10 (`CacheMisses`) by the benchmark process's thread IDs and their lifetimes gave 1,587 samples for A and 6,412 for B. Both benchmark output files contain 20 runs and the same workload checksum. The trace summaries reported no lost ETW events.

These are samples from one recording per structure, not exact cache-miss counts or rates. They include workload generation and tree construction as well as benchmark operations. A and B also differ in their instrumentation, and A uses a provisional implementation. Repeat the recordings on final team code before drawing conclusions.

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

WPR traces can include system-wide process information. ETL files in this folder are ignored by Git; review them before sharing. The A recording containing the benchmark is `a-cache-run2.etl`; the earlier `a-cache.etl` was stopped before the benchmark ran.

Reference: [Microsoft: Recording hardware performance events](https://learn.microsoft.com/en-us/windows-hardware/test/wpt/recording-pmu-events).
