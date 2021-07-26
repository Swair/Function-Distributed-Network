TARGET  := fdnserver


SRC  :=  $(wildcard *.c) $(wildcard *.cc)
DIR  :=  $(notdir $(SRC))                 
OBJS :=  $(patsubst %.c,%.o, $(patsubst %.cc,%.o,$(DIR)))  

CC       := g++ -g -std=c++11


INCLUDE := -std=c++11 -g -Wall \
 	-D_GLIBCXX_USE_CXX11_ABI=0 \
	-I. 


LIBS  := -lworkflow -lssl -lcrypto -lpthread


CFLAGS   := -std=c++11 -g -Wall -O0 $(DEFINES) $(INCLUDE) 
CXXFLAGS := $(CFLAGS) -DHAVE_CONFIG_H


.PHONY : objs clean purge   

all  : $(TARGET)  


objs : $(OBJS)  

clean :
	rm -rf $(OBJS) 

purge : clean
	rm -rf $(TARGET)

repacbuild: purge all


$(TARGET) : $(OBJS)
	$(CC) $(INCLUDE) $^ -o $@ $(LIBS) 






