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
{   //改变原有字符串，如果是整数则末尾添加“.0”，如果是“0ff”则改成“off”
    if(str == "0ff")
        str = "off";
    if(str != "off" && str.find(".") == str.npos)
        str = str + ".0";
    return str;
}

int main(int argv, char **argc)
{
    std::string correct_fileName, myanswer_fileName;
    std::unordered_map<int, std::unordered_map<int, std::string>> correctState, myState;
    std::unordered_map<std::string, int> correctHeader;
    std::unordered_map<int,int> f;


    correct_fileName = argc[2];
    myanswer_fileName = argc[1];

    std::string temp; // 临时变量
    std::fstream file;

    //存正确的pst
    file.open(correct_fileName, std::ios::in); 
    if(!file.is_open()){
        printf("no such file %s\n",correct_fileName.c_str());
        return 1;
    }

    file >> temp;

    int row_num, col_num;
    while (!file.eof())
    {
        for (int i = 0; i < 1000; i++)
        {
            if (temp != "off" && !isNum(temp))
            {
                correctHeader[temp] = i;
                //  std::cout<<temp<<" ";
                file >> temp;
            }
            else
            {
                col_num = i;//记录表列数
                break;
            }
        }

        //  std::cout<<std::endl;
        
        for(int row = 0; !file.eof(); row++)
        {
            for(int col = 0; col < col_num; col++)
            {
                temp = chg(temp);
                correctState[row][col] = temp;
                row_num = row + 1;//记录表行数
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
    if(!file.is_open()){
        printf("no such file %s\n",myanswer_fileName.c_str());
        return 1;
    }

    file >> temp;

    int cnum;
    while (!file.eof())
    {
        for (int i = 0; i < 1000; i++)
        {
            if (temp != "off" && !isNum(temp))
            {
                if(correctHeader.find(temp) == correctHeader.end())
                {
                    //std::cout<<"Don't have header \""<<temp<<" \""<<std::endl;
                }
                else
                {
                    f[i] = correctHeader[temp];
                }
                //  std::cout<<temp<<"-"<<f[i]<<" ";
                file >> temp;
            }
            else
            {
                cnum = i;
                break;
            }
        }

        //  std::cout<<std::endl;
        
        for(int row = 0; !file.eof(); row++)
        {
            for(int col = 0; col < cnum; col++)
            {
                temp = chg(temp);
                myState[row][col] = temp;
                //  std::cout<<temp<<" ";
                file >> temp;   
            }
            //  std::cout<<std::endl;
        }
    }
    file.close();


    //比较两个表
    bool rowCorrect = 0;
    int correct_col_num = 0, correct_row_num = 0, NonCorrectRow = 0;
    if(col_num != cnum)
    {
        std::cout<<"The num of headers is incorrect"<<std::endl; 
    }
    else
    {
        for(int i = 0; i< row_num; i++)
        {
            rowCorrect = 0;
            NonCorrectRow = 0;
            for(int myRow = 0; !rowCorrect && myRow < row_num; myRow++)
            {
                for (int j = 0; j < col_num; j++)
                {
                    if(myState[i][j] == correctState[myRow][f[j]])
                    {
                        correct_col_num+=1;
                    }
                    else
                    {
                        rowCorrect = 0;
                        NonCorrectRow += 1;
                        break;
                    } 
                }
                if(correct_col_num == col_num)
                {
                    rowCorrect = 1;
                    correct_row_num += 1;
                }
                correct_col_num = 0;
            } 
            if( NonCorrectRow == row_num )
            {
                // std::cout<<NonCorrectRow<<"-"<<row_num<<std::endl;
                std::cout<<"The "<< i+1 <<" row is incorrect"<<std::endl;  //myPst的第i行状态不正确
            }   
        }
        if(correct_row_num == row_num)
        {
            // std::cout<<"The pst is correct"<<std::endl; 
        }     
    }

    std::cout <<std::endl<<"Finish!"<<std::endl;
    return 0;
}