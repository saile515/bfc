#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

unsigned int last_scope = 0;
unsigned int next_scope() { return ++last_scope; }

class Scope {
  public:
    Scope(std::string &file, unsigned int &index) : file(file), index(index) {
        scope_id = next_scope();
        init();
    }
    std::stringstream output;
    std::vector<Scope> sub_scopes;
    unsigned int scope_id;

  private:
    std::string &file;
    unsigned int &index;
    bool end_of_scope = false;

    void init() {
        do {
            step();
            index++;
        } while (index < file.length() && !end_of_scope);
    }

    void step() {
        char cmd = file[index];
        if (cmd == '>') {
            output << "    add DataPointer, 1\n";
        } else if (cmd == '<') {
            output << "    sub DataPointer, 1\n";
        } else if (cmd == '+') {
            output << "    mov rsi, [DataPointer]\n"
                   << "    mov al, [Data+rsi]\n"
                   << "    inc al\n"
                   << "    mov [Data+rsi], al\n";
        } else if (cmd == '-') {
            output << "    mov rsi, [DataPointer]\n"
                   << "    mov al, [Data+rsi]\n"
                   << "    dec al\n"
                   << "    mov [Data+rsi], al\n";
        } else if (cmd == '.') {
            output << "    mov rsi, [DataPointer]\n"
                   << "    mov al, [Data+rsi]\n"
                   << "    push rax\n"
                   << "    mov rax, 1\n"
                   << "    mov rdi, 1\n"
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
        } else if (cmd == '[') {
            index++;
            Scope scope(file, index);
            output << "    mov rsi, [DataPointer]\n"
                   << "    mov al, [Data+rsi]\n"
                   << "    cmp rax, 0\n"
                   << "    ja " << scope.scope_id << "\n";
            sub_scopes.push_back(std::move(scope));
        } else if (cmd == ']') {
            output << "    mov rsi, [DataPointer]\n"
                   << "    mov al, [Data+rsi]\n"
                   << "    cmp rax, 0\n"
                   << "    je " << scope_id << "\n"
                   << "    ret\n";
            end_of_scope = true;
        }
    }
};

void add_scope_to_output(std::stringstream &output, Scope &scope) {
    output << scope.scope_id << ":\n" << scope.output.str();
    for (Scope &sub_scope : scope.sub_scopes) {
        add_scope_to_output(output, sub_scope);
    }
}

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
           << "DataPointer dd 0\n"
           << "Data times 30000 db 0\n"
           << "section .text\n"
           << "global _start\n"
           << "_start:\n";

    unsigned int index = 0;

    Scope global_scope(file, index);
    output << global_scope.output.str();

    output << "    mov rax, 60\n"
           << "    mov rdi, 0\n"
           << "    syscall\n";

    for (Scope &scope : global_scope.sub_scopes) {
        add_scope_to_output(output, scope);
    }

    std::ofstream output_file("build/out.asm");
    output_file << output.rdbuf();
    output_file.close();

    return 0;
}
