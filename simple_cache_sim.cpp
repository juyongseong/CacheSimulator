#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <cmath>
#include <stack>
using namespace std;

typedef struct {
	int tag;
	int valid;
}Line;

class LRU{
private:
	int size;
	int* RU;	//MRUbit는 RU[0], LRUbit는 RU[size-1]
	int associativity;
public:
	LRU() {}
	void init(int associativity) {	//LRU 초기화
		size=0;
		RU = new int[associativity];
		this->associativity = associativity;
	}
	void put(int num) {
		int temp;
		int state = 1;
		for (int i = 0; i < size; i++) {		//최근 사용한 slot를 가장 앞으로..
			if (num == RU[i]) {
				state = 0;
				temp = RU[i];
				for (int j = i; j > 0; j--) {
					RU[j] = RU[j - 1];
				}
				RU[0] = num;
				break;
			}
		}
		if (size == associativity) {
			//무조건 겹치지는 않음
		}
		else if(state) {
			for (int i = size; i > 0; i--) {
				RU[i]=RU[i-1];
			}
			RU[0] = num;
			size++;
		}
	}
	int getLRUbit() {
		return RU[size-1];
	}
	int getSize() {
		return size;
	}
};

typedef struct {
	Line* myLine;
	LRU* lru;
}Set;

