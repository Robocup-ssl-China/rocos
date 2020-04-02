if [ ! -d "build" ]; then
	mkdir build
fi

cd build
if [ ! -d proto ]; then
	mkdir proto
fi
cd ../../Medusa/share/proto
../../../3rdParty/protobuf/bin/protoc.exe --cpp_out=../../../Client/build/proto *.proto
cd ../../../Client/build

cmake -A x64 -G "Visual Studio 14 2015" ..
cmake --build . --config Release

cp ../../3rdParty/ODE/bin/ode_double.dll ../../ZBin