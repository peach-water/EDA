#include<iostream>
#include<string>
#include<fstream>

int main()
{
	// char data[200];
    // fstream  afile;
    // afile.open("afile.dat", ios::app|ios::out|ios::in);
    // cin>>data;
    // //将数据写入文件
    // afile<<data;
    // //将数据从文件中读取出来
    // afile>>data;
    // cout<<data;
    // afile.close();

    std::cout<<"test basic cases source.csv"<<std::endl
            <<"g++ Compare_source.cpp"<<std::endl;
    for(int i=0; i<48; i++)
    {
        std::cout << "./parser -f ../TestCase/case"<<i<<"/test.upf"<<std::endl
                //<< "g++ Compare_source.cpp"<<std::endl
                <<"./a.out source.csv ../TestCase/case"<<i<<"/source.csv"<<std::endl;
    }
    std::cout<<std::endl<<std::endl;
    std::cout<<"test basic cases terminal.out"<<std::endl
            <<"g++ Compare_terminal.cpp"<<std::endl;
    for(int i=0; i<48; i++)
    {
        std::cout << "./parser -f ../TestCase/case"<<i<<"/test.upf"<<std::endl
                <<"./a.out terminal.out ../TestCase/case"<<i<<"/terminal.out"<<std::endl;
    }
    std::cout<<std::endl<<std::endl;
    std::cout<<std::endl<<std::endl;
    std::cout<<"test basic cases pst.csv"<<std::endl
            <<"g++ Compare_pst.cpp"<<std::endl;
    for(int i=0; i<48; i++)
    {
        std::cout << "./parser -f ../TestCase/case"<<i<<"/test.upf"<<std::endl
                <<"./a.out pst.csv ../TestCase/case"<<i<<"/pst.csv"<<std::endl;
    }
    std::cout<<std::endl<<std::endl;




    std::cout<<"test BMCases source.csv"<<std::endl
            <<"g++ Compare_source.cpp"<<std::endl;

    for(int i=1; i<11; i++)
    {
        std::cout << "./parser -f ../TestCase/BMCases/case"<<i<<"/case.upf"<<std::endl
                //<< "g++ Compare_source.cpp"<<std::endl
                <<"./a.out source.csv ../TestCase/BMCases/case"<<i<<"/source.csv"<<std::endl;
    }
    std::cout<<std::endl<<std::endl;
    std::cout<<"test BMCases terminal.out"<<std::endl
            <<"g++ Compare_terminal.cpp"<<std::endl;
    for(int i=1; i<11; i++)
    {
        std::cout << "./parser -f ../TestCase/BMCases/case"<<i<<"/case.upf"<<std::endl
                //<< "g++ Compare_source.cpp"<<std::endl
                <<"./a.out terminal.out ../TestCase/BMCases/case"<<i<<"/terminal.out"<<std::endl;
    }
    std::cout<<std::endl<<std::endl;
    std::cout<<"test BMCases pst.csv"<<std::endl
            <<"g++ Compare_pst.cpp"<<std::endl;
    
    for(int i=1; i<11; i++)
    {
        std::cout << "./parser -f ../TestCase/BMCases/case"<<i<<"/case.upf"<<std::endl
                <<"./a.out pst.csv ../TestCase/BMCases/case"<<i<<"/pst.csv"<<std::endl;
    }
    std::cout<<std::endl<<std::endl;

	return 0;


}
