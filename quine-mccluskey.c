#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 1000

typedef struct minterm {
	char bit[40]; // 각각의 bit를 저장할 배열 선언
	int flag; // 이 minterm이 사용되었는지 확인하기 위한 변수
	int usage; // 이 minterm이 비교할 때 사용되었는지 확인하기 위한 변수
	int covering[40]; // 이 minterm이 어떤 것을 커버하는지를 저장하기 위한 변수
	int length;
} Minterm;

int countOne(char a[], int length); // minterm의 1의 개수 세는 함수
Minterm** memoryAllocate(int width, int length); // 메모리 동적 할당하는 함수
int memoryRelease(Minterm** arr, int length); // 할당한 메모리 해제하는 함수
void removeBSN(char str[]); // '\0' 값을 없애기 위한 함수
void findPrime(Minterm** arr, Minterm* prime, int width, int length, int *primePointer); // PI 찾기 위한 함수
void copyNPaste(Minterm** arr, Minterm** arr1, int width, int length); // 비교 완료한 것들을 이전 배열에 저장하고, 나머지는 다 초기화하는 함수 
int coverOrNot(int *trueMintermCover, int trueNumber); // minterm이 전부 커버가 되었는지 확인하는 함수
int calculateCost(Minterm *essentialPI, int bitLength); // essential PI로 구성할 때 필요한 트랜지스터 개수 계산하는 함수

