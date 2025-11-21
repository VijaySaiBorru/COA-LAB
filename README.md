# 🖥️ Computer Organization & Architecture (COA) Lab

This repository contains a comprehensive simulation suite for a **RISC-V Processor** and a **CPU Cache Memory System**. It allows for low-level analysis of instruction execution, pipeline hazards, and memory access patterns.

## 📂 Repository Contents

| File | Status | Description |
| :--- | :--- | :--- |
| **`final.cpp`** | ✅ **Included** | **5-Stage Pipeline Simulator** (IF, ID, EX, MEM, WB) with integrated Assembler. |
| **`cache.cpp`** | ✅ **Included** | **Standard Cache Simulator** (Random Eviction, Write-Through). |
| **`assembler.cpp`** | ✅ **Included** | **Assembler Utility** to convert RISC-V Assembly to Machine Code. |
| **`cache_lru.cpp`** | 📝 *See Below* | **Advanced Cache** (LRU, Write-Back) - *Code provided in README*. |
| **`trace_gen.cpp`** | 📝 *See Below* | **Trace Generator** for synthetic testing - *Code provided in README*. |

---

## 1️⃣ 5-Stage Pipeline CPU (`final.cpp`)

A fully functional simulator for the RISC-V RV32I (plus M-extension) instruction set.

### Features
* **Pipeline Stages:** Instruction Fetch, Decode, Execute, Memory Access, Write Back.
* **Hazard Handling:**
    * **Data Hazards:** Automatically detects dependencies and inserts **Stalls**.
    * **Control Hazards:** Handles Branch/Jump instructions by **Flushing** the pipeline.
* **Input:** Accepts assembly code (e.g., `ADD x1, x2, x3`), assembles it internally, and executes it.

### 🚀 How to Run
```bash
g++ final.cpp -o cpu_sim
./cpu_sim
```
**Usage:**
1. Enter the number of lines of assembly code.
2. Paste your assembly instructions.
3. View the cycle-by-cycle pipeline state.
   
