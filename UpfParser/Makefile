cc = g++
prom = parser
source = main.cpp TclCommand.cpp  UpfModel.cpp  Utility.cpp NetworkAnalysis.cpp


$(prom) : $(source)
	$(cc) -o $(prom) $(source) -ltcl8.5 -std=c++11

build:
	$(cc) -o $(prom) $(source) -ltcl8.5 -std=c++11

run : $(source)
	$(cc) -o $(prom) $(source) -ltcl8.5 -std=c++11
	./$(prom) -f ../TestCase/case2/test.upf
debug: $(source)
	# $(prom) : $(source)
	$(cc) -o $(prom) $(source) -g -ltcl8.5 -std=c++11
	gdb $(prom) 
clear: $(prom)
	rm $(prom)