if [ ! -d cpp ]; then
	mkdir cpp
fi
protoc --experimental_allow_proto3_optional --cpp_out=cpp *.proto