**[RISC-V CPU Visualizer](https://risc-v-cpu-visualizer.vercel.app/)**

---

## 2️⃣ Standard Cache Simulator (`cache.cpp`)

Simulates a **Set-Associative Cache** to analyze hit/miss rates under different traffic conditions.

* **Configuration:** 32 KB Size, 8-way Associativity, 64-Byte Blocks.
* **Policies:** Random Eviction, Write-Through (Hit), Write-No-Allocate (Miss).
* **Traffic:** Uses an internal Normal Distribution generator to simulate "Locality of Reference".

### 🚀 How to Run
```bash
g++ cache.cpp -o cache_sim
./cache_sim
```
*The simulation runs for 100 Million iterations or until a 15-second timeout is reached.*

---

## 3️⃣ Advanced Cache: LRU & Write-Back

To test **Least Recently Used (LRU)** eviction and **Write-Back** policies, you need to create a separate file using the code below.

### 🛠️ Setup & Execution
1.  Create a file named **`cache_lru.cpp`**.
2.  Copy the code from the collapsible section below.
3.  Compile and run.

<details>
<summary><b>⬇️ CLICK HERE to copy LRU Cache Code</b></summary>

```cpp
#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;

#define CACHE_SIZE 32768 
#define BLOCK_SIZE 64 
#define ASSOCIATIVITY 8 
#define ADDRESS_SIZE 40 
#define WORD_SIZE 4 

enum State { Invalid, Valid, MissPending, Dirty };

class CacheBlock {
public:
    State state;
    int tag;
    int accessCounter;  
    vector<string> data;
    CacheBlock() : state(Invalid), tag(0), accessCounter(0) {
        data.resize(BLOCK_SIZE / WORD_SIZE, "");
    }
};

class CacheSet {
public:
    vector<CacheBlock> blocks;
    CacheSet() { blocks.resize(ASSOCIATIVITY); }

    int findTag(int tag) {
        for (int i = 0; i < ASSOCIATIVITY; ++i) {
            if (blocks[i].state != Invalid && blocks[i].tag == tag)
                return i;
        }
        return -1;
    }

    int getLRUEvictionIndex() {
        int lruIndex = 0;
        for (int i = 1; i < ASSOCIATIVITY; ++i) {
            if (blocks[i].accessCounter < blocks[lruIndex].accessCounter) {
                lruIndex = i;
            }
        }
        return lruIndex;
    }
};

class Cache {
public:
    int cacheSize, blockSize, associativity, numSets, indexBits, blockOffsetBits, tagBits;
    int totalAccesses, totalHits;
    vector<CacheSet> cacheSets;
    int accessCounter;

    Cache() {
        cacheSize = CACHE_SIZE;
        blockSize = BLOCK_SIZE;
        associativity = ASSOCIATIVITY;
        numSets = cacheSize / (blockSize * associativity);
        blockOffsetBits = log2(blockSize);
        indexBits = log2(numSets);
        tagBits = ADDRESS_SIZE - blockOffsetBits - indexBits;
        cacheSets.resize(numSets);
        totalAccesses = 0; totalHits = 0; accessCounter = 0;
    }

    void cache_details() {
        cout << "Cache Type: " << associativity << "-way Set-Associative (LRU)" << endl;
    }

    void cache_check_and_process(uint64_t address, char type, string writedata) {
        totalAccesses++;
        int offset = address & ((1 << blockOffsetBits) - 1);
        int index = (address >> blockOffsetBits) & ((1 << indexBits) - 1);
        int tag = (address >> (blockOffsetBits + indexBits)) & ((1 << tagBits) - 1);
        int wordOffsetInBlock = offset / WORD_SIZE;
        CacheSet& cacheSet = cacheSets[index];
        int blockIndex = cacheSet.findTag(tag);
        accessCounter++;

        if (blockIndex != -1 && cacheSet.blocks[blockIndex].state != Invalid) {
            CacheBlock& block = cacheSet.blocks[blockIndex];
            block.accessCounter = accessCounter;
            if (block.state == Valid || block.state == Dirty) {
                totalHits++;
                if (type == 'W') {
                    block.data[wordOffsetInBlock] = writedata;
                    block.state = Dirty;
                }
            }
        } else {
            int evictionIndex = -1;
            vector<int> invalidIndices;
            for (int i = 0; i < ASSOCIATIVITY; ++i) {
                if (cacheSet.blocks[i].state == Invalid) invalidIndices.push_back(i);
            }
            if (!invalidIndices.empty()) {
                evictionIndex = invalidIndices[rand() % invalidIndices.size()];
            } else {
                evictionIndex = cacheSet.getLRUEvictionIndex();
            }
            CacheBlock& newBlock = cacheSet.blocks[evictionIndex];
            newBlock.state = Valid;
            newBlock.tag = tag;
            newBlock.data = vector<string>(BLOCK_SIZE / WORD_SIZE, "LoadedData");
            newBlock.accessCounter = accessCounter;
            if (type == 'W') {
                newBlock.data[wordOffsetInBlock] = writedata;
                newBlock.state = Dirty;
            }
        }
    }

    void printHitRate() {
        cout << "Total Accesses: " << totalAccesses << ", Hits: " << totalHits << endl;
        cout << "Hit Rate: " << (100.0 * totalHits / totalAccesses) << "%" << endl;
    }
};

uint64_t generate_address() {
    static random_device rd;
    static mt19937 generator(rd());
    uint64_t mean = 1ULL << 39;
    double stddev = static_cast<double>(1ULL << 18) / 17;
    normal_distribution<double> dist(static_cast<double>(mean), stddev);
    uint64_t address;
    do {
        address = static_cast<uint64_t>(round(dist(generator)));
        address &= ~3;
    } while (address >= (1ULL << 40));
    return address;
}

int main() {
    srand(time(NULL));
    Cache cache;
    cache.cache_details();
    auto start_time = high_resolution_clock::now();
    for (int i = 0; i < 1e8; i++) {
        uint64_t address = generate_address();
        char type = (i % 3 == 0) ? 'W' : 'R';
        cache.cache_check_and_process(address, type, "Sent_Data");
        auto end_time = high_resolution_clock::now();
        if ((end_time - start_time).count() >= 15.0 * 1e9) break; 
    }
    cache.printHitRate();
    return 0;
}
```
</details>

```bash
g++ cache_lru.cpp -o lru_sim
./lru_sim
```

---

## 4️⃣ Trace-Based Simulation

To run reproducible tests using a static list of memory addresses (Trace File).

### Step A: Generate Trace (`trace_gen.cpp`)
Create a file named `trace_gen.cpp`, paste the code below, and run it to generate `addresses.txt`.

<details>
<summary><b>⬇️ CLICK HERE for Trace Generator Code</b></summary>

```cpp
#include <bits/stdc++.h>
using namespace std;

uint64_t generate_address(int shift) {
    static random_device rd;
    static mt19937 generator(rd()); 
    uint64_t mean = 1ULL << shift; 
    double stddev = static_cast<double>(1ULL << 12);
    normal_distribution<double> dist(static_cast<double>(mean), stddev);
    uint64_t address;
    do {
        address = static_cast<uint64_t>(round(dist(generator)));
        address &= ~3;
    } while (address >= (1ULL << 40));
    return address;
}

int main() {
    srand(time(NULL));
    ofstream outFile("addresses.txt");
    if (!outFile) return 1;
    for (int i = 0; i < 1e7; i++) { // 10 Million addresses
        int j = rand() % 3;
        uint64_t address = (j==1) ? generate_address(35) : (j==2) ? generate_address(25) : generate_address(20);
        outFile << address << endl;
    }
    outFile.close();
    cout << "Generated addresses.txt" << endl;
    return 0;
}
```
</details>

### Step B: Run Trace Simulator (`trace_sim.cpp`)
Create `trace_sim.cpp` with the code below to process the generated file.

<details>
<summary><b>⬇️ CLICK HERE for Trace Simulator Code</b></summary>

```cpp
#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;

#define CACHE_SIZE 32768 
#define BLOCK_SIZE 64 
#define ASSOCIATIVITY 8 
#define ADDRESS_SIZE 40 
#define WORD_SIZE 4 

enum State { Invalid, Valid, MissPending };

class CacheBlock {
public:
    State state;
    int tag;
    vector<string> data;
    CacheBlock() : state(Invalid), tag(0) {
        data.resize(BLOCK_SIZE / WORD_SIZE, "");
    }
};

class CacheSet {
public:
    vector<CacheBlock> blocks;
    CacheSet() { blocks.resize(ASSOCIATIVITY); }

    int findTag(int tag) {
        for (int i = 0; i < ASSOCIATIVITY; ++i) {
            if (blocks[i].state == Valid && blocks[i].tag == tag)
                return i;
        }
        return -1;
    }

    int getRandomEvictionIndex() {
        return rand() % ASSOCIATIVITY;
    }
};

class Cache {
public:
    int cacheSize, blockSize, associativity, numSets, indexBits, blockOffsetBits, tagBits;
    int totalAccesses, totalHits;
    vector<CacheSet> cacheSets;

    Cache() {
        cacheSize = CACHE_SIZE;
        blockSize = BLOCK_SIZE;
        associativity = ASSOCIATIVITY;
        numSets = cacheSize / (blockSize * associativity);
        blockOffsetBits = log2(blockSize);
        indexBits = log2(numSets);
        tagBits = ADDRESS_SIZE - blockOffsetBits - indexBits;
        cacheSets.resize(numSets);
        totalAccesses = 0; totalHits = 0;
    }

    void cache_check_and_process(uint64_t address, char type, string writedata) {
        totalAccesses++;
        int offset = address & ((1 << blockOffsetBits) - 1);
        int index = (address >> blockOffsetBits) & ((1 << indexBits) - 1);
        int tag = (address >> (blockOffsetBits + indexBits)) & ((1 << tagBits) - 1);
        
        CacheSet& cacheSet = cacheSets[index];
        int blockIndex = cacheSet.findTag(tag);

        if (blockIndex != -1) {
            totalHits++;
        } else {
            int evictionIndex = -1;
            vector<int> invalidIndices;
            for (int i = 0; i < ASSOCIATIVITY; ++i) {
                if (cacheSet.blocks[i].state == Invalid) invalidIndices.push_back(i);
            }
            if (!invalidIndices.empty()) {
                evictionIndex = invalidIndices[rand() % invalidIndices.size()];
            } else {
                evictionIndex = cacheSet.getRandomEvictionIndex();
            }
            CacheBlock& evictBlock = cacheSet.blocks[evictionIndex];
            evictBlock.state = Valid;
            evictBlock.tag = tag;
        }
    }

    void printHitRate() {
        cout << "Total Accesses: " << totalAccesses << ", Hits: " << totalHits << endl;
        cout << "Hit Rate: " << (100.0 * totalHits / totalAccesses) << "%" << endl;
    }
};

int main() {
    srand(time(NULL));
    Cache cache;
    ifstream inFile("addresses.txt");
    if (!inFile) { cout << "Error opening addresses.txt" << endl; return 1; }
    
    string line;
    int i = 0;
    auto start_time = high_resolution_clock::now();
    
    while (getline(inFile, line)) {
        uint64_t address = stoull(line);
        char type = (i % 3 == 0) ? 'W' : 'R';
        cache.cache_check_and_process(address, type, "Data");
        i++;
        if (i % 100000 == 0) { 
             auto end_time = high_resolution_clock::now();
             if ((end_time - start_time).count() >= 15.0 * 1e9) break;
        }
    }
    cache.printHitRate();
    return 0;
}
```
</details>

```bash
g++ trace_gen.cpp -o trace_gen
./trace_gen
g++ trace_sim.cpp -o trace_sim
./trace_sim
```

---

## 5️⃣ Standalone Assembler (`assembler.cpp`)

A utility to convert RISC-V assembly text directly into 32-bit binary strings.

### 🚀 How to Run
```bash
g++ assembler.cpp -o assembler
./assembler
```

---

## 🌐 Interactive Visualizer

If you have any doubts about how the pipeline flows or how instructions are processed step-by-step, you can use this helpful online tool:

👉 **[RISC-V CPU Visualizer](https://risc-v-cpu-visualizer.vercel.app/)**

This visualizer provides a visual representation of the datapath, which can clarify concepts used in the `final.cpp` simulator.
