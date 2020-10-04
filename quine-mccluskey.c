#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 1000

typedef struct minterm {
	char bit[40]; // ������ bit�� ������ �迭 ����
	int flag; // �� minterm�� ���Ǿ����� Ȯ���ϱ� ���� ����
	int usage; // �� minterm�� ���� �� ���Ǿ����� Ȯ���ϱ� ���� ����
	int covering[40]; // �� minterm�� � ���� Ŀ���ϴ����� �����ϱ� ���� ����
	int length;
} Minterm;

int countOne(char a[], int length); // minterm�� 1�� ���� ���� �Լ�
Minterm** memoryAllocate(int width, int length); // �޸� ���� �Ҵ��ϴ� �Լ�
int memoryRelease(Minterm** arr, int length); // �Ҵ��� �޸� �����ϴ� �Լ�
void removeBSN(char str[]); // '\0' ���� ���ֱ� ���� �Լ�
void findPrime(Minterm** arr, Minterm* prime, int width, int length, int *primePointer); // PI ã�� ���� �Լ�
void copyNPaste(Minterm** arr, Minterm** arr1, int width, int length); // �� �Ϸ��� �͵��� ���� �迭�� �����ϰ�, �������� �� �ʱ�ȭ�ϴ� �Լ� 
int coverOrNot(int *trueMintermCover, int trueNumber); // minterm�� ���� Ŀ���� �Ǿ����� Ȯ���ϴ� �Լ�
int calculateCost(Minterm *essentialPI, int bitLength); // essential PI�� ������ �� �ʿ��� Ʈ�������� ���� ����ϴ� �Լ�

