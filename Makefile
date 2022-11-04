
FLAGS=-std=c++17 -Wall -Werror -Wpedantic -D_GNU_SOURCE -g
COMPILER=g++

main: main.cpp student.o parsing.o db.o query.o utils.o select.o update.o delete.o insert.o
	${COMPILER} -o tinydb main.cpp parsing.o student.o db.o query.o utils.o select.o update.o delete.o insert.o ${FLAGS}

run:
	make main && ./tinydb

parsing.o: parsing.cpp parsing.hpp student.o
	${COMPILER} -c parsing.cpp ${FLAGS}

student.o: student.cpp student.hpp
	${COMPILER} -c student.cpp ${FLAGS}

query.o: query.cpp query.hpp
	${COMPILER} -c query.cpp ${FLAGS}

db.o: db.cpp db.hpp
	${COMPILER} -c db.cpp ${FLAGS}

utils.o: utils.cpp utils.hpp
	${COMPILER} -c utils.cpp ${FLAGS}

select.o: select.cpp select.hpp
	${COMPILER} -c select.cpp ${FLAGS}

update.o: update.cpp update.hpp
	${COMPILER} -c update.cpp ${FLAGS}

delete.o: delete.cpp delete.hpp
	${COMPILER} -c delete.cpp ${FLAGS}

insert.o: insert.cpp insert.hpp
	${COMPILER} -c insert.cpp ${FLAGS}


tests: tests/run_tests.py
	./tests/run_tests.py

clean:
	rm logs/*
clear:
	rm *.o
	rm tinydb
	#rm main
