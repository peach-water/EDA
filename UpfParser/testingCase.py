import os

os.system("echo 1")

os.system("g++ Compare_source.cpp")
os.system("make build")

for i in range(48):
    string = "echo Normal case " + str(i)
    os.system(string)
    string = "./parser -f ../TestCase/case" + str(i) + "/test.upf"
    os.system(string)
    string2 = "./a.out source.csv ../TestCase/case" + str(i) + "/source.csv"
    os.system(string2)
# BMCase测试
for i in range(1,11,1):
    string = "echo BMCcase " + str(i)
    os.system(string)
    string = "./parser -f ../TestCase/BMCases/case" + str(i) + "/case.upf" 
    os.system(string)
    string2 = "./a.out source.csv ../TestCase/BMCases/case" + str(i) + "/source.csv"
    os.system(string2)