import os
import time


os.system("g++ Compare_source.cpp -o a.out -std=c++11")
os.system("g++ Compare_terminal.cpp -o b.out -std=c++11")
os.system("g++ Compare_pst.cpp -o c.out -std=c++11")
os.system("make")

for i in range(48):
    string = "echo Normal case " + str(i)
    os.system(string)
    string = "./parser -f ../TestCase/case" + str(i) + "/test.upf"
    start = time.perf_counter()
    os.system(string)
    end = time.perf_counter()
    print(end-start,end=" seconds")
    print()
    string2 = "./a.out ./source.csv ../TestCase/case" + str(i) + "/source.csv"
    os.system(string2)
    string2 = "./b.out ./terminal.out ../TestCase/case" + str(i) + "/terminal.out"
    os.system(string2)
    string2 = "./c.out ./pst.csv ../TestCase/case" + str(i) + "/pst.csv"
    os.system(string2)
# BMCase测试
for i in range(1,11,1):
    string = "echo BMCcase " + str(i)
    os.system(string)
    string = "./parser -f ../TestCase/BMCases/case" + str(i) + "/case.upf" 
    start = time.perf_counter()
    os.system(string)
    end = time.perf_counter()
    print(end-start,end=" second")
    print()
    string2 = "./a.out ./source.csv ../TestCase/BMCases/case" + str(i) + "/source.csv"
    os.system(string2)
    string2 = "./b.out ./terminal.out ../TestCase/BMCases/case" + str(i) + "/terminal.out"
    os.system(string2)
    string2 = "./c.out ./pst.csv ../TestCase/BMCases/case" + str(i) + "/pst.csv"
    os.system(string2)

os.system("rm ./a.out")
os.system("rm ./b.out")
os.system("rm ./c.out")