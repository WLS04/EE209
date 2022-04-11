// 20200628 진우결
// 과제 2
// sgrep.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for skeleton code */
#include <unistd.h> /* for getopt */
#include "str.h"

#define MAX_STR_LEN 1023
#define FALSE 0
#define TRUE  1

/*
 * Fill out your own functions here (If you need)
 */
char *StrSplit(char *str, char *del)
//문자열 str과 제거할 문자 del을 받은 후 del을 '\0'을 설정하여 del기준으로 문자열을 구분
//string.h의 함수 strtok를 참고
{
  char *str_Start = 0; //시작 주소 지정
  char *save; //시작 주소 임시 저장
  static char *str_NEXT; //정적 변수를 이용하여 값이 계속 저장되도록 함
  /* del을 찾은 후 NULL이 입력되면 그다음 str_Start에 str_NEXT를 지정하기 위해 이용 */ 
  if(str) //입력된 문자열이 NULL이 아니라면 시작 주소를 입력된 문자열의 주소로 지정
    str_Start=str;
  else //만약 NULL이 입력된다면 이전에 저장해두었던 str_NEXT에서 시작
    str_Start=str_NEXT;

  if(StrGetLength(str_Start)<1)
    return NULL;

  save = str_Start; //str_Start를 한 칸씩 증가시키며 이용하므로 처음 주소를 save에 저장
  while(*str_Start) //str_Start가 끝날때까지 del이 존재하는지 확인
  {
    if(*str_Start==(*del)) //만약 str_Start문자열의 문자가 del과 같다면 del을 '\0'
    {
    *str_Start='\0';
    break;      
    }
    str_Start++;
  }
  str_Start++; //del을 바꾼 후 그 다음 주소를 str_NEXT에 저장
  str_NEXT=str_Start;
  return save; //처음에 저장해둔 시작 주소 반환
}
/*--------------------------------------------------------------------*/
/* PrintUsage()
   print out the usage of the Simple Grep Program                     */
/*--------------------------------------------------------------------*/
void
PrintUsage(const char* argv0)
{
  const static char *fmt =
	  "Simple Grep (sgrep) Usage:\n"
	  "%s pattern [stdin]\n";

  printf(fmt, argv0);
}
/*-------------------------------------------------------------------*/
/* SearchPattern()
   Your task:
   1. Do argument validation
   - String or file argument length is no more than 1023
   - If you encounter a command-line argument that's too long,
   print out "Error: pattern is too long"

   2. Read the each line from standard input (stdin)
   - If you encounter a line larger than 1023 bytes,
   print out "Error: input line is too long"
   - Error message should be printed out to standard error (stderr)

   3. Check & print out the line contains a given string (search-string)

   Tips:
   - fgets() is an useful function to read characters from file. Note
   that the fget() reads until newline or the end-of-file is reached.
   - fprintf(sderr, ...) should be useful for printing out error
   message to standard error

   NOTE: If there is any problem, return FALSE; if not, return TRUE  */
/*-------------------------------------------------------------------*/
int
SearchPattern(const char *pattern)
{
  char buf[MAX_STR_LEN + 2];

  /*
   *  TODO: check if pattern is too long
   */
  /* pattern의 길이가 MAX_STR_LEN보다 크면 오류 */
  if (StrGetLength(pattern)>MAX_STR_LEN)
  {
    fprintf(stderr, "Error: pattern is too long\n");
    return (EXIT_FAILURE);
  }
  /* Read one line at a time from stdin, and process each line */
  while (fgets(buf, sizeof(buf), stdin)) /* 파일에서 한 줄씩 읽음 */
  {
    /* buf의 길이가 MAX_STR_LEN보다 크면 오류 출력 */
    if(StrGetLength(buf)>MAX_STR_LEN)
      {
      fprintf(stderr, "Error: input line is too long\n");
      return (EXIT_FAILURE);
      }

    if(StrFindChr(pattern, '*')) /* pattern 내부에 *기호가 존재하는 경우 */
    {
      const char *save;
      char pattern_T[1025] = "\0"; /* 테스트용 pattern_T 지정 + 초기화 */
      save = pattern; /* pattern의 시작 주소를 저장 */
      int i = 0;
      while (*pattern) /* pattern이 끝나지 않았다면 */
      {
        pattern_T[i] = *pattern; /* 테스트용 pattern_T에 입력된 pattern */
        i++;
        pattern++;
      }
      pattern = save; /* 패턴 시작주소 다시 저장 */
      //pattern_T[i]=0;
      char *ptr = StrSplit(pattern_T, "*"); 
      /* '*' 기호 기준으로 패턴 나누고 첫 번째 나누어진 부분을 ptr에 저장 */
      char *lines = buf; 
      /* 먼저 buf의 시작 주소를 저장 */
      while (ptr && StrFindStr(lines, ptr))
      /* ptr이 존재하고 lines에 ptr이 존재하면 통과 */
      {
        lines = StrFindStr(lines, ptr)+StrGetLength(ptr);
        /* StrFindStr이 lines 문자열 내에서 ptr의 시작주소를 반환하므로 그 다음 ptr은
        [반환한 주소+ptr길이] 이후의 주소에서 찾아야 한다. */
        ptr = StrSplit(NULL, "*");
        /* 그다음 ptr 저장('*'로 구분됨) */
      }
      if (ptr == NULL)
        fprintf(stdout, "%s", buf); 
        /*ptr이 NULL이라면, 즉 패턴을 다 통과했다면 그 줄을 그대로 출력*/
    }

    else /*pattern내부에 '*'기호가 존재하지 않는 경우*/
    {
      if(StrFindStr(buf, pattern))
        fprintf(stdout, "%s", buf); 
        /* 패턴이 존재하면 buf를 그대로 출력. 이 과정에서 만들었던 StrFindStr 이용*/
    }
  }

  return TRUE;
}
/*-------------------------------------------------------------------*/
int
main(const int argc, const char *argv[])
{
  /* Do argument check and parsing */
  if (argc < 2) {
	  fprintf(stderr, "Error: argument parsing error\n");
	  PrintUsage(argv[0]);
	  return (EXIT_FAILURE);
  }

  return SearchPattern(argv[1]) ? EXIT_SUCCESS:EXIT_FAILURE;
}