int main(void) {
	FILE * fp1 = fopen("input_minterm.txt", "rt"); // input 파일 오픈
	FILE * fp2 = fopen("result.txt", "wt"); // output 파일 오픈

	Minterm **arr; // minterm의 정보를 저장할 배열
	Minterm **arr1; // minterm의 정보를 저장할 배열
	Minterm temp[SIZE] = { 0 }; // input 파일로부터 읽은 값을 임시로 저장할 배열
	Minterm trueMinterm[SIZE] = { 0 }; // true minterm을 저장할 배열
	Minterm prime[SIZE] = { 0 }; // PI를 저장할 배열
	Minterm essentialPI[SIZE] = { 0 }; // essential PI를 저장할 배열
	int trueMintermCover[SIZE] = { 0 }; // essential PI를 찾기 위해 PI가 이 배열을 전부 커버(1로 만들어야함)하는지 확인하기 위한 배열
	int sumOfCovering[SIZE] = { 0 }; // PI가 커버하는 minterm의 개수를 저장할 배열
	int PI_index[SIZE] = { 0 }; // 배열에서 PI의 위치를 저장할 배열
	int trueNumber = 0; // true minterm의 개수를 저장할 변수
	int bitLength; // bit 길이 저장할 변수
	int inputNumber = 0; // 입력된 minterm의 개수를 저장할 변수
	int i, j, k, m, n, r; // 각종 loop에 사용될 변수
	int width, length; // 각각 배열의 가로와 세로의 길이를 저장하는 변수
	int cal; // minterm의 1의 개수를 저장할 변수
	char temp1[SIZE] = { 0 }; // minterm의 값을 임시로 저장할 배열
	int count = 0; // 두 minterm끼리의 bit가 달라 그 bit를 '-'로 바꾼 횟수 저장할 변수
	int index; // 배열의 index 값 저장할 변수
	int tempNum1, tempNum2; // 배열 내림차순할 때 값을 임시로 저장하기 위한 변수
	int compare; // PI가 minterm을 커버하는지를 저장할 변수
	int cost; // 필요한 트랜지스터 개수를 저장할 변수
	int primeIndex = 0; // prime 배열에서의 index 값을 저장할 배열
	int * primePointer = &primeIndex; // primeIndex를 가리키는 포인터 변수

	i = 0;
	while (feof(fp1) == 0) { // input 파일을 끝까지 읽어들임	
		fgets(temp[i].bit, sizeof(temp[i].bit), fp1); // temp배열에 임시로 읽은 값들 저장
		removeBSN(temp[i].bit); // '\0' 값 제거

		/* true minterm을 따로 저장 */
		if (temp[i].bit[0] == 'm') {
			strcpy(trueMinterm[trueNumber].bit, temp[i].bit);
			trueNumber++;
		}
		i++;
		inputNumber++; // 입력된 input의 개수 count
	}

	bitLength = atoi(temp[0].bit); // bit 길이 저장

	/* bit 길이를 처음에 입력하지 않은 경우 */
	if (temp[0].bit[0] == 'd' || temp[0].bit[0] == 'm') {
		fprintf(fp2, "bit 길이를 처음에 입력해주십시오\n");
		return 0;
	}		

	/* 입력한 bit 길이와 실제 minterm의 bit 길이가 다를 경우 */
	for (i = 1; i < inputNumber; i++) {
		if (temp[i].bit[bitLength + 2] != '\0') {
			fprintf(fp2, "입력한 bit 길이에 맞게 minterm을 입력해주십시오\n");
			return 0;
		}
	}

	width = inputNumber; // 입력 받은 input의 개수를 implicant table의 가로 길이에 저장
	length = bitLength + 1; // 입력 받은 bit 길이 + 1을 implicant table의 세로 길이에 저장

	arr = memoryAllocate(width, length); // 메모리 할당
	arr1 = memoryAllocate(width, length); // 메모리 할당


	/* minterm을 1의 개수에 따라 분류하여 implicant table에 정렬 */
	for (i = 1; i < inputNumber - 1; i++) {
		cal = countOne(temp[i].bit, bitLength); // minterm의 1의 개수를 count
		for (j = 0; j < width; j++) {
			if (arr[cal][j].flag == 0) { // 저장하려는 위치에 다른 값이 없을 경우 저장			
				for (n = 0; n < bitLength; n++) {
					arr[cal][j].bit[n] = temp[i].bit[n + 2];
				}
				arr[cal][j].flag = 1; // 이 자리에는 값이 저장되어 있다는 것을 알림
				j = width; // loop문 탈출
			}
		}
	}

	for (k = 0; k < bitLength; k++) {
		for (i = 0; i < length - 1; i++) {
			for (n = 0; n < width; n++) {
				index = 1;
				if (arr[i][n].flag == 0) // 해당 위치에 minterm이 없을 때 다음 행으로 가기 위해 loop 탈출
					n = width;
				else {
					strcpy(temp1, arr[i][n].bit); // 비교 대상을 임시로 저장
					for (j = 0; j < width; j++) { // 다음 행의 모든 요소와 비교					
						if (i + index > length - 1) { // 다음 행이 존재하지 않을 때 비교 중단하기 위해 loop 탈출						
							j = width;
							n = width;
							i = length;
						}
						else {
							if (arr[i + index][0].flag == 0) { // 해당 위치의 바로 다음 행 첫번째 요소에 minterm이 없으면 다음 행으로 넘어감							
								if (i + index < length - 1) {
									index++;
									j = 0;
								}
							}
							else if (arr[i + index][j].flag == 0) { // 행에 더 이상 비교할 minterm이 없을 때 다음 행으로 넘어감							
								j = width;
							}
							else {
								/* 두 minterm 사이에서 bit 단위로 값을 비교 */
								for (m = 0; m < bitLength; m++) {
									if (temp1[m] == '-' || arr[i + index][j].bit[m] == '-')	{
										if (temp1[m] == '-' && arr[i + index][j].bit[m] == '-') // 두 bit 모두 '-'일 경우 다음 bit 비교
											continue;
										else { // PI를 만들지 못해 다른 minterm을 비교해야한다.										
											m = bitLength;
											count = 0;
										}
									}
									else if (temp1[m] != arr[i + index][j].bit[m]) { // 서로의 bit가 다를 때									
										temp1[m] = '-';
										count++;
									}
								}
								if (count == 1) { // 한 bit만 달랐을 때								
									r = 0;
									while (arr1[i][r].flag != 0) {
										r++;
									}
									strcpy(arr1[i][r].bit, temp1); // 다른 그룹에 저장
									arr[i][n].usage = 1; // 비교되었던 minterm들 체크
									arr[i + index][j].usage = 1; // 비교되었던 minterm들 체크
									arr1[i][r].flag = 1; // minterm이 존재한다는 것을 알림
									r = 0;
								}
								count = 0;
								strcpy(temp1, arr[i][n].bit); // 다시 비교하기 위해 원래 값으로 초기화
							}
						}
					}
				}
			}
		}
		findPrime(arr, prime, width, length, primePointer); // PI 찾기 위한 함수
		copyNPaste(arr, arr1, width, length); // 비교 완료한 것들을 이전 배열에 저장하고, 나머지는 다 초기화하는 함수
	}

	/* 중복되는 prime implicant를 걸러냄 */
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE - 1 - i; j++) {
			if (!strcmp(prime[SIZE - 1 - i].bit, prime[j].bit)) {
				prime[SIZE - 1 - i].flag = 0;
			}
		}
	}

	/* PI가 어떤 minterm을 커버하는지를 저장 */
	for (i = 0; i < SIZE; i++) {
		if (prime[i].flag == 0)
			continue;
		else {
			for (j = 0; j < trueNumber; j++) {
				compare = 1;
				for (m = 0; m < bitLength; m++) {
					if (prime[i].bit[m] == '-')
						continue;
					else if (prime[i].bit[m] != trueMinterm[j].bit[m + 2])
						compare = 0;
				}
				if (compare == 1)
					prime[i].covering[j] = 1;
			}
		}
	}

	/* PI가 커버하는 minterm의 개수 저장 */
	for (i = 0; i < SIZE; i++) {
		if (prime[i].flag == 0)
			continue;
		else {
			for (j = 0; j < trueNumber; j++) {
				sumOfCovering[i] += prime[i].covering[j];
			}
		}
	}

	/* PI_index 배열을 1부터 1씩 증가하여 저장 */
	for (i = 0; i < SIZE; i++)
		PI_index[i] = i;

	/* PI가 커버하는 minterm의 개수를 내림차순으로 정렬하고, 그 PI의 index도 같이 내림차순해서 위치를 알 수 있게 함 */
	for (i = 0; i < SIZE - 1; i++) {
		for (j = 0; j < SIZE - 1 - i; j++) {
			if (sumOfCovering[j] < sumOfCovering[j + 1]) {
				tempNum1 = sumOfCovering[j];
				sumOfCovering[j] = sumOfCovering[j + 1];
				sumOfCovering[j + 1] = tempNum1;

				tempNum2 = PI_index[j];
				PI_index[j] = PI_index[j + 1];
				PI_index[j + 1] = tempNum2;
			}
		}
	}

	/* 가장 많이 minterm을 커버하는 PI부터 차례로 모든 minterm을 다 커버할 때까지 PI를 1개씩 추가 */
	for (i = 0; i < SIZE; i++) {
		if (coverOrNot(trueMintermCover, trueNumber) == 0) { // 전부 커버하지 못했을 때		
			for (m = 0; m < trueNumber; m++)
				if (trueMintermCover[m] == 0) // 커버하지 못한 부분에서 멈춤
					break;

			n = 0;
			while (prime[PI_index[n]].covering[m] != 1) // 멈춘 부분에서 minterm을 커버하는 PI 찾기
				n++;
			
			for (j = 0; j < trueNumber; j++)
				trueMintermCover[j] += prime[PI_index[n]].covering[j]; // 방금 찾은 PI가 커버하는 것들을 전부 추가

			strcpy(essentialPI[i].bit, prime[PI_index[n]].bit); // essential PI 배열에 저장
			essentialPI[i].flag = 1;
		}
	}

	cost = calculateCost(essentialPI, bitLength); // essential PI로 구성할 때 필요한 트랜지스터 개수 계산

	i = 0;
	while (essentialPI[i].flag == 1) { // 모든 essential PI 출력	
		fprintf(fp2, "%s\n", essentialPI[i].bit);
		i++;
	}
	fprintf(fp2, "\nCost(#of transistors): %d\n", cost); // 필요한 트랜지스터 개수 출력

	memoryRelease(arr, length); // 메모리 해제
	memoryRelease(arr1, length); // 메모리 해제
	fclose(fp1); // 스트림 종료
	fclose(fp2); // 스트림 종료
	return 0;
}

