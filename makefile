PROGNAME := program
SOURCESWITHMAIN := $(wildcard src/*.cpp)
HEADERS := $(wildcard src/*.hpp)
HEADERS := $(patsubst src/%.hpp, inc/%.hpp, $(HEADERS))
SOURCES := $(patsubst main.cpp, , $(SOURCESWITHMAIN))
OBJECTS := $(patsubst src/%.cpp, obj/%.o, $(SOURCES))
DEPENDS := $(patsubst src/%.cpp, obj/%.dep, $(SOURCESWITHMAIN))

bin/$(PROGNAME): ./bin lib/libStat.a src/main.cpp $(HEADERS)
	@g++ -std=c++11 src/main.cpp -DBOOST_ALL_DYN_LINK -pthread -L lib -l Stat -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_dnn -lopencv_video -lopencv_videoio -lboost_log -lboost_log_setup -lboost_thread -lboost_system -o bin/$(PROGNAME) -I ./inc 
	@echo Succesfully...
inc/%.hpp: src/%.hpp ./inc
	@ln -f $< $@
./inc:
	@echo inc folder created...
	@mkdir -p inc
./bin:
	@mkdir -p bin
	@echo bin folder created...
lib/libStat.a: ./lib $(OBJECTS)
	@ar rc lib/libStat.a $(OBJECTS)
	@echo static lib created...
./lib:
	@mkdir -p lib
	@echo lib folder created...
obj/%.o: src/%.cpp
	@mkdir -p obj
	@g++ -std=c++11 -DBOOST_ALL_DYN_LINK -c $< -o $@

obj/%.dep: src/%.cpp
	@mkdir -p obj
	@g++ -MM $< -MT "$@ $(patsubst %.dep, %.o, $@)" -o $@
-include $(DEPENDS)

.PHONY: clean
clean:
	@rm -rf bin/ lib/ obj/ inc/
	@echo Cleaning...

.PHONY: push
push:
	@git add .
	@git commit -m "some changes"
	@git push origin master

.PHONY: help
help:
	@echo type make for compiling
	@echo type make clean for cleaning
	@echo type make push for fast pushing to repo
