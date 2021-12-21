all: benchmark

proto:
	protoc --cpp_out=. *.proto

benchmark: benchmark.cc proto
	c++ -std=c++17 benchmark.cc demo.pb.cc -o benchmark `pkg-config --cflags --libs protobuf` -O3 -g
