#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Include the header file for 'strndup' function
#include <dirent.h> //"diretory entry"  
					//It is part of the standard library and provides functionalities(기능, in computer science /  function: 함수) for directory manipulation. 
#include <ctype.h>

/* Hi */

#ifndef HAVE_STRNDUP
// Implementation of strndup if not available
char *strndup(const char *s, size_t n) {
    char *p = (char *)malloc(n + 1);
    if (p) {
        strncpy(p, s, n);
        p[n] = '\0';
    }
    return p;
}
#endif

typedef 
	enum {
		C_ZERO,
		C_LIST,
		C_SHOW,
		C_TEST,
		C_EXIT,
	}
	command_t ;


char * read_a_line (FILE * fp)
{
	static char buf[BUFSIZ] ;
	static int buf_n = 0 ;
	static int curr = 0 ;

	if (feof(fp) && curr == buf_n - 1)
		return 0x0 ;

	char * s = 0x0 ;
	size_t s_len = 0 ;
	do {
		int end = curr ;
		while (!(end >= buf_n || !iscntrl(buf[end]))) {
			end++ ;
		}
		if (curr < end && s != 0x0) {
			curr = end ;
			break ;
		}
		curr = end ;
		while (!(end >= buf_n || iscntrl(buf[end]))) {
			end++ ;
		}
		if (curr < end) {
			if (s == 0x0) {
				s = strndup(buf + curr, end - curr) ;
				s_len = end - curr ;
			}
			else {
				s = realloc(s, s_len + end - curr + 1) ;
				s = strncat(s, buf + curr, end - curr) ;
				s_len = s_len + end - curr ;
			}
		}
		if (end < buf_n) {
			curr = end + 1 ;
			break ;
		}

		buf_n = fread(buf, 1, sizeof(buf), fp) ;
		curr = 0 ;
	} while (buf_n > 0) ;
	return s ;
}



void print_menu() {

	printf("1. List all wordbooks\n") ;
	printf("2. Show the words in a wordbook\n") ;
	printf("3. Test with a wordbook\n") ;
	printf("4. Exit\n") ;
}

int get_command() {        //어떤 것을 실행할지 선택하는 함수 get_command()정의 (cmd: 번호로 입력받음)
	int cmd ;

	printf(">") ;
	scanf("%d", &cmd) ;
	return cmd ;
}

void list_wordbooks ()     //wordbooks 폴더에 있는 파일들을 출력하는 함수 list_wordbooks() 정의
{

	DIR * d = opendir("C:\\Users\\green\\Project\\Wordquiz\\wordquiz\\wordquiz-main\\wordbooks");  //절대주소와 상대주소 차이 이해하고 다시 수정하기
	
	printf("\n  ----\n") ;

	struct dirent * wb ;
	while ((wb = readdir(d)) != NULL) {
		if (strcmp(wb->d_name, ".") != 0 && strcmp(wb->d_name, "..") !=0) {
			printf("  %s\n", wb->d_name) ;
		}
	}
	closedir(d) ;

	printf("  ----\n") ;
}

void show_words ()
{
	char wordbook[128] ;  //wordbook이라는 문자열 배열 선언
	char filepath[256] ;  //filepath이라는 문자열 배열 선언

	list_wordbooks() ;

	printf("Type in the name of the wordbook?\n") ;
	printf(">") ;
	scanf("%s", wordbook) ; //wordbooks 파일하고 wordbook 문자열은 다름

	sprintf(filepath, "wordbooks/%s", wordbook) ;  //sprintf 이해안감, filepath에 파일경로가 저장됨

	FILE * fp = fopen(filepath, "r") ;

	printf("\n  -----\n") ;
	char * line ;
	while (line = read_a_line(fp)) {
		char * word = strtok(line, "\"") ;
		strtok(NULL, "\"") ;
		char * meaning = strtok(NULL, "\"") ;

		printf("  %s : %s\n", word, meaning) ;

		free(line) ;
	}
	printf("  -----\n\n") ;

	fclose(fp) ;
}


void run_test ()
{
	char wordbook[128] ;
	char filepath[256] ;

	printf("Type in the name of the wordbook?\n") ;
	printf(">") ;
	scanf("%s", wordbook) ;

	sprintf(filepath, "wordbooks/%s", wordbook) ;

	FILE * fp = fopen(filepath, "r") ;

	printf("\n-----\n") ;

	int n_questions = 0 ;
	int n_correct = 0 ; 

	char * line ;
	while (line = read_a_line(fp)) {
		char * word = strtok(line, "\"") ;
		strtok(NULL, "\"") ;
		char * meaning = strtok(NULL, "\"") ;

		printf("Q. %s\n", meaning) ;
		printf("?  ") ;

		char answer[128] ;
		scanf("%s", answer) ;

		if (strcmp(answer, word) == 0) {
			printf("- correct\n") ;
			n_correct++ ;
		}
		else {
			printf("- wrong: %s\n", word) ;
		}

		n_questions++ ;
		free(line) ;
	}

	printf("(%d/%d)\n", n_correct, n_questions) ;

	printf("-----\n\n") ;

	fclose(fp) ;
}


int main ()
{
	
	printf(" *** Word Quiz *** \n\n") ;

	int cmd ;
	do {
		print_menu() ;

		cmd = get_command() ;
		switch (cmd) {            //swith 문이면 숫자를 매개변수로 받나?
			case C_LIST : {
				list_wordbooks() ;
				break ;
			}

			case C_SHOW: {
				show_words() ;
				break ;
			}

			case C_TEST: {
				run_test() ;
				break ;
			}

			case C_EXIT: {
				return EXIT_SUCCESS ;
			}
		}
	}
	while (cmd != C_EXIT) ;


	return EXIT_SUCCESS ;
}

// 틀린문제 다시 보여주는 기능 추가할 것