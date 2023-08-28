#include "Asm.hpp"
#include <fstream>
#define SIZE 10000

using namespace std;

Byte dat[SIZE];

int main(int argn, char** argv)
{
    ifstream is("prog.asm");
    Y86_64Assembler(dat,SIZE,is);
    ofstream out("out",ios::out|ios::binary);
    out.write((char *)dat,SIZE);
    is.close();out.close();
    return 0;
}