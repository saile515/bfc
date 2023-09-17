#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

unsigned int last_scope = 0;
unsigned int next_scope() { return ++last_scope; }

class Scope {
  public:
    Scope(std::string &file, int &index) : file(file), index(index) {
        scope_id = next_scope();
        init();
    }
    std::stringstream output;
    unsigned int scope_id;

  private:
    std::string &file;
    int &index;
    bool end_of_scope = false;

    void init() {
        do {
            index++;
            step();
        } while (index < file.length() && !end_of_scope);
    }

    void step() {
        char cmd = file[index];
        if (cmd == '>') {
            output << "    inc qword [DataPointer]\n";
        } else if (cmd == '<') {
            output << "    dec qword [DataPointer]\n";
        } else if (cmd == '+') {
            output << "    mov rax, qword [DataPointer]\n"
                   << "    inc byte [Data+rax]\n";
        } else if (cmd == '-') {
            output << "    mov rax, qword [DataPointer]\n"
                   << "    dec byte [Data+rax]\n";
        } else if (cmd == '.') {
            output << "    mov rax, qword [DataPointer]\n"
                   << "    mov bl, byte [Data+rax]\n"
                   << "    push rbx\n"
                   << "    mov rax, 1\n"
                   << "    mov rdi, 1\n"
                   << "    mov rsi, rsp\n"
                   << "    mov rdx, 1\n"
                   << "    syscall\n"
                   << "    add rsp, 8\n";
        } else if (cmd == ',') {
            output << "    mov rax, 0\n"
                   << "    mov rdi, 0\n"
                   << "    mov rbx, qword [DataPointer]\n"
                   << "    lea rsi, qword [Data+rbx]\n"
                   << "    mov rdx, 1\n"
                   << "    syscall\n";
        } else if (cmd == '[') {
            Scope scope(file, index);
            output << "    mov rax, qword [DataPointer]\n"
                   << "    mov bl, byte [Data+rax]\n"
                   << "    cmp bl, 0\n"
                   << "    je end" << scope.scope_id << "\n"
                   << "    jmp start" << scope.scope_id << "\n"
                   << "start" << scope.scope_id << ":\n"
                   << scope.output.str();
        } else if (cmd == ']') {
            output << "    mov rax, qword [DataPointer]\n"
                   << "    mov bl, [Data+rax]\n"
                   << "    cmp bl, 0\n"
                   << "    ja start" << scope_id << "\n"
                   << "    jmp end" << scope_id << "\n"
                   << "end" << scope_id << ":\n";
            end_of_scope = true;
        }
    }
};

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

    std::stringstream output;
    output << "section .data\n"
           << "DataPointer dq 0\n"
           << "Data times 30000 db 0\n"
           << "section .text\n"
           << "global _start\n"
           << "_start:\n";

    int index = -1;

    Scope global_scope(file, index);
    output << global_scope.output.str();

    output << "    mov rax, 60\n"
           << "    mov rdi, 0\n"
           << "    syscall\n";

    std::ofstream output_file("build/out.asm");
    output_file << output.rdbuf();
    output_file.close();

    return 0;
}
