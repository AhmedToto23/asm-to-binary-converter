#include <iostream>
#include <string>
#include <bitset>
using namespace std;

int main() {
    // Conversion table mapping assembly instructions to their hexadecimal opcodes
    // Direct instructions (e.g., LDA) and indirect instructions (e.g., LDA I) are included
    // Indirect opcodes have bit 15 set (e.g., LDA I = 0x8002)
    string convert[] = {
        "AND 0",    // Direct AND: 0000 xxxx xxxx xxxx
        "ADD 1",    // Direct ADD: 0001 xxxx xxxx xxxx
        "LDA 2",    // Direct LDA: 0010 xxxx xxxx xxxx
        "STA 3",    // Direct STA: 0011 xxxx xxxx xxxx
        "BUN 4",    // Direct BUN: 0100 xxxx xxxx xxxx
        "BSA 5",    // Direct BSA: 0101 xxxx xxxx xxxx
        "ISZ 6",    // Direct ISZ: 0110 xxxx xxxx xxxx
        "AND I 8000", // Indirect AND: 1000 xxxx xxxx xxxx
        "ADD I 8001", // Indirect ADD: 1000 xxxx xxxx xxxx
        "LDA I 8002", // Indirect LDA: 1000 xxxx xxxx xxxx
        "STA I 8003", // Indirect STA: 1000 xxxx xxxx xxxx
        "BUN I 8004", // Indirect BUN: 1000 xxxx xxxx xxxx
        "BSA I 8005", // Indirect BSA: 1000 xxxx xxxx xxxx
        "ISZ I 8006", // Indirect ISZ: 1000 xxxx xxxx xxxx
        "CLA 7800", // Clear Accumulator
        "CLE 7400", // Clear E register
        "CMA 7200", // Complement Accumulator
        "CME 7100", // Complement E register
        "CIR 7080", // Circulate Right
        "CIL 7040", // Circulate Left
        "INC 7020", // Increment Accumulator
        "SPA 7010", // Skip if Positive
        "SNA 7008", // Skip if Negative
        "SZA 7004", // Skip if Zero
        "SZE 7002", // Skip if E is Zero
        "HLT 7001", // Halt
        "INP F800", // Input
        "OUT F400", // Output
        "SKI F200", // Skip on Input
        "SKO F100", // Skip on Output
        "ION F080", // Interrupt On
        "IOF F040"  // Interrupt Off
    };

    // Location counter to track memory addresses
    int lc = 0;
    // Sample assembly program to convert to binary
    string code[] = {
        "ORG 100",    // Set origin to address 100
        "LDA I SUB",  // Load accumulator indirectly via SUB (test indirect addressing)
        "CMA",        // Complement accumulator
        "INC",        // Increment accumulator
        "ADD MIN",    // Add MIN to accumulator
        "STA DIF",    // Store accumulator to DIF
        "HLT",        // Halt program
        "MIN, DEC 83",// Define MIN as decimal 83
        "SUB, DEC -23",// Define SUB as decimal -23
        "DIF, HEX 0", // Define DIF as hexadecimal 0
        "END"         // End of program
    };

    // Calculate sizes of arrays
    int codeSize = sizeof(code) / sizeof(code[0]);
    int convertSize = sizeof(convert) / sizeof(convert[0]);
    // Arrays to store symbol table (labels and their addresses)
    string symbol[10];
    int address[10];
    int symbolCount = 0;

    // Pass 1: Build symbol table by assigning addresses to labels
    for (int i = 0; i < codeSize; i++) {
        // Check for ORG directive to set location counter
        if (code[i].find("ORG") != string::npos) {
            lc = 100; // Set starting address to 100
            continue;
        }
        // Stop processing at END directive
        if (code[i] == "END") {
            break;
        }
        // Check for labels (indicated by comma)
        int commaPos = code[i].find(',');
        if (commaPos != string::npos) {
            // Extract label before comma
            string label = code[i].substr(0, commaPos);
            symbol[symbolCount] = label;
            address[symbolCount] = lc;
            symbolCount++;
        }
        lc++; // Increment location counter for next instruction
    }

    // Output symbol table
    cout << "Symbol Table:" << endl;
    for (int i = 0; i < symbolCount; i++) {
        // Print label and its address in hexadecimal
        cout << symbol[i] << ": " << hex << uppercase << address[i] << endl;
    }

    // Pass 2: Generate machine code
    cout << "\nMachine Code:" << endl;
    lc = 100; // Reset location counter to starting address
    for (int i = 0; i < codeSize; i++) {
        // Skip ORG and END directives
        if (code[i].find("ORG") != string::npos || code[i] == "END") {
            continue;
        }

        // Handle data definitions (DEC, HEX)
        int commaPos = code[i].find(',');
        if (commaPos != string::npos) {
            int decPos = code[i].find("DEC");
            int hexPos = code[i].find("HEX");
            if (decPos != string::npos) {
                // Convert decimal value to 16-bit binary
                int value = stoi(code[i].substr(decPos + 4));
                bitset<16> binary(value);
                cout << hex << uppercase << lc++ << ": " << binary << endl;
                continue;
            } else if (hexPos != string::npos) {
                // Convert hexadecimal value to 16-bit binary
                int value = stoi(code[i].substr(hexPos + 4), nullptr, 16);
                bitset<16> binary(value);
                cout << hex << uppercase << lc++ << ": " << binary << endl;
                continue;
            }
        }

        // Process instructions
        for (int j = 0; j < convertSize; j++) {
            // Extract opcode and value from conversion table
            int spacePos = convert[j].find(' ');
            string opp = convert[j].substr(0, spacePos);
            string value = convert[j].substr(spacePos + 1);
            // Check if current instruction matches opcode
            if (code[i].find(opp) != string::npos) {
                int fvalue = stoi(value, nullptr, 16);
                // Handle memory-reference instructions (those with operands)
                int spaceCode = code[i].find(' ');
                if (spaceCode != string::npos && opp != "CLA" && opp != "CLE" && opp != "CMA" && opp != "CME" &&
                    opp != "CIR" && opp != "CIL" && opp != "INC" && opp != "SPA" && opp != "SNA" &&
                    opp != "SZA" && opp != "SZE" && opp != "HLT" && opp != "INP" && opp != "OUT" &&
                    opp != "SKI" && opp != "SKO" && opp != "ION" && opp != "IOF") {
                    // Extract label after space
                    string label = code[i].substr(spaceCode + 1);
                    for (int k = 0; k < symbolCount; k++) {
                        if (label == symbol[k]) {
                            // Combine opcode (bits 12-15) with address (bits 0-11)
                            fvalue = (fvalue & 0xF000) | address[k];
                            break;
                        }
                    }
                }
                // Output instruction as 16-bit binary
                bitset<16> binary(fvalue);
                cout << hex << uppercase << lc++ << ": " << binary << endl;
                break;
            }
        }
    }

    return 0;
}
