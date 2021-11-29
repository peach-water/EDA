# 简介
这是一个简单的UPF电路设计分析软件，可以帮助你发现电路设计过程中的潜在问题，帮助你更好的修复它们。

# 一点比赛要求

b. Please complete the reset implementation according to our requirement based on this. 
    -> You are encouraged to do some enhancements on this framework to finish your project better with lower time/memory cost 
    -> You are encouraged to complete the current parser framework based on your knowledge about LP/UPF
    -> You are encouraged to design the testing cases by yourself

# 如何编译
编译环境
* tcl 8.5
* gcc 4.8.5+
* ubuntu 18.04

```shell
    make
```
使用这个命令清理程序
```shell
    make clear
```
用这个命令可以打开gdb对当前程序进行调试。
```shell
    make debug
```

# 如何使用
```shell
    ./parser -f example.upf
```

# 自动化测试

本实验提供了完整的自动化测试脚本，可以一键比较出官方结果和本程序执行结果之间的差距。脚本使用方法

```python
    python3 AutoTestingCase.py
```

如果一切正常，每个case会有"correct： the result of source.csv(terminal.out/pst.csv) is correct！"的字符打印。否则会报出错误信息，并会显示"warning: the result of pst.csv isn't correct！"的输出信息，错误信息是相对程序执行结果输出的。也就是说报错行数应当看程序输出结果。与此同时还会有**时间**信息的输出，这个时间受到当时环境影响较大，不过更加能体现出平时运行的实际时间

**说明**

执行过程中会出现两个报错

* 普通case4的最后一行，官方答案其中的A_P2的3.1是错误的，在upf中没有3.1的状态定义，实际状态是2.1与程序结果一致。

* 普通case29的前两行，官方答案中Scope3/SP4出现了一个” * “号，代表这个端口有0.6和1.2两个值。程序会输出两行，分别是Scope3/SP4为0.6和1.2的两行。这一点点小误差不影响程序运行结果正确性。

# 测试用例设计

TestCase/DEScases文件夹中case1和case2是我队设计的两个不同的测试用例。
测试方法

```shell
    ./parser -f ../TestCase/DEScases/case1/case.upf
    ./parser -f ../TestCase/DEScases/case2/case.upf
```
