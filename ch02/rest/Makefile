CC=g++
CFLAGS=-std=c++14
LDFLAGS=-lcpprest -lpthread -lssl -lcrypto

all: CppRestSdkReadPage CppRestSdkSimpleService

CppRestSdkSimpleService:
	$(CC) $(CFLAGS) CppRestSdkSimpleService.cpp -o ./CppRestSdkSimpleService $(LDFLAGS)

CppRestSdkReadPage:
	$(CC) $(CFLAGS) CppRestSdkReadPage.cpp -o ./CppRestSdkReadPage $(LDFLAGS)

clean:
	rm -f *.o CppRestSdkReadPage CppRestSdkSimpleService
