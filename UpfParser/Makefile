cc = g++
prom = parser
source = main.cpp TclCommand.cpp  UpfModel.cpp  Utility.cpp NetworkAnalysis.cpp


$(prom) : $(source)
	$(cc) -o $(prom) $(source) -ltcl8.5 -std=c++11

perf:
	$(cc) -o $(prom) $(source) -ltcl8.5 -std=c++11 -pg
	# ./parser -f ../TestCase/BMCases/case9/case.upf
	# gprof ./parser gmon.out|less -S

debug: $(source)
	# $(prom) : $(source)
	$(cc) -o $(prom) $(source) -g -ltcl8.5 -std=c++11
	gdb $(prom) 
	
clear: $(prom)
	rm $(prom)