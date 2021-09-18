cc = g++
prom = parser
source = main.cpp TclCommand.cpp  UpfModel.cpp  Utility.cpp 


$(prom) : $(source)
	$(cc) -o $(prom) $(source) -ltcl8.5 -std=c++11

run : $(source)
	$(cc) -o $(prom) $(source) -ltcl8.5 -std=c++11
	./$(prom) -f example.upf
debug: $(source)
	# $(prom) : $(source)
	$(cc) -o $(prom) $(source) -g -ltcl8.5 -std=c++11
	gdb $(prom) 