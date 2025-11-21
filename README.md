# RISC-V Processor Pipeline & Cache Simulation Suite

This project is a comprehensive simulation suite for a 32-bit RISC-V architecture. It includes a functional **5-Stage Pipelined Processor** and a configurable **Set-Associative Cache Memory System**.

## 📂 Project Structure

| File | Status | Description |
| :--- | :--- | :--- |
| **`final.cpp`** | ✅ Ready | Core 5-Stage Pipeline Simulator with integrated assembler. |
| **`cache.cpp`** | ✅ Ready | Standard Cache Simulator (Random Eviction, Write-Through). |
| **`assembler.cpp`** | ✅ Ready | Standalone Utility to convert Assembly to Machine Code. |
| **`cache_lru.cpp`** | 🛠️ **Setup Req.** | Advanced Cache (LRU, Write-Back). *See instructions below.* |
| **`trace_gen.cpp`** | 🛠️ **Setup Req.** | Synthetic Trace Generator. *See instructions below.* |

---

## 1️⃣ 5-Stage Pipeline CPU (`final.cpp`)

A functional simulator for the RISC-V RV32I (plus M-extension) instruction set.

* **5 Stages:** Instruction Fetch, Decode, Execute, Memory Access, Write Back.
* **Hazard Handling:** Automatically inserts **Stalls** for data hazards and **Flushes** for control hazards.

### 🚀 How to Run
```bash
g++ final.cpp -o cpu_sim
./cpu_sim
Usage: Enter the number of lines of assembly code, then paste your instructions (e.g., ADD x1, x2, x3).

2️⃣ Standard Cache Simulator (cache.cpp)
The default cache simulator configuration.

Config: 32KB Size, 8-way Associativity, 64B Blocks.

Policies: Random Eviction, Write-Through (Hit), Write-No-Allocate (Miss).

Traffic: Internal Normal Distribution generator.

🚀 How to Run
Bash

g++ cache.cpp -o cache_sim
./cache_sim
Runs for 100M iterations or 15 seconds.

3️⃣ Advanced Cache: LRU & Write-Back
This version implements Least Recently Used (LRU) replacement and Write-Back/Write-Allocate policies.

🛠️ Setup & Execution
Create a new file named cache_lru.cpp.

Copy the code from the "LRU Implementation Code" section below (click to expand).

Paste it into cache_lru.cpp.

<details> <summary><b>⬇️ CLICK HERE to copy LRU Implementation Code</b></summary>

C++

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
                if (type == 'R') {
                   // cout << "Read hit." << endl;
                } else if (type == 'W') {
                    block.data[wordOffsetInBlock] = writedata;
                    block.state = Dirty;
                   // cout << "Write hit. Marked Dirty." << endl;
                }
            }
        } else {
            // cout << "Cache miss." << endl;
            int evictionIndex = -1;
            vector<int> invalidIndices;
            for (int i = 0; i < ASSOCIATIVITY; ++i) {
                if (cacheSet.blocks[i].state == Invalid) invalidIndices.push_back(i);
            }
            if (!invalidIndices.empty()) {
                evictionIndex = invalidIndices[rand() % invalidIndices.size()];
            } else {
                evictionIndex = cacheSet.getLRUEvictionIndex();
                if (cacheSet.blocks[evictionIndex].state == Dirty) {
                   // cout << "Evicting Dirty Block (Write-Back)." << endl;
                }
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
</details>

Run the code:

Bash

g++ cache_lru.cpp -o lru_sim
./lru_sim
4️⃣ Trace-Based Simulation
To simulate using a pre-generated list of addresses (Trace File) instead of random generation.

🛠️ Setup & Execution
Step A: Generate Trace File

Create a file trace_gen.cpp.

Copy the code below (Source 77-93 from docs) and paste it into the file.

Run it to create addresses.txt.

<details> <summary><b>⬇️ CLICK HERE for Trace Generator Code</b></summary>

C++

#include <bits/stdc++.h>
using namespace std;
// ... (Paste content from Source 77-93 in provided docs) ...
// *Note: Ensure you copy the full generate_address1/2/3 logic and main function.*
</details>

Step B: Run Trace Simulation

Create a file trace_sim.cpp.

Copy the code below (Source 94-143 from docs) and paste it.

This code reads addresses.txt and simulates the cache.

<details> <summary><b>⬇️ CLICK HERE for Trace Simulator Code</b></summary>

C++

#include <bits/stdc++.h>
using namespace std;
// ... (Paste content from Source 94-143 in provided docs) ...
</details>

Bash

g++ trace_gen.cpp -o trace_gen
./trace_gen  # Creates addresses.txt
g++ trace_sim.cpp -o trace_sim
./trace_sim  # Runs simulation
5️⃣ Standalone Assembler (assembler.cpp)
A utility to convert RISC-V assembly text directly into 32-bit binary strings.

🚀 How to Run
Bash

g++ assembler.cpp -o assembler
./assembler
