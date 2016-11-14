#include <algorithm>
#include <cstddef>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <vector>

using namespace std;

char hex_to_int(char);

char hex_to_int(char hex) {
    if ('0' <= hex && hex <= '9') {
        return hex - '0';
    }
    if ('a' <= hex && hex <= 'z') {
        return hex - 'a' + 10;
    }
    if ('A' <= hex && hex <= 'Z') {
        return hex - 'A' + 10;
    }
    return 0;
}

int
main(int argc, char** argv) {

    if (2 != argc) {
        cout << "usage: " << argv[0] << " <output>\n";
        return 1;
    }

    ofstream output;
    output.open(argv[1], ofstream::out | ofstream::binary);
    if (! output.is_open()) {
        cout << "no such file " << argv[1] << "\n";
        return 1;
    }

    const size_t raw_buffer_size = 1023;
    const size_t hex_buffer_size = raw_buffer_size + 1;
    char raw_buffer[raw_buffer_size];
    char hex_buffer[hex_buffer_size];
    uint64_t read_index = 0;
    uint64_t write_index = 0;

    do {
        cin.read(raw_buffer, raw_buffer_size);
        for (int i = 0; i < cin.gcount(); i += 1) {
            if (!isalnum(raw_buffer[i])) {
                continue;
            }
            hex_buffer[write_index % hex_buffer_size] = raw_buffer[i];
            // size_t could overflow, but oh well. Don't apply the mod here to
            // make reading easier.
            write_index += 1;
        }
        // This is gonna be slow.
        while (read_index < write_index - 1) {
            char hi = hex_to_int(hex_buffer[read_index % hex_buffer_size]);
            read_index += 1;
            char lo = hex_to_int(hex_buffer[read_index % hex_buffer_size]);
            read_index += 1;
            char byte = (char) ((hi << 4) | lo);
            output.write(&byte, 1);
        }
    } while (cin.good());

    output.close();
    return 0;
}
