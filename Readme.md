# PIM Compiler for SIMD Matrix Multiplication

## 📌 Project Overview
This project implements a **Processing-in-Memory (PIM) Compiler** that translates C++ programs into **pPIM ISA** for a DRAM-based SIMD architecture. The compiler processes **matrix multiplication** using **vectorized operations** to optimize memory access and computation.

## 🚀 Features
- **Matrix Multiplication using SIMD ISA**
- **Aligned Memory Mapping for PIM**
- **Custom Instruction Set (pPIM ISA)**
- **CSV-based Matrix Input Support**
- **Optimized Parallel Computation**

## 📂 Project Structure
```
PIMcompiler/
│── src/
│   ├── main.cpp          # Entry point for PIM compiler
│── data/
│   ├── matrix.csv        # Input file containing matrices
│── build/
│── README.md             # Project Documentation
```

## 🔧 Installation & Setup
### 1️⃣ **Install Dependencies**
Ensure you have a C++ compiler installed:
- **MinGW** (for Windows): [Download Here](https://www.mingw-w64.org/)
- **Microsoft Visual C++ (MSVC)** (for Windows)
- **GCC (Linux/Mac)**

### 2️⃣ **Clone the Repository**
```sh
git clone https://github.com/your-repo/PIMcompiler.git
cd PIMcompiler/src
```

### 3️⃣ **Compile the Program**
#### ➤ Using MinGW (Windows)
```sh
g++ main.cpp -o main.exe
```
#### ➤ Using MSVC (Windows)
```sh
cl main.cpp
```
#### ➤ Using GCC (Linux/Mac)
```sh
g++ main.cpp -o main
```

### 4️⃣ **Run the Program**
#### ➤ On Windows:
```sh
main.exe
```
#### ➤ On Linux/Mac:
```sh
./main
```

## 📄 Input Format (CSV File)
Your **`matrix.csv`** file should be formatted as:
```
1,2,3
4,5,6
7,8,9
---
9,8,7
6,5,4
3,2,1
```
- **First matrix (`A`)** is before `---`
- **Second matrix (`B`)** is after `---`