int main(void) {
	FILE * fp1 = fopen("input_minterm.txt", "rt"); // input ���� ����
	FILE * fp2 = fopen("result.txt", "wt"); // output ���� ����

	Minterm **arr; // minterm�� ������ ������ �迭
	Minterm **arr1; // minterm�� ������ ������ �迭
	Minterm temp[SIZE] = { 0 }; // input ���Ϸκ��� ���� ���� �ӽ÷� ������ �迭
	Minterm trueMinterm[SIZE] = { 0 }; // true minterm�� ������ �迭
	Minterm prime[SIZE] = { 0 }; // PI�� ������ �迭
	Minterm essentialPI[SIZE] = { 0 }; // essential PI�� ������ �迭
	int trueMintermCover[SIZE] = { 0 }; // essential PI�� ã�� ���� PI�� �� �迭�� ���� Ŀ��(1�� ��������)�ϴ��� Ȯ���ϱ� ���� �迭
	int sumOfCovering[SIZE] = { 0 }; // PI�� Ŀ���ϴ� minterm�� ������ ������ �迭
	int PI_index[SIZE] = { 0 }; // �迭���� PI�� ��ġ�� ������ �迭
	int trueNumber = 0; // true minterm�� ������ ������ ����
	int bitLength; // bit ���� ������ ����
	int inputNumber = 0; // �Էµ� minterm�� ������ ������ ����
	int i, j, k, m, n, r; // ���� loop�� ���� ����
	int width, length; // ���� �迭�� ���ο� ������ ���̸� �����ϴ� ����
	int cal; // minterm�� 1�� ������ ������ ����
	char temp1[SIZE] = { 0 }; // minterm�� ���� �ӽ÷� ������ �迭
	int count = 0; // �� minterm������ bit�� �޶� �� bit�� '-'�� �ٲ� Ƚ�� ������ ����
	int index; // �迭�� index �� ������ ����
	int tempNum1, tempNum2; // �迭 ���������� �� ���� �ӽ÷� �����ϱ� ���� ����
	int compare; // PI�� minterm�� Ŀ���ϴ����� ������ ����
	int cost; // �ʿ��� Ʈ�������� ������ ������ ����
	int primeIndex = 0; // prime �迭������ index ���� ������ �迭
	int * primePointer = &primeIndex; // primeIndex�� ����Ű�� ������ ����

	i = 0;
	while (feof(fp1) == 0) { // input ������ ������ �о����	
		fgets(temp[i].bit, sizeof(temp[i].bit), fp1); // temp�迭�� �ӽ÷� ���� ���� ����
		removeBSN(temp[i].bit); // '\0' �� ����

		/* true minterm�� ���� ���� */
		if (temp[i].bit[0] == 'm') {
			strcpy(trueMinterm[trueNumber].bit, temp[i].bit);
			trueNumber++;
		}
		i++;
		inputNumber++; // �Էµ� input�� ���� count
	}

	bitLength = atoi(temp[0].bit); // bit ���� ����

	/* bit ���̸� ó���� �Է����� ���� ��� */
	if (temp[0].bit[0] == 'd' || temp[0].bit[0] == 'm') {
		fprintf(fp2, "bit ���̸� ó���� �Է����ֽʽÿ�\n");
		return 0;
	}		

	/* �Է��� bit ���̿� ���� minterm�� bit ���̰� �ٸ� ��� */
	for (i = 1; i < inputNumber; i++) {
		if (temp[i].bit[bitLength + 2] != '\0') {
			fprintf(fp2, "�Է��� bit ���̿� �°� minterm�� �Է����ֽʽÿ�\n");
			return 0;
		}
	}

	width = inputNumber; // �Է� ���� input�� ������ implicant table�� ���� ���̿� ����
	length = bitLength + 1; // �Է� ���� bit ���� + 1�� implicant table�� ���� ���̿� ����

	arr = memoryAllocate(width, length); // �޸� �Ҵ�
	arr1 = memoryAllocate(width, length); // �޸� �Ҵ�


	/* minterm�� 1�� ������ ���� �з��Ͽ� implicant table�� ���� */
	for (i = 1; i < inputNumber - 1; i++) {
		cal = countOne(temp[i].bit, bitLength); // minterm�� 1�� ������ count
		for (j = 0; j < width; j++) {
			if (arr[cal][j].flag == 0) { // �����Ϸ��� ��ġ�� �ٸ� ���� ���� ��� ����			
				for (n = 0; n < bitLength; n++) {
					arr[cal][j].bit[n] = temp[i].bit[n + 2];
				}
				arr[cal][j].flag = 1; // �� �ڸ����� ���� ����Ǿ� �ִٴ� ���� �˸�
				j = width; // loop�� Ż��
			}
		}
	}

	for (k = 0; k < bitLength; k++) {
		for (i = 0; i < length - 1; i++) {
			for (n = 0; n < width; n++) {
				index = 1;
				if (arr[i][n].flag == 0) // �ش� ��ġ�� minterm�� ���� �� ���� ������ ���� ���� loop Ż��
					n = width;
				else {
					strcpy(temp1, arr[i][n].bit); // �� ����� �ӽ÷� ����
					for (j = 0; j < width; j++) { // ���� ���� ��� ��ҿ� ��					
						if (i + index > length - 1) { // ���� ���� �������� ���� �� �� �ߴ��ϱ� ���� loop Ż��						
							j = width;
							n = width;
							i = length;
						}
						else {
							if (arr[i + index][0].flag == 0) { // �ش� ��ġ�� �ٷ� ���� �� ù��° ��ҿ� minterm�� ������ ���� ������ �Ѿ							
								if (i + index < length - 1) {
									index++;
									j = 0;
								}
							}
							else if (arr[i + index][j].flag == 0) { // �࿡ �� �̻� ���� minterm�� ���� �� ���� ������ �Ѿ							
								j = width;
							}
							else {
								/* �� minterm ���̿��� bit ������ ���� �� */
								for (m = 0; m < bitLength; m++) {
									if (temp1[m] == '-' || arr[i + index][j].bit[m] == '-')	{
										if (temp1[m] == '-' && arr[i + index][j].bit[m] == '-') // �� bit ��� '-'�� ��� ���� bit ��
											continue;
										else { // PI�� ������ ���� �ٸ� minterm�� ���ؾ��Ѵ�.										
											m = bitLength;
											count = 0;
										}
									}
									else if (temp1[m] != arr[i + index][j].bit[m]) { // ������ bit�� �ٸ� ��									
										temp1[m] = '-';
										count++;
									}
								}
								if (count == 1) { // �� bit�� �޶��� ��								
									r = 0;
									while (arr1[i][r].flag != 0) {
										r++;
									}
									strcpy(arr1[i][r].bit, temp1); // �ٸ� �׷쿡 ����
									arr[i][n].usage = 1; // �񱳵Ǿ��� minterm�� üũ
									arr[i + index][j].usage = 1; // �񱳵Ǿ��� minterm�� üũ
									arr1[i][r].flag = 1; // minterm�� �����Ѵٴ� ���� �˸�
									r = 0;
								}
								count = 0;
								strcpy(temp1, arr[i][n].bit); // �ٽ� ���ϱ� ���� ���� ������ �ʱ�ȭ
							}
						}
					}
				}
			}
		}
		findPrime(arr, prime, width, length, primePointer); // PI ã�� ���� �Լ�
		copyNPaste(arr, arr1, width, length); // �� �Ϸ��� �͵��� ���� �迭�� �����ϰ�, �������� �� �ʱ�ȭ�ϴ� �Լ�
	}

	/* �ߺ��Ǵ� prime implicant�� �ɷ��� */
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE - 1 - i; j++) {
			if (!strcmp(prime[SIZE - 1 - i].bit, prime[j].bit)) {
				prime[SIZE - 1 - i].flag = 0;
			}
		}
	}

	/* PI�� � minterm�� Ŀ���ϴ����� ���� */
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

	/* PI�� Ŀ���ϴ� minterm�� ���� ���� */
	for (i = 0; i < SIZE; i++) {
		if (prime[i].flag == 0)
			continue;
		else {
			for (j = 0; j < trueNumber; j++) {
				sumOfCovering[i] += prime[i].covering[j];
			}
		}
	}

	/* PI_index �迭�� 1���� 1�� �����Ͽ� ���� */
	for (i = 0; i < SIZE; i++)
		PI_index[i] = i;

	/* PI�� Ŀ���ϴ� minterm�� ������ ������������ �����ϰ�, �� PI�� index�� ���� ���������ؼ� ��ġ�� �� �� �ְ� �� */
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

	/* ���� ���� minterm�� Ŀ���ϴ� PI���� ���ʷ� ��� minterm�� �� Ŀ���� ������ PI�� 1���� �߰� */
	for (i = 0; i < SIZE; i++) {
		if (coverOrNot(trueMintermCover, trueNumber) == 0) { // ���� Ŀ������ ������ ��		
			for (m = 0; m < trueNumber; m++)
				if (trueMintermCover[m] == 0) // Ŀ������ ���� �κп��� ����
					break;

			n = 0;
			while (prime[PI_index[n]].covering[m] != 1) // ���� �κп��� minterm�� Ŀ���ϴ� PI ã��
				n++;
			
			for (j = 0; j < trueNumber; j++)
				trueMintermCover[j] += prime[PI_index[n]].covering[j]; // ��� ã�� PI�� Ŀ���ϴ� �͵��� ���� �߰�

			strcpy(essentialPI[i].bit, prime[PI_index[n]].bit); // essential PI �迭�� ����
			essentialPI[i].flag = 1;
		}
	}

	cost = calculateCost(essentialPI, bitLength); // essential PI�� ������ �� �ʿ��� Ʈ�������� ���� ���

	i = 0;
	while (essentialPI[i].flag == 1) { // ��� essential PI ���	
		fprintf(fp2, "%s\n", essentialPI[i].bit);
		i++;
	}
	fprintf(fp2, "\nCost(#of transistors): %d\n", cost); // �ʿ��� Ʈ�������� ���� ���

	memoryRelease(arr, length); // �޸� ����
	memoryRelease(arr1, length); // �޸� ����
	fclose(fp1); // ��Ʈ�� ����
	fclose(fp2); // ��Ʈ�� ����
	return 0;
}

