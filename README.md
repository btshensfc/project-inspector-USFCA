
# Project 1: System Inspector

Writer: Porfirio Mohabir
University of San Francisco
Operating Systems

See: https://www.cs.usfca.edu/~mmalensek/cs326/assignments/project-1.html 

To compile and run:

```bash
make
./inspector
```

## Testing

To execute the test cases, use `make test`. To pull in updated test cases, run `make testupdate`. You can also run a specific test case instead of all of them:

```
# Run all test cases:
make test

# Run a specific test case:
make test run=4

# Run a few specific test cases (4, 8, and 12 in this case):
make test run='4 8 12'

```
# Project Inpector

The Insector Project creates a reports on user's system and display's system/hardware information in a very easy to read format. The programs uses system calls to read/open files and uses tokenization to retrieve certain data within the file. 


# Program Output:

```

Hostname: bloop
Kernel Version: 5.2.9-arch1-1-ARCH
Uptime: 25 days, 6 hours, 55 minutes, 13 seconds 
Hardware Information
------------------
CPU Model: AMD EPYC Processor (with IBPB)
Processing Units: 2
Load Average (1/5/15 min): 0.01 0.02 0.01 
CPU Usage:	[--------------------] 0.0% 
Memory Usage: [###-----------------] 18.8% (0.2 GB / 1.0 GB)
Task Information
------------------
Tasks Running: 96
  PID |        State |                 Task Name |            User | Tasks 
------+--------------+---------------------------+-----------------+-------
    1 |     sleeping |                   systemd |            root | 1 
    2 |     sleeping |                  kthreadd |            root | 1 
    3 |         idle |                    rcu_gp |            root | 1 
    4 |         idle |                rcu_par_gp |            root | 1 
    6 |         idle |      kworker/0:0H-kblockd |            root | 1 
    8 |         idle |              mm_percpu_wq |            root | 1 
    9 |     sleeping |               ksoftirqd/0 |            root | 1 
   10 |         idle |               rcu_preempt |            root | 1 
   11 |     sleeping |                    rcuc/0 |            root | 1 
...

```






