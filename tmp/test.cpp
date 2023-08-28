#include <bits/stdc++.h>

int main()
{
    using std::cout;using std::endl;
    std::smatch results;
    std::regex regex(R"(%(\w{2,3}))");
    auto str=std::string("%r10");
    std::regex_match(str,results,regex);
    std::cout<<results.size()<<std::endl;
    for(auto& item:results) std::cout<<item<<std::endl;
}