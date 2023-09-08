#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char **argv) {

    if (argc != 2) {
        std::cout << "Wrong usage\n";
        return 1;
    }

    if (!std::filesystem::exists(argv[1])) {
        std::cout << "File does not exist\n";
        return 1;
    }

    std::stringstream file_stream;
    std::ifstream file_input(argv[1]);
    file_stream << file_input.rdbuf();
    std::string file = file_stream.str();

    std::cout << file << "\n";

    std::stringstream output;
    output << "section .data\n"
           << "DataPointer dw 0\n"
           << "Data db 0\n"
           << "section .text\n"
           << "global _start\n"
           << "_start:\n";

    int index = 0;

    while (index < file.length()) {
        char cmd = file[index];
        if (cmd == '>') {
            output << "    add DataPointer, 1\n";
        } else if (cmd == '<') {
            output << "    sub DataPointer, 1\n";
        } else if (cmd == '+') {
            output << "    mov rbx, [DataPointer]\n"
                   << "    mov al, [Data+rbx]\n"
                   << "    inc al\n"
                   << "    mov [Data+rbx], al\n";
        } else if (cmd == '-') {
            output << "    mov rbx, [DataPointer]\n"
                   << "    mov al, [Data+rbx]\n"
                   << "    dec al\n"
                   << "    mov [Data+rbx], al\n";
        } else if (cmd == '.') {
            output << "    mov rbx, [DataPointer]\n"
                   << "    mov al, [Data+rbx]\n"
                   << "    push rax\n"
                   << "    mov rax, 1\n"
                   << "    mov edi, 1\n"
                   << "    mov rsi, rsp\n"
                   << "    mov rdx, 1\n"
                   << "    syscall\n"
                   << "    add rsp, 8\n";
        } else if (cmd == ',') {
            output << "    mov rax, 0\n"
                   << "    mov rdi, 0\n"
                   << "    mov rbx, [DataPointer]\n"
                   << "    mov rsi, [Data+rbx]\n"
                   << "    mov rdx, 1\n"
                   << "    syscall\n";
        }
        index++;
    }

    std::ofstream output_file("build/out.asm");
    output_file << output.rdbuf();
    output_file.close();

    return 0;
}
