<h1>🏭 Factory Manager: Multithreaded Production Line Simulator [C Project]</h1>
[YouTube Demo & Output Samples]()

<h2>📝 Description</h2>
Factory Manager is a multithreaded production simulation in C for Linux, developed as part of an Operating Systems lab. It simulates a factory where a main manager coordinates multiple production lines (each with a producer-consumer pair) using POSIX threads, mutexes, condition variables, and semaphores. The objective is to master synchronization, concurrency control, and inter-thread communication using shared circular buffers.
<br><br> 
Each line includes a dedicated buffer (transport belt) where products are created and consumed concurrently by lightweight processes. 

<h2>🛠️ Languages and Utilities Used </h2>

- <b>C</b>
- <b>GCC</b> 
- <b>POSIX Threads</b>
- <b>Semaphores</b>
- <b>Mutexes and condition variables</b>
- <b>Linux</b>

<h2>📷 Output Examples</h2>

<p align="center"> 
Factory manager launching process managers: <br/> 
<img src="https://i.imgur.com/example_factory1.png" width="60%" alt="Factory launch"/> 
<br /><br /> 
Producer and consumer using transport belt: <br/> 
<img src="https://i.imgur.com/example_factory2.png" width="60%" alt="Producer consumer"/> 
<br /><br /> 
Queue operations: <br/> <img src="https://i.imgur.com/example_factory3.png" width="60%" alt="Queue ops"/> 
</p> 

<h2>📌 Features</h2>

- Reads and validates an input script defining factory structure.
- Factory Manager:
  - Loads configuration from file.
  - Manages synchronization and order of execution using semaphores.
  - Creates multiple Process Managers (one per belt).
- Process Manager:
  - Launches two threads (producer & consumer).
  - Handles shared buffer (circular queue) with full/empty checks.
- Queue implemented with:
  - Mutexes and condition variables.
  - Blocking on full/empty state for safe concurrent access.
- Precise logging: standardized [OK] and [ERROR] messages for traceability.
- Fault-tolerant: process_manager errors don’t halt factory_manager.

<h2>📽️ Usage Instructions</h2>

1. Clone or Download the Project
2. Compile using Make:
```bash
make all      # builds factory_manager and required files
```
3. Prepare an input script (example: input.txt) with format:
```
<Max number of belts> [<Belt ID> <Belt size> <Number of elements>]...
```
4. Execute the simulation:
```bash
./factory_manager input.txt
```
5. Clean binaries:
```bash
make clean
```

<h2>🧑‍💻 Authors</h2>

- Susana Ye Zhan
- Xinyi Yewu
```
This project is for educational purposes only. All rights reserved to Universidad Carlos III de Madrid.
```
