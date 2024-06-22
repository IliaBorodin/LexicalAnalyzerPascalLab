#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define BUFSIZE 256
#define SIZE 4
const char* fname = "text.txt";
struct tabl {
	int num;
	char** mas;
	int size;
	int capacity;
};
typedef struct tabl* ptabl;
 ptabl TW, TID, TD, TNUM;
char buf[BUFSIZE];
char c;
int d;
int j;
int top;
int line=1;
int column=1;
void clear(void);
void add(void);
int look(ptabl T);
int putl(ptabl T);
int putnum();
void makelex(int k, int n);
void tabls_print();
void scan(void)
{
	typedef enum state
	{
		H, ID, NUM, COM, ASS, DLM, ER, FIN
	}state;
	state TC;
	FILE* fp;
	TC = H;
	fp=fopen(fname, "r");
	if (fp == NULL) {  return -1; }
	c = fgetc(fp);
	do {
		switch (TC) {
		case H:
			if (c == ' ')
				c = fgetc(fp);
			else
				if (isalpha(c)) {
					clear();
					add();
					c = fgetc(fp);
					TC = ID;
				}
				else if (isdigit(c)) {
					clear();
					add();
					//d = c - '0';
					c = fgetc(fp);
					TC = NUM;
				}
				else if (c == '{') {
					c = fgetc(fp);
					TC = COM;
				}
				else if (c == ':' || c == '!') {
					clear();
					add();
					c = fgetc(fp);
					TC = ASS;
				}
				else if (feof(fp) != 0) {
					TC = FIN;
				}
				else TC = DLM;
			break;
		case ID:
			if (isalpha(c) || isdigit(c)) {
				add();
				c = fgetc(fp);
			}
			else {
				if (j = look(TW)) {
					makelex(1, j);
					++column;
				}
				else {
					j = putl(TID);
					makelex(4, j);
					++column;
				}
				TC = H;
			}
			break;
		case NUM:
			if (isdigit(c)) {
				add();
				//d = d * 10 + (c - '0');
				c = fgetc(fp);
			}
			else {
					makelex(3, putnum());
					++column;
					TC = H;
				
				
			}
	        
			break;
		case COM:
			if (c == '}') {
				c = fgetc(fp);
				TC = H;
			}
			else {
				do {
					c = fgetc(fp);
				} while (c!= '}' && feof(fp) == 0);
				if (c == '}') {
					c = fgetc(fp);
					TC = H;
				}
				else {
					printf("\nComment error, line %d column %d. No character '}' detected\n", line, column);
					TC = ER;
				}
			}
			break;
		case ASS:
			if (c == '=') {
				add();
				c = fgetc(fp);
				TC = H;
				makelex(2, look(TD));
				++column;
			}
			else {
				printf("\nError, line %d column %d. Invalid character, expected character '='\n", line, column);
				TC = ER;
			}
			break;
		
		case DLM:
			clear();
			add();
			if (j = look(TD)) {
				c = fgetc(fp);
				makelex(2, j);
				++column;
				TC = H;
			}
			else{
				if (c == '\n') {
					printf("\n");
					++line;
					column = 1;
					c = fgetc(fp);
					TC = H;
				}
				else {
					printf("\nError, line %d column %d. Invalid character\n", line, column);
					TC = ER;
				}
			}
			break;
		}
		

	} while (TC != FIN && TC != ER);
	if (TC == ER) printf("ERROR!!!\n");
	else printf("Lexical analysis is completed\n");
}
//Две таблицы, list1 - таблица TW, list2 - таблица TD
char* list1[] = { "begin","end","do","while","loop","int" };
char* list2[] = { "=","<",">","!=","+","-","*","/",";",":=","(",")","."};
//Вспомогательные функции для структуры tabl
// Инициализация таблицы
void tabl_init(ptabl T) {
	T->capacity = SIZE;
	T->size = 0;
	T->mas = (char**)malloc(SIZE * sizeof(char*));
}
//Получение размера таблицы
int tabl_size(ptabl T) {
	return T->size;
}
//Изменение размера таблицы
void tabl_resize(ptabl T, int newCap) {
	T->capacity = newCap;
	T->mas = realloc(T->mas, newCap * sizeof(char*));
}
//Добавление строки в таблицу из буфера buf
void tabl_push(ptabl T) {
	if (T->size >= T->capacity)
		tabl_resize(T, T->capacity + T->capacity / 2);

	T->mas[T->size] = malloc(top+1);
	memcpy(T->mas[T->size], buf, top+1);

	T->size++;
}
//Получение строки из таблицы по индексу
char* tabl_element_get(ptabl T, int index) {
	if (index < 0 || index > T->size - 1 || T->size <= 0)
		return NULL;

	return T->mas[index];
}


void main() {

	struct tabl tabl1 = { 1,list1,6,6 };
	struct tabl tabl2 = { 2,list2,13,13 };
	struct tabl tabl3 = { 3, };
	struct tabl tabl4 = { 4, };
	TW = &tabl1;
	TD = &tabl2;
	TNUM = &tabl3;
	TID = &tabl4;
	tabl_init(TNUM);
	tabl_init(TID);
	scan();
	tabls_print();




}

void clear(void) {
	memset(buf, 0, top);
	top = 0;
}
void add(void) {
	buf[top] = c;
	++top;
}
int look(ptabl T) {
	for (int i = 0; i < T->size; ++i) {
		if (strcmp(T->mas[i], buf) == 0) {
			return i + 1;
		}
	}
	return 0;
}
int putl(ptabl T) {
	if (look(T) == 0) {
		tabl_push(T);
		return T->size;
	}
	else {
		return look(T);
	}
}
int putnum() {
	if (look(TNUM) == 0) {
		tabl_push(TNUM);
		return TNUM->size;
	}
	else {
		return look(TNUM);
	}
}
void makelex(int k, int i) {
	printf("<%d,%d>\t", k, i);
}
void tabls_print() {
	printf("---------------------\n");
	printf("Table output TW \n");
	for (int i = 0; i < TW->size; ++i) {
		char* str = (char*)tabl_element_get(TW, i);
		printf("Lexeme num %d:  %s\n", i + 1, str);
	}
	printf("---------------------\n");
	printf("Table output TD \n");
	for (int i = 0; i < TD->size; ++i) {
		char* str = (char*)tabl_element_get(TD, i);
		printf("Lexeme num %d:  %s\n", i + 1, str);
	}
	printf("---------------------\n");
	printf("Table output TNUM \n");
	for (int i = 0; i < TNUM->size; ++i) {
		char* str = (char*)tabl_element_get(TNUM, i);
		printf("Lexeme num %d:  %s\n", i + 1, str);
	}
	printf("---------------------\n");
	printf("Table output TID \n");
	for (int i = 0; i < TID->size; ++i) {
		char* str = (char*)tabl_element_get(TID, i);
		printf("Lexeme num %d:  %s\n", i+1, str);
	}

}