class Cache{
private:
	int totalbyte;		//cache의 전체 바이트 단위 크기 
	int blockbyte;		//cache block 하나의 바이트 단위 크기 
	int nSet;			//set의 개수	2의 index승
	int associativity;	//associativity,즉 N-way associative cache에서 N값
	int nTag;			//TIO각각의 비트
	int nIndex;
	int nOffset;		
	int hit;			//접근, 히트, 미스 카운트
	int miss;
	int access;
	Set* allSet;		//Set구조체 배열을 가지고 있는 변수
	Set* mySet;			//Set 구조체를 가리키는 포인터 변수, 편한 연산을 위한..
public:
	Cache() {}
	~Cache() {
		for(int i=0; i<nSet; i++) {		//소멸자에서 메모리 반환
			mySet = allSet +i;
			free(mySet->myLine);
			free(mySet->lru);
		}
		free(allSet);

	}
	int init(int TB, int BB, int A) {					//주어진 값에따라 변수를 초기화 하는 함수
		totalbyte = TB;
		blockbyte = BB;
		associativity = A;
		nSet = (totalbyte/blockbyte)/associativity;

		nOffset = log((double)blockbyte)/log(2.0);			//계산법에 따라서 TIO비트구함
		nIndex = log((double)totalbyte)/log(2.0) - log((double)associativity)/log(2.0) - nOffset;
		nTag = 32 - nOffset - nIndex;
		hit=0;
		miss=0;
		access=0;

		allSet = (Set*)malloc((nSet)*sizeof(Set));	
		//set구조체를 nSet만큼(인덱스의개수) 저장할 공간 만듬. allSet[nSet]
		for(int i=0; i<nSet; i++) {			//각각의 set구조체 초기화
			mySet = allSet+i;
			mySet->myLine = (Line*)malloc((associativity)*sizeof(Line));	
			//Line구조체를 associativity만큼(slot의개수) 저장할 공간 만듬

			for(int j=0; j<associativity; j++) {		//각Line초기화
				(mySet->myLine+j)->tag =-1;
				(mySet->myLine+j)->valid =0;
			}
			mySet->lru= new LRU();						//LRU초기화
			(mySet->lru)->init(associativity);
		}

		if(log((double)blockbyte)/log(2.0) - (int)(log((double)blockbyte)/log(2.0)) != 0 ){
			printf("Cache block의 크기가 2의 지수승이 아닌 경우 \n");
			return 1;
		}
		if(associativity !=1 && associativity !=2 && associativity != 4 && associativity != 8) {
			printf("Associativity가 1, 2, 4, 8 이외의 값이 입력된 경우 \n");
			return 1;
		}
		if(totalbyte % (blockbyte * associativity) != 0) {
			printf("Cache 전체의 크기가 (cache block 크기)x(associativity)의 배수가 아닐 경우 \n");
			return 1;
		}
		return 0;
	}
	void printTIO() {									//TIO비트를 출력하는 함수
		printf("tag: %d bits \nindex: %d bits \noffset: %d bits\n", nTag, nIndex, nOffset);
	}
	void printfHitRate() {	//시뮬레이터의 결과 히트레이트를 출력하는 함수
		printf("Result: total access %d, hit %d, hit rate %.2f\n",
			access, hit, (float)hit/access);
	}
	int Simulator(const char* traceFile) {				
		//주어진 캐쉬의 형태에 따라서 시뮬레이터를 돌리는 함수
		char none[256];
		int address=0;
		FILE* fin = NULL;
		if((fin = fopen(traceFile, "r"))== NULL ) {					//파일을 읽기모드로 오픈
			printf("존재하지 않은 트레이스 파일 입력했을 경우 \n");	//파일이 없을경우 리턴1
			return 1;
		}
		int tag, index, offset;							//값을 저장할 정수	
		int tagMask, indexMask, offsetMask;				//비트마스크

		offsetMask = blockbyte-1;						
		//ex 만약 blockbyte == 16이라면 0000 0000 0000 0000 0000 0000 0000 1111
		indexMask = (nSet-1)<<nOffset;	
		tagMask = -1<<(nIndex+nOffset);

		while(!feof(fin)) {													//파일의 끝까지 읽음
			if(fscanf(fin, "%s %x %s", none, &address, none) != 3) break;	//\n\0에서 \0을 읽을경우 방지

			offset = address & offsetMask;									//비트마스크로 각 값을 저장
			index = (address & indexMask)>>nOffset;
			tag = (address & tagMask)>>nIndex+nOffset;

			access++;									//접근++
			//printf("@tag : %x,\tindex : %x,\t",tag, index);
			mySet = allSet + index;						// mySet = allSet[index]
			bool missOrHit = false;						//false는 miss, true는 hit
			for(int j=0; j<associativity; j++) {
				if((mySet->myLine)->valid == 1 && (mySet->myLine+j)->tag == tag) {		
					//라인이 채워져 있고 같은태그가 있다면
					missOrHit = true;
					hit++;								//히트++
					//cout<<"hit,\tslot : "<< j<<endl;
					(mySet->lru)->put(j);				//LRU계산
					break;
				}
			}
			if(missOrHit ==false) {					//미스라면
				miss++;								//미스++
				
				int size = (mySet->lru)->getSize();	
				//size를 포문에서 invalue를 체크해서 구할수도 있지만,, 
				//LRU구조체를 이용하여 구함.
				if(size == associativity) {	
					//꽉차있다면 LRU비트에 값을 채워 넣는다.
					int LRUbit = (mySet->lru)->getLRUbit();	
					(mySet->myLine+LRUbit)->tag=tag;
					(mySet->myLine+LRUbit)->valid=1;
					(mySet->lru)->put(LRUbit);	//LRU계산
					//cout<<"miss,\tslot : "<<LRUbit<<endl;
				}else {	
					//꽉차지 않았다면 남은 칸에 값을 채워 넣는다.
					(mySet->myLine+size)->tag=tag;
					(mySet->myLine+size)->valid=1;	
					(mySet->lru)->put(size);	//LRU계산
					//cout<<"miss,\tslot : "<<size<<endl;
				}
			}
		}
		fclose(fin);				//파일을 닫는다.
		return 0;
	}
};

int main(int argc, const char* argv[]) {

	if(argc != 5) {
		printf("프로그램의 입력 인자가 4개가 아닐 경우\n");				//입력된 인자의 수가 잘못된 경우
		printf("사용법. simple_cache_sim <trace file> <cache's byte size> <cache block's byte size> <number of associativity>\n");
		return 1;
	}
	Cache* cache = new Cache();
	const char* traceFile = argv[1];

	if(cache->init(atoi(argv[2]),atoi(argv[3]),atoi(argv[4])) == 0) {	
		//입력된 값의 초기화가 정상일 경우
		cache->printTIO();
		if(cache->Simulator(traceFile) == 0) {				
			//시뮬레이터가 정상 종료됬을 경우
			cache->printfHitRate();
		}else 
			printf("시뮬레이터의 비정상 종료");
	}else 
		printf("올바르지 않은 캐쉬의 초기화\n");

	return 0;
}