/* minterm의 1의 개수 세는 함수 */
int countOne(char a[], int length) {
	int num = 0;
	int i;

	for (i = 0; i < length + 2; i++) {
		if (a[i] == '1') num++;
	}
	return num;
}

/* 메모리 동적 할당하는 함수 */
Minterm** memoryAllocate(int width, int length) {
	Minterm **arr;
	int i;

	if ((arr = (Minterm**)calloc(length, sizeof(Minterm*))) == NULL) { // 열의 값만큼 동적 할당	
		printf("메모리 할당 실패\n");
		exit(1);
	}
	for (i = 0; i < length; i++) {
		if ((arr[i] = (Minterm*)calloc(width, sizeof(Minterm))) == NULL) { // 한 번더 행의 값만큼 동적 할당		
			printf("메모리 할당 실패\n");
			exit(1);
		}
	}
	return arr;
}

/* 할당한 메모리 해제하는 함수 */
int memoryRelease(Minterm** arr, int length) {
	int i;

	/* 2중 해제해야 함 */
	for (i = 0; i < length; i++)
		free(arr[i]);

	free(arr);
	return 0;
}

/* '\0' 값을 없애기 위한 함수 */
void removeBSN(char str[]) {
	int len = strlen(str);
	str[len - 1] = 0;
}

