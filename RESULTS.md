# Lottery Scheduler Results (xv6)

## Setup
- Kernel: xv6 with lottery scheduling over RUNNABLE processes.
- Tickets: each process has `tickets >= 1` (default initialized to 1).
- Syscall: `settickets(n)` updates the calling process tickets; returns 0 on success and -1 if `n < 1`.
- Test program: `testlottery`
  - Validates `settickets(0)` fails and `settickets(1)` succeeds.
  - For experiments: forks children, assigns tickets, runs a CPU-bound loop for a fixed duration (in ticks), and prints `work` units completed per child.

## Workload
Each child:
1. Calls `settickets(Ti)`
2. Runs a tight CPU-bound loop for a fixed duration using `uptime()` ticks as the timer
3. Counts `work` units (loop iterations) as a proxy for CPU time
4. Prints `pid`, `tickets`, and `work`

Observed CPU share is estimated as:
- share_i = work_i / sum(work_all_children)

## Results

### Experiment 1: 3 children, tickets [10, 20, 70]
- Duration: 3000 ticks
- Expected shares: 10%, 20%, 70%

Observed work units:
- t=10: 185050
- t=20: 422571
- t=70: 1261044
Total work = 1868665

Observed shares:
- t=10: 185050 / 1868665 = 9.90%
- t=20: 422571 / 1868665 = 22.62%
- t=70: 1261044 / 1868665 = 67.48%

### Experiment 2: 4 children, tickets [1, 1, 1, 1]
- Duration: 3000 ticks
- Expected shares: 25%, 25%, 25%, 25%

Observed work units:
- 395718
- 435423
- 434518
- 436495
Total work = 1702154

Observed shares:
- 395718 / 1702154 = 23.25%
- 435423 / 1702154 = 25.58%
- 434518 / 1702154 = 25.53%
- 436495 / 1702154 = 25.64%

### Experiment 3: 2 children, tickets [1, 3]
- Duration: 2000 ticks
- Expected shares: 25%, 75%

Observed work units:
- t=1: 293741
- t=3: 878091
Total work = 1171832

Observed shares:
- t=1: 293741 / 1171832 = 25.07%
- t=3: 878091 / 1171832 = 74.93%

## Notes on variance and convergence
Lottery scheduling is probabilistic: at each scheduling decision, a RUNNABLE process is selected with probability proportional to its ticket count.
Over short runs, randomness introduces variance in observed CPU shares (e.g., in the equal-ticket experiment one process received ~23% in this run).
As run duration increases, the number of lottery draws increases and the observed shares converge toward the expected ticket ratios (law of large numbers).
