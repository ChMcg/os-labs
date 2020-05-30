#!/bin/bash



# g++ lab_07_1.cpp -o build/lab_07_1 -lpthread -lrt
# g++ lab_07_2.cpp -o build/lab_07_2 -lpthread -lrt

# g++ lab_08_1.cpp -o build/lab_08_1 -lpthread -lrt
# g++ lab_08_2.cpp -o build/lab_08_2 -lpthread -lrt

# g++ lab_09_1.cpp -o build/lab_09_1 -lpthread -lrt
# g++ lab_09_2.cpp -o build/lab_09_2 -lpthread -lrt

f=`find -name \*.cpp | grep -v 'old' | grep -v 'getch.cpp' | grep -v 'config.cpp' | sort`

if [ `mkdir -p object_code` ]; then
	echo "./object_code created"
fi

if [ `mkdir -p build` ]; then
	echo "./build created"
fi

for file in $f
do

    OFNAME="`pwd`/build/`echo $file | sed -e 's/.\///' | sed -e 's/.cpp//'`"
	g++ -g "$file" -o "$OFNAME" -lpthread -lrt;
	fine=$?
	echo -n "Processing $file: "
	if [ $fine == 0 ]; then
		echo -en "\033[37;1;42m done \033[0m\n"
	else
		echo -en "\033[37;1;41m error \033[0m\n"
	fi
done

