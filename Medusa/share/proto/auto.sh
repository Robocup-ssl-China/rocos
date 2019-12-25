if [ ! -d cpp ]; then
	mkdir cpp
fi
protoc --cpp_out=cpp *.proto
# if [ ! -f python ]; then
# 	mkdir python
# fi
# protoc --python_out=python --cpp_out=cpp *.proto
# touch python/__init__.py
