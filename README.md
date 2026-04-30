# Memory-Efficient Versioned File Indexer

A C++ command-line tool that builds a **word-level frequency index** over large text files using a **fixed-size buffer**, so memory usage stays independent of file size.

Built as part of **CS253 Assignment 1**.

---

## Features

- **Buffered file reading** — processes files incrementally (256 KB – 1024 KB buffer), never loads the whole file
- **Word-level indexing** — case-insensitive, alphanumeric tokens only
- **Version management** — maintains separate indices per named version
- **Three query types:**
  - `word` — frequency of a word in a version
  - `top` — top-K most frequent words (descending)
  - `diff` — frequency difference of a word between two versions
- **OOP design** — abstract `Query` base class, derived `WordQuery` / `TopKQuery`, template utility
- **Exception handling** — runtime errors caught and reported cleanly

---

## Build

```bash
g++ -std=c++17 -O2 analyzer.cpp -o analyzer
```

---

## Usage

### Word frequency query
```bash
./analyzer --file dataset_v1.txt --version v1 \
           --buffer 512 --query word --word error
```

### Top-K query
```bash
./analyzer --file dataset_v1.txt --version v1 \
           --buffer 512 --query top --top 10
```

### Difference query (two files)
```bash
./analyzer --file1 dataset_v1.txt --version1 v1 \
           --file2 dataset_v2.txt --version2 v2 \
           --buffer 512 --query diff --word error
```

---

## Command-Line Options

| Flag | Description |
|------|-------------|
| `--file <path>` | Input file (single-version queries) |
| `--file1 <path>` | First file (diff query) |
| `--file2 <path>` | Second file (diff query) |
| `--version <name>` | Version label (single-version queries) |
| `--version1 <name>` | First version label (diff query) |
| `--version2 <name>` | Second version label (diff query) |
| `--buffer <kb>` | Buffer size in KB (256–1024) |
| `--query <type>` | `word` \| `top` \| `diff` |
| `--word <token>` | Word to look up |
| `--top <k>` | Number of top results |

---

## Output

Every run prints:
- Query result
- Allocated buffer size (KB)
- Total execution time (seconds)

---

## Design

```
Query (abstract base)
├── WordQuery   — handles word count + diff
└── TopKQuery   — handles top-K ranking

FileProcessor   — buffered reading, boundary-safe tokenization
Tokenizer       — normalises tokens (lowercase, alphanumeric only)
displayInfo<T>  — template utility for labelled output
```

---

## Constraints

- Buffer size: 256 KB ≤ size ≤ 1024 KB (constant throughout execution)
- Memory grows only with the number of unique words, not file size
- Tokens split across buffer boundaries are handled correctly

---

## Author

**Sneha Kumari** — Roll No. 241024  
IIT Kanpur · CS253 · 2026
