# MyVCS  
*A Git-like Version Control System in C*

**Author:** S Nikhil 
**Date:** October 4, 2025  

---

## 🧩 Abstract
**MyVCS** is a simplified command-line version control system inspired by **Git**.  
Written entirely in **C** for **Linux**, it aims to demystify distributed version control by recreating Git’s core operations from scratch.

The system supports:
- Repository initialization  
- Staging files  
- Committing snapshots  
- Viewing commit history  
- Restoring files from previous versions  

This project focuses on understanding **file system manipulation**, **hashing algorithms**, and **data modeling** within operating system contexts.

---

## 🚀 Introduction
Version Control Systems (VCS) are fundamental in modern software development, enabling collaboration and historical tracking.  
Git is widely used but internally complex — **MyVCS** aims to replicate its core logic to make these mechanisms transparent.

### Implemented Commands
| Command | Description |
|----------|-------------|
| `init` | Initialize a new empty repository |
| `add` | Stage files for the next commit |
| `commit` | Record a snapshot of staged files |
| `log` | Display commit history |
| `checkout` | Restore a file from a previous commit |

---

## 🏗️ System Architecture and Design

### Repository Structure
When you run `myvcs init`, the following structure is created:
```bash
.myvcs/
├── objects/ # Stores blobs, trees, and commits (by SHA-1 hash)
├── refs/ # Contains branch references (e.g., 'main')
│ └── main
├── HEAD # Points to the current branch (ref: refs/main)
└── index # Staging area mapping filenames to blob hashes
```

### Object Model
MyVCS stores data as content-addressable objects:

#### 🧱 Blob
- Stores raw file content without metadata.  
- Each blob’s hash acts as its unique identifier.

#### 🌳 Tree
- Represents a directory structure.  
- Each entry records file mode, object type (blob/tree), hash, and filename.

#### 🕒 Commit
- Connects the repository’s history by referencing:
  - Tree hash  
  - Parent commit hash  
  - Author and timestamp  
  - Commit message  

---

## ⚙️ Implementation Details

Each command is an independent C program using file I/O and **OpenSSL** for hashing.

### `myvcs_init`
- Uses `mkdir()` from `<sys/stat.h>` to create `.myvcs`, `.myvcs/objects`, `.myvcs/refs`.
- Initializes `HEAD` and `refs/main`.

### `myvcs_add <filename>`
- Reads file content.  
- Computes SHA-1 hash (via OpenSSL).  
- Stores blob in `.myvcs/objects/`.  
- Updates `.myvcs/index`.

### `myvcs_commit -m "<message>"`
1. Reads `.myvcs/index` and creates a tree object.  
2. Generates hash and stores it.  
3. Retrieves parent commit hash.  
4. Constructs and writes commit object (tree, parent, author, message).  
5. Updates `refs/main` and clears index.

### `myvcs_log`
- Reads `refs/main` to find the latest commit.  
- Prints commit info recursively until no parent exists.

### `myvcs_checkout <hash> <file>`
- Finds the commit’s tree and associated blob hash.  
- Restores file content from the blob object.

---

## 🧠 Challenges and Future Work

### Challenges
- String manipulation and memory management in C.  
- Integrating OpenSSL (`-lcrypto`) and converting binary hash to hexadecimal.

### Future Enhancements
- **Branching & Merging** — multiple branches and merge operations.  
- **Compression** — using `zlib` for object compression.  
- **Status Command** — compare working directory, index, and HEAD.  
- **Improved Index** — enable efficient file updates and removals.

---

## 🏁 Conclusion
The **MyVCS** project successfully replicates the foundational mechanics of Git, providing insight into distributed version control and C-level file management.

**Key Learnings:**
- Low-level file I/O and directory handling  
- Hashing and data storage models  
- Practical understanding of operating system principles  

---

## 🛠️ Build Instructions

### Prerequisites
- Linux environment  
- GCC compiler  
- OpenSSL library  

### Compilation Example
```bash
gcc myvcs_init.c -o myvcs_init
gcc myvcs_add.c -o myvcs_add -lcrypto
gcc myvcs_commit.c -o myvcs_commit -lcrypto
gcc myvcs_log.c -o myvcs_log
gcc myvcs_checkout.c -o myvcs_checkout

./myvcs_init
./myvcs_add file.txt
./myvcs_commit -m "Initial commit"
./myvcs_log
./myvcs_checkout <commit-hash> file.txt
```

---

## 📜 License

This project is open-source and available under the MIT License.

---

## 📚 References

Chacon, S., & Straub, B. (2014). Pro Git (2nd ed.). Apress.

Git Source Code – https://github.com/git/git

---

## 🧱 Keywords
`Version Control` · `C Programming` · `Git Clone` · `SHA-1` · `Linux` · `Filesystem`