/* PI 찾기 위한 함수 */
void findPrime(Minterm** arr, Minterm* prime, int width, int length, int *primePointer) {
	int i, j;

	for (i = 0; i < length; i++) {
		for (j = 0; j < width; j++) {
			if (arr[i][j].flag == 1 && arr[i][j].usage == 0) { // 비교에 사용은 되었지만, 쓸모가 없었을 때의 것들을 PI에 저장			
				strcpy(prime[*primePointer].bit, arr[i][j].bit);
				prime[*primePointer].flag = 1;
				(*primePointer)++;
			}
		}
	}
}

/* 비교 완료한 것들을 이전 배열에 저장하고, 나머지는 다 초기화하는 함수 */
void copyNPaste(Minterm** arr, Minterm** arr1, int width, int length) {
	int i, j;

	/* 첫 번째 배열을 전부 0으로 초기화 */
	for (i = 0; i < length; i++) {
		for (j = 0; j < width; j++)	{
			strcpy(arr[i][j].bit, "0");
			arr[i][j].usage = 0;
			arr[i][j].flag = 0;
		}
	}

	/* 첫 번째 배열에 두 번째 배열 값들을 복사 */
	for (i = 0; i < length; i++) {
		for (j = 0; j < width; j++) {
			strcpy(arr[i][j].bit, arr1[i][j].bit);
			arr[i][j].flag = arr1[i][j].flag;
		}
	}

	/* 두 번째 배열을 전부 0으로 초기화 */
	for (i = 0; i < length; i++) {
		for (j = 0; j < width; j++) {
			strcpy(arr1[i][j].bit, "0");
			arr1[i][j].usage = 0;
			arr1[i][j].flag = 0;
		}
	}
}

/* minterm이 전부 커버가 되었는지 확인하는 함수 */
int coverOrNot(int *trueMintermCover, int trueNumber) {
	int i;
	int flag = 1;

	for (i = 0; i < trueNumber; i++) {
		if (trueMintermCover[i] == 0) // minterm이 하나라도 커버가 안되었을 경우
			flag = 0;
	}
	return flag;
}

/* essential PI로 구성할 때 필요한 트랜지스터 개수 계산 */
int calculateCost(Minterm *essentialPI, int bitLength) {
	int i = 0;
	int j;
	int count = 0; // 총 PI의 개수
	int cost = 0; // 트랜지스터 개수
	int input; // '-'를 제외한 input 개수

	while (essentialPI[i].flag == 1) {
		input = bitLength;
		for (j = 0; j < bitLength; j++)	{			
			if (essentialPI[i].bit[j] == '-') // input이 '-'인 것은 제외
				input--;
			if (essentialPI[i].bit[j] == '0') // 0일 경우 NOT gate를 추가해야 해서 트랜지스터 2개 추가
				cost += 2;
		}
		if (input >= 2) // input이 2개 이상일 경우 AND gate 사용
			cost += (input * 2) + 2; // 각각 PI의 AND gate 구성할 때 필요한 트랜지스터 개수 계산
		count++; // 총 PI의 개수 1 증가
		i++;
	}
	if (count >= 2) // PI이 2개 이상일 경우 OR gate 사용
		cost += count * 2 + 2; // PI로 OR gate 구성할 때 필요한 트랜지스터 개수 계산
	return cost;
}