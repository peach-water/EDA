#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <string.h>
// using namespace std;

// 请注意这个函数是用来检查第三问是否正确的

int main(int argv, char **argc)
{
    std::unordered_map<std::string, std::unordered_set<std::string>> Supply_net, Supply_Port;
    std::fstream file;
    std::string correct_file_name;  // 正确答案文件名
    std::string myanswer_file_name; // 我的答案文件名
    // 注意，先输入我的答案文件名，再输入正确答案文件名
    correct_file_name = argc[2];
    myanswer_file_name = argc[1];

    file.open(myanswer_file_name, std::ios::in);

    // std::cout << myanswer_file_name << " " << correct_file_name << std::endl;

    if (!file.is_open())
    {
        std::cout << "file dont exist" << std::endl;
        return 0;
    }

    std::string temp; // 这是临时变量
    char name[20];
    char s[500];

    file >> temp;
    while (!file.eof())
    {

        file >> temp; // 这才是接口名字
        // std::cout << temp << std::endl;

        const std::string Port_name = temp;
        Supply_net[temp].clear();
        Supply_Port[temp].clear();

        file >> temp;

        file >> temp;
        // std::cout << "Supply_Net" << "   ";
        while (temp != "Supply_Port")
        {
            Supply_net[Port_name].insert(temp);
            // printf("%s ", temp.c_str());
            file >> temp;
        }
        // std::cout << std::endl << "Supply_Port" << "    ";
        file >> temp;
        while (temp != "Supply_Source" && !file.eof())
        {
            Supply_Port[Port_name].insert(temp);
            // printf("%s ", temp.c_str());
            file >> temp;
        }
        // std::cout << std::endl;
    }
    file.close();

    file.open(correct_file_name, std::ios::in);

    if (!file.is_open())
    {
        printf("文件打开错误");
        return 1;
    }

    file >> temp;
    while (!file.eof())
    {

        file >> temp; // 这才是接口名字

        const std::string Port_name = temp;
        // Supply_net[temp].clear();

        file >> temp;

        file >> temp;

        while (temp != "Supply_Port")
        {
            auto T = Supply_net[Port_name].find(temp);
            if (T == Supply_net[Port_name].end())
            {
                printf("no such Net %s in %s\n", temp.c_str(), Port_name.c_str());
                return 0;
            }
            Supply_net[Port_name].erase(T);

            file >> temp;
        }

        file >> temp;
        while (temp != "Supply_Source" && !file.eof())
        {
            auto T = Supply_Port[Port_name].find(temp);
            if (T == Supply_Port[Port_name].end())
            {
                printf("no such Port %s in %s\n", temp.c_str(), Port_name.c_str());
                return 0;
            }
            Supply_Port[Port_name].erase(T);

            file >> temp;
        }
    }
    file.close();

    for (auto i = Supply_Port.begin(); i != Supply_Port.end(); i++)
    {
        if (i->second.size() != 0)
        {
            std::cout << i->first << " Port error, exist ";
            for (auto j = i->second.begin(); j != i->second.end(); j++)
            {
                printf("%s ", j->c_str());
            }
            printf("\n");
        }
    }

    for (auto i = Supply_net.begin(); i != Supply_net.end(); i++)
    {
        if (i->second.size() != 0)
        {
            std::cout << i->first << " net error, exist";
            for (auto j = i->second.begin(); j != i->second.end(); j++)
            {
                printf("%s ", j->c_str());
            }
            printf("\n");
        }
    }
    std::cout <<"Source Finish!" << std::endl;
    return 0;
}