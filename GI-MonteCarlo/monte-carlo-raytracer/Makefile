all:

cuda:
	/usr/local/cuda/bin/nvcc -O3 -arch=sm_30 -o montecarlo -I Util -I Primitives -L/usr/local/cuda/lib -lcuda -lcudart -Xcompiler -fopenmp main.cu 
osx:
	g++ -std=c++11  -O3 -o montecarlo  -I$(CURDIR)  main.cpp
linux:
	g++ -std=c++11  -O3 -o montecarlo  -I$(CURDIR) -I/usr/include/OpenEXR  main.cpp -Wl,--no-as-needed -lrt -L/usr/lib -lHalf -lIlmImf 
mpi:
	mpic++ -std=c++11  -O3 -o montecarlo  -I$(CURDIR) -fopenmp main_mpi.cpp
windows:
	#VPATH = "C:\Program Files (x86)\ILMBase\include\OpenEXR"
	g++ -I $(CURDIR) -I"C:\Program Files (x86)\ILMBase\include\OpenEXR" -O3 -std=gnu++11 main.cpp -o montecarlo -lgomp
triolith:
	g++ -std=c++11 -Nmpi  -O3 -o montecarlo  -I$(CURDIR) -I/usr/include/OpenEXR  main.cpp -Wl,--no-as-needed -lrt

clean: rm -rf *o exec.out
