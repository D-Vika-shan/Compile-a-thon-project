#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>
#include <iomanip>

using namespace std;

// Memory Layout Parameters
const int BASE_A = 0x00;
const int BASE_B = 0x20;
const int BASE_C = 0x40;
const int BANK_SIZE = 16;  // Assume DRAM Bank size

// SIMD ISA Opcodes
const int NO_OP = 0b00;
const int PROG  = 0b01;
const int EXE   = 0b10;
const int END   = 0b11;

// SIMD Vector Size
const int SIMD_WIDTH = 4;  // Processing 4 elements in parallel

// Function to map opcode to instruction name
string getOpcodeName(int opcode) {
    switch (opcode) {
        case NO_OP: return "NO_OP";
        case PROG:  return "PROG";
        case EXE:   return "EXE";
        case END:   return "END";
        default:    return "UNKNOWN";
    }
}

// Function to generate SIMD ISA instruction
string generateSIMD_ISA(int opcode, int corePtr, int rd, int wr, int rowAddr) {
    int instruction = (opcode << 17) | (corePtr << 11) | (rd << 10) | (wr << 9) | rowAddr;

    // Convert to hex format
    stringstream hexStream;
    hexStream << "0x" << setw(6) << setfill('0') << hex << uppercase << instruction;
    string instructionHex = hexStream.str();

    // Convert to binary format
    string instructionBinary = bitset<24>(instruction).to_string();

    // Print instruction details
    cout << "Opcode: " << bitset<2>(opcode) << " (" << getOpcodeName(opcode) << ")"
         << ", Core Ptr: " << bitset<6>(corePtr)
         << ", Rd: " << rd
         << ", Wr: " << wr
         << ", Row Address: " << bitset<9>(rowAddr)
         << " [" << instructionHex << "]"
         << " | Binary: " << instructionBinary
         << endl;

    return instructionHex;
}

// Aligned Memory Address Mapping
int getAlignedAddress(int base, int row, int col, int size) {
    int bank = ((row * size + col) / SIMD_WIDTH) * SIMD_WIDTH; // Aligning address
    return base + (row * size + col) + bank;
}

// Detect matrix size from CSV
int detectMatrixSize(string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Could not open file: " << filename << endl;
        exit(1);
    }

    string line;
    int rowCount = 0;
    
    while (getline(file, line)) {
        if (line.find("---") == string::npos)  // Ignore separator
            rowCount++;
    }

    file.close();
    return rowCount / 2;
}

// Read matrix from CSV
vector<vector<int>> readMatrixFromCSV(string filename, int size, int startLine) {
    ifstream file(filename);
    vector<vector<int>> matrix(size, vector<int>(size, 0));
    
    if (!file.is_open()) {
        cerr << "Could not open file: " << filename << endl;
        exit(1);
    }

    string line;
    int row = 0, lineNum = 0;
    
    while (getline(file, line)) {
        if (line.find("---") != string::npos) 
            continue;

        if (lineNum >= startLine && row < size) {
            stringstream ss(line);
            string val;
            int col = 0;
            while (getline(ss, val, ',') && col < size) {
                matrix[row][col] = stoi(val);
                col++;
            }
            row++;
        }
        lineNum++;
    }

    file.close();
    return matrix;
}

int main() {
    string filename = "C:\\Users\\devik\\Downloads\\matrix.csv";
 
    int matrixSize = detectMatrixSize(filename);

    cout << "Detected Matrix Size (N): " << matrixSize << endl;

    // Read matrices from CSV
    vector<vector<int>> A = readMatrixFromCSV(filename, matrixSize, 0);
    vector<vector<int>> B = readMatrixFromCSV(filename, matrixSize, matrixSize + 1);

    // 🔹 Program SIMD LUTs
    cout << "\nProgramming SIMD LUTs:\n";
    cout << generateSIMD_ISA(PROG, 1, 0, 1, 0) << "  ; PROGRAM LUT0 (SIMD Multiply)\n";
    cout << generateSIMD_ISA(PROG, 2, 0, 1, 0) << "  ; PROGRAM LUT1 (SIMD Add)\n";

    // 🔹 Load Matrices into SIMD Memory
    cout << "\nLoading Matrices into SIMD Memory:\n";
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j += SIMD_WIDTH) {  // Vectorized Load
            int addrA = getAlignedAddress(BASE_A, i, j, matrixSize);
            int addrB = getAlignedAddress(BASE_B, j, i, matrixSize);
            cout << generateSIMD_ISA(PROG, 0, 0, 1, addrA) << "  ; VLOAD A[" << i << "][" << j << "]\n";
            cout << generateSIMD_ISA(PROG, 0, 0, 1, addrB) << "  ; VLOAD B[" << j << "][" << i << "]\n";
        }
    }

    // 🔹 Optimized SIMD pPIM Matrix Multiplication
    cout << "\nGenerating Optimized SIMD pPIM ISA for Matrix Multiplication:\n";
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j += SIMD_WIDTH) {  // Vectorized computation
            int addrC = getAlignedAddress(BASE_C, i, j, matrixSize);
            cout << generateSIMD_ISA(PROG, 0, 0, 1, addrC) << "  ; INIT C[" << i << "][" << j << "]\n";

            for (int k = 0; k < matrixSize; k += SIMD_WIDTH) {  // Unrolling Loop
                int addrA1 = getAlignedAddress(BASE_A, i, k, matrixSize);
                int addrB1 = getAlignedAddress(BASE_B, k, j, matrixSize);

                cout << generateSIMD_ISA(PROG, 0, 1, 0, addrA1) << "  ; VLOAD A[" << i << "][" << k << "]\n";
                cout << generateSIMD_ISA(PROG, 0, 1, 0, addrB1) << "  ; VLOAD B[" << k << "][" << j << "]\n";
                cout << generateSIMD_ISA(EXE, 1, 1, 0, 0) << "  ; VMUL A, B -> C\n";
                cout << generateSIMD_ISA(EXE, 2, 1, 0, 0) << "  ; VADD C, prev_C\n";
            }

            cout << generateSIMD_ISA(PROG, 0, 0, 1, addrC) << "  ; VSTORE C[" << i << "][" << j << "]\n";
        }
    }

    cout << generateSIMD_ISA(END, 0, 0, 0, 0) << "  ; END PROGRAM\n";
    return 0;
}