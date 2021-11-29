#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <unordered_set>

std::string getString(std::string &input)
{ // 去掉两边的空格
    return input.substr(1, input.size() - 2);
}

int main(int argv, char **argc)
{
    bool correct = true;
    std::string correct_fileName, myanswer_fileName;
    std::unordered_map<std::string, std::unordered_set<std::string>> State, NonZero_State;

    if (argv < 2)
    {
        printf("parameter not enough\n");
        return 0;
    }

    correct_fileName = argc[2];
    myanswer_fileName = argc[1];

    std::string temp, t1, t2; // 临时变量
    std::fstream file;

    file.open(myanswer_fileName, std::ios::in);
    if (!file.is_open())
    {
        printf("no such file %s\n", myanswer_fileName.c_str());
        return 1;
    }

    file >> temp;
    while (!file.eof())
    {

        if (temp == "State")
        {
            file >> temp;
            t1 = getString(temp);
            for (int i = 0; i < 3; i++)
            {
                file >> temp;
            }
            t2 = getString(temp);
            for (int i = 0; i < 4; i++)
            {
                file >> temp;
            }

            State[t2].insert(t1); // 添加state记录
        }
        else if (temp == "Non-Zero")
        {
            file >> temp;
            std::unordered_set<std::string> l_temp;
            while (temp != "applied")
            {
                file >> temp;
                l_temp.insert(getString(temp));
            }
            file >> temp;
            file >> temp;
            file >> temp;
            file >> temp;
            NonZero_State[getString(temp)] = l_temp;
        }
        else
        {
            printf("你获得了： %s", temp.c_str());
        }
        file >> temp;
    }

    file.close();
    file.open(correct_fileName, std::ios::in);

    if (!file.is_open())
    {
        printf("no such file %s \n", correct_fileName.c_str());
        return 1;
    }

    file >> temp;
    while (!file.eof())
    {
        if (temp == "State")
        {
            file >> temp;
            t1 = getString(temp);
            for (int i = 0; i < 3; i++)
            {
                file >> temp;
            }
            t2 = getString(temp);
            for (int i = 0; i < 4; i++)
            {
                file >> temp;
            }
            // State[t1] = t2; // 添加state记录
            auto T = State.find(t2);
            if (T == State.end())
            {
                printf("Error, no such %s in %s\n", t1.c_str(), t2.c_str());
                correct = false;
            }
            else if (T->second.find(t1) == T->second.end())
            {
                printf("Error, %s no such %s\n", t2.c_str(), t1.c_str());
                correct = false;
            }
            else
            {
                T->second.erase(T->second.find(t1));
            }
        }
        else if (temp == "Non-Zero")
        {
            file >> temp;
            std::unordered_set<std::string> l_temp;
            while (temp != "applied")
            {
                file >> temp;
                l_temp.insert(getString(temp));
            }
            file >> temp;
            file >> temp;
            file >> temp;
            file >> temp;
            // NonZero_State[getString(temp)] = l_temp;
            temp = getString(temp);
            for (auto i = l_temp.begin(); i != l_temp.end(); i++)
            {
                auto T = NonZero_State.find(temp);
                if (T == NonZero_State.end())
                {
                    printf("Error, no %s", temp.c_str());
                    correct = false;
                }
                else
                {
                    auto Y = NonZero_State[temp].find(*i);
                    if (Y == NonZero_State[temp].end())
                    {
                        printf("Error, %s don't have %s\n", i->c_str(), temp.c_str());
                        correct = false;
                    }
                    else
                    {
                        NonZero_State[temp].erase(Y);
                    }
                }
            }
        }
        else
        {
            printf("你获得了： %s", temp.c_str());
        }
        file >> temp;
    }

    for (auto i = State.begin(); i != State.end(); i++)
    {
        if (i->second.size() != 0)
        {
            correct = false;
            printf("You exist,but not correct: %s\n", i->first.c_str());
            for (auto j = i->second.begin(); j != i->second.end(); j++)
            {
                printf("%s ", j->c_str());
            }
            printf("\n");
        }
    }

    for (auto i = NonZero_State.begin(); i != NonZero_State.end(); i++)
    {
        if (i->second.size() != 0)
        {
            correct = false;
            printf("You exist,but not correct: %s\n", i->first.c_str());
            for (auto j = i->second.begin(); j != i->second.end(); j++)
            {
                printf("%s ", j->c_str());
            }
            printf("\n");
        }
    }
    file.close();
    if (correct)
    {
        std::cout<<"correct： the result of terminal.out is correct！"<<std::endl;
    }
    else 
    {
        std::cout<<"warning: the result of terminal.out isn't correct！"<<std::endl;
    }
    // std::cout
    //     << "terminal Finish!" << std::endl;
    return 0;
}