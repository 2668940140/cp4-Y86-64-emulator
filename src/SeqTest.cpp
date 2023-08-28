#include <iostream>
#include <fstream>
#include "Seq.hpp"
#include "utilities.hpp"

#define SIZE 10000
Byte Mem[SIZE];
Byte Ins[SIZE];
Seq seq(Ins,Mem,SIZE);

using namespace std;

int main(int, char**){
    ifstream ifs("out",ios::in|ios::binary);
    ifs.read((char*)Ins,SIZE);
    ifs.close();
    write(Mem,Qword(10));
    seq.run();

    cout<<readQword(Mem)<<endl;
}