/* minterm�� 1�� ���� ���� �Լ� */
int countOne(char a[], int length) {
	int num = 0;
	int i;

	for (i = 0; i < length + 2; i++) {
		if (a[i] == '1') num++;
	}
	return num;
}

/* �޸� ���� �Ҵ��ϴ� �Լ� */
Minterm** memoryAllocate(int width, int length) {
	Minterm **arr;
	int i;

	if ((arr = (Minterm**)calloc(length, sizeof(Minterm*))) == NULL) { // ���� ����ŭ ���� �Ҵ�	
		printf("�޸� �Ҵ� ����\n");
		exit(1);
	}
	for (i = 0; i < length; i++) {
		if ((arr[i] = (Minterm*)calloc(width, sizeof(Minterm))) == NULL) { // �� ���� ���� ����ŭ ���� �Ҵ�		
			printf("�޸� �Ҵ� ����\n");
			exit(1);
		}
	}
	return arr;
}

/* �Ҵ��� �޸� �����ϴ� �Լ� */
int memoryRelease(Minterm** arr, int length) {
	int i;

	/* 2�� �����ؾ� �� */
	for (i = 0; i < length; i++)
		free(arr[i]);

	free(arr);
	return 0;
}

/* '\0' ���� ���ֱ� ���� �Լ� */
void removeBSN(char str[]) {
	int len = strlen(str);
	str[len - 1] = 0;
}

