build:
	g++ *.cpp -pthread -lSDL2 -o launcher/chip-8

clean:
	rm chip-8
