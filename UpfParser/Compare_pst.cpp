#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <unordered_set>

bool isNum(std::string str) //判断是否是数字
{
    for (int i = 0; i < str.size(); i++)
    {
        int tmp = (int)str[i];
        if ((tmp >= 48 && tmp <= 57) || tmp == 46)
        {
            continue;
        }
        else
        {
            return false;
        }
    }
    return true;
}

std::string chg(std::string str)
{ //改变原有字符串，如果是整数则末尾添加“.0”，如果是“0ff”则改成“off”
    if (str == "0ff")
        str = "off";
    if (str != "off" && str.find(".") == str.npos)
        str = str + ".0";
    return str;
}

std::string chgth(int a)
{
    std::string b;
    if (a % 10 == 1)   
        b = "st";
    else if (a % 10 == 2)
        b = "nd";
    else if (a % 10 == 3)
        b = "rd";
    else
        b = "th";
    return b;
}

int main(int argv, char **argc)
{
    bool correct = true;
    std::string correct_fileName, myanswer_fileName;
    std::unordered_map<int, std::unordered_map<int, std::string>> correctState, myState;
    std::unordered_map<std::string, int> correctHeader;
    std::unordered_map<int, int> f;
    std::unordered_set<int> nc_row;

    correct_fileName = argc[2];
    myanswer_fileName = argc[1];

    std::string temp; // 临时变量
    std::fstream file;

    //存正确的pst
    file.open(correct_fileName, std::ios::in);
    if (!file.is_open())
    {
        printf("no such file %s\n", correct_fileName.c_str());
        return 1;
    }

    file >> temp;

    int first_row_num, first_col_num;
    while (!file.eof())
    {
        for (int i = 0; i < 10000; i++)
        {
            if (temp != "off" && !isNum(temp))
            {
                correctHeader[temp] = i;
                //  std::cout<<temp<<" ";
                file >> temp;
            }
            else
            {
                first_col_num = i; //记录表列数
                break;
            }
        }

        //  std::cout<<std::endl;

        for (int row = 0; !file.eof(); row++)
        {
            for (int col = 0; col < first_col_num; col++)
            {
                temp = chg(temp);
                correctState[row][col] = temp;
                first_row_num = row + 1; //记录表行数
                //  std::cout<<temp<<" ";
                file >> temp;
            }
            // std::cout<<std::endl;
        }
    }
    file.close();

    //  std::cout<<"-------------------"<<std::endl;

    //存我的pst
    file.open(myanswer_fileName, std::ios::in);
    if (!file.is_open())
    {
        printf("no such file %s\n", myanswer_fileName.c_str());
        return 1;
    }

    file >> temp;

    int myColNum; int myRowNum;
    while (!file.eof())
    {
        for (int i = 0; i < 10000; i++)
        {
            if (temp != "off" && !isNum(temp))
            {
                if (correctHeader.find(temp) == correctHeader.end())
                {
                    std::cout<<"Don't have header \""<<temp<<"\""<<std::endl;
                    correct = false;
                }
                else
                {
                    f[i] = correctHeader[temp];
                }
                // std::cout<<temp<<"-"<<f[i]<<" ";
                file >> temp;
            }
            else
            {
                myColNum = i;
                // std::cout<<myColNum<<" ";
                break;
            }
        }

        //  std::cout<<std::endl;

        for (int row = 0; !file.eof(); row++)
        {
            for (int col = 0; col < myColNum; col++)
            {
                temp = chg(temp);
                myState[row][col] = temp;
                //  std::cout<<temp<<" ";
                file >> temp;
            }
            myRowNum = row +1;
            nc_row.insert(row);
            //  std::cout<<std::endl;
        }
    }

    //比较两个表
    bool rowCorrect = 0;
    bool m_rowCorrect = 1;
    int correct_col_num = 0, NonCorrectRow = 0;
    if (first_col_num != myColNum)
    {
        std::cout << "The num of headers isn't correct" << std::endl;
        correct = false;
    }
    else
    {
        if (first_row_num != myRowNum)
        {
            std::cout << "The num of PstStates isn't correct" << std::endl;
            m_rowCorrect = false;
            correct = false;
        }
        for (int i = 0; i < first_row_num; i++)
        {
            rowCorrect = 0;
            NonCorrectRow = 0;
            for (int myRow = 0; !rowCorrect && myRow < myRowNum; myRow++)
            {
                for (int j = 0; j < first_col_num; j++)
                {
                    if (myState[myRow][j] == correctState[i][f[j]])
                    {
                        correct_col_num += 1;
                    }
                    else
                    {
                        rowCorrect = 0;
                        NonCorrectRow += 1;
                        break;
                    }
                }
                if (correct_col_num == first_col_num)
                {
                    rowCorrect = 1;
                    nc_row.erase(myRow);
                }
                correct_col_num = 0;
            }
            // std::cout<<myRowNum<<"-"<<NonCorrectRow<< std::endl;
            if (NonCorrectRow == myRowNum)
            {
                // std::cout<<"----"<<first_row_num<<"-"<<myRowNum<<std::endl;
                // std::cout << "The " << i + 1 << " row in correct pst doesn't exist!" << std::endl; // myPst的第i行状态不正确
                std::cout << "The " << i + 1 << chgth(i + 1) << " PstState in the correct pst doesn't exist in my pst!." << std::endl; // correct_pst的第几行不存在
                correct = false;
            }
        }
        if (!m_rowCorrect && !nc_row.empty())
        {
            std::cout<<"The ";
            for (auto nc = nc_row.begin(); nc != nc_row.end(); nc++)
            {
                std::cout << (*nc)+1 << chgth((*nc) + 1) <<" ";
            }   
            std::cout<<"PstState in my pst is not correct!"<<std::endl;
        }
    }

    // std::cout << "Pst Finish!" << std::endl;
    if (correct)
    {
        std::cout<<"correct： the result of pst.csv is correct！"<<std::endl;
        std::cout<<std::endl;
    }
    else 
    {
        std::cout<<"warning: the result of pst.csv isn't correct！"<<std::endl;
        std::cout<<std::endl;
    }

    correctState.clear();
    myState.clear();
    correctState.clear();
    f.clear();
    nc_row.clear();

    return 0;

}