/* PI ã�� ���� �Լ� */
void findPrime(Minterm** arr, Minterm* prime, int width, int length, int *primePointer) {
	int i, j;

	for (i = 0; i < length; i++) {
		for (j = 0; j < width; j++) {
			if (arr[i][j].flag == 1 && arr[i][j].usage == 0) { // �񱳿� ����� �Ǿ�����, ���� ������ ���� �͵��� PI�� ����			
				strcpy(prime[*primePointer].bit, arr[i][j].bit);
				prime[*primePointer].flag = 1;
				(*primePointer)++;
			}
		}
	}
}

/* �� �Ϸ��� �͵��� ���� �迭�� �����ϰ�, �������� �� �ʱ�ȭ�ϴ� �Լ� */
void copyNPaste(Minterm** arr, Minterm** arr1, int width, int length) {
	int i, j;

	/* ù ��° �迭�� ���� 0���� �ʱ�ȭ */
	for (i = 0; i < length; i++) {
		for (j = 0; j < width; j++)	{
			strcpy(arr[i][j].bit, "0");
			arr[i][j].usage = 0;
			arr[i][j].flag = 0;
		}
	}

	/* ù ��° �迭�� �� ��° �迭 ������ ���� */
	for (i = 0; i < length; i++) {
		for (j = 0; j < width; j++) {
			strcpy(arr[i][j].bit, arr1[i][j].bit);
			arr[i][j].flag = arr1[i][j].flag;
		}
	}

	/* �� ��° �迭�� ���� 0���� �ʱ�ȭ */
	for (i = 0; i < length; i++) {
		for (j = 0; j < width; j++) {
			strcpy(arr1[i][j].bit, "0");
			arr1[i][j].usage = 0;
			arr1[i][j].flag = 0;
		}
	}
}

/* minterm�� ���� Ŀ���� �Ǿ����� Ȯ���ϴ� �Լ� */
int coverOrNot(int *trueMintermCover, int trueNumber) {
	int i;
	int flag = 1;

	for (i = 0; i < trueNumber; i++) {
		if (trueMintermCover[i] == 0) // minterm�� �ϳ��� Ŀ���� �ȵǾ��� ���
			flag = 0;
	}
	return flag;
}

/* essential PI�� ������ �� �ʿ��� Ʈ�������� ���� ��� */
int calculateCost(Minterm *essentialPI, int bitLength) {
	int i = 0;
	int j;
	int count = 0; // �� PI�� ����
	int cost = 0; // Ʈ�������� ����
	int input; // '-'�� ������ input ����

	while (essentialPI[i].flag == 1) {
		input = bitLength;
		for (j = 0; j < bitLength; j++)	{			
			if (essentialPI[i].bit[j] == '-') // input�� '-'�� ���� ����
				input--;
			if (essentialPI[i].bit[j] == '0') // 0�� ��� NOT gate�� �߰��ؾ� �ؼ� Ʈ�������� 2�� �߰�
				cost += 2;
		}
		if (input >= 2) // input�� 2�� �̻��� ��� AND gate ���
			cost += (input * 2) + 2; // ���� PI�� AND gate ������ �� �ʿ��� Ʈ�������� ���� ���
		count++; // �� PI�� ���� 1 ����
		i++;
	}
	if (count >= 2) // PI�� 2�� �̻��� ��� OR gate ���
		cost += count * 2 + 2; // PI�� OR gate ������ �� �ʿ��� Ʈ�������� ���� ���
	return cost;
}