// 20200628 진우결
// 과제 2
// str.c
// 최종
#include <assert.h> /* to use assert() */
#include <stdio.h>
#include <stdlib.h> /* for strtol() */
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "str.h"

/* Your task is: 
   1. Rewrite the body of "Part 1" functions - remove the current
      body that simply calls the corresponding C standard library
      function.
   2. Write appropriate comment per each function
*/

/* Part 1 */
/*------------------------------------------------------------------------*/
// 문자열의 길이를 '\0'전까지 읽어서 반환
size_t StrGetLength(const char* pcSrc)
{
  const char *pcEnd;
  assert(pcSrc); /* NULL address, 0, and FALSE are identical. */
  pcEnd = pcSrc;
	
  while (*pcEnd) /* null character and FALSE are identical. */
    pcEnd++;

  return (size_t)(pcEnd - pcSrc);
}

/*------------------------------------------------------------------------*/
// 문자열을 pcSrc로부터 복사하여 pcDest에 붙여넣음
char *StrCopy(char* pcDest, const char* pcSrc)
{
  assert(pcDest); //assert로 입력된 pcDest, pcSrc이 NULL인지 확인
  assert(pcSrc);                                                                                                                                                 
  char *SAVE;
  SAVE = pcDest; //pcDest 시작 주소를 SAVE에 저장
  while (*pcSrc) //pcSrc가 \0일때까지
  {
    *pcDest = *pcSrc; //COPY의 각 값에 pcSrc값 저장
    pcSrc++; //pcSrc, COPY 한 칸씩 이동
    pcDest++;
  }
  *pcDest='\0'; //COPY마지막 주소에 \0 저장
  return (char*)SAVE; //SAVE에 저장했던 시작 주소 반환
}

/*------------------------------------------------------------------------*/
// 두 문자열을 받은 후 앞에서부터 아스키코드값으로 비교하여 어떤쪽이 더 큰지 반환
int StrCompare(const char* pcS1, const char* pcS2)
{
  assert(pcS1); //assert로 입력된 pcS1, pcS2가 NULL인지 확인
  assert(pcS2);
  while (*pcS1 || *pcS2) //pcS1, pcS2가 모두 끝나기 전까지 비교
  {
    int compare = (*pcS1-*pcS2); //앞에서부터 각 주소에서의 값에 대해 아스키코드 값 차이 저장
    if (compare) //차이가 존재하면 그 차이만큼 반환
      return compare;
    pcS1++;
    pcS2++;
  }
  return 0; //pcS1, pcS2 모두 끝까지 돌면서 차이가 없다면 0반환
}

/*------------------------------------------------------------------------*/
// 문자열 내에서 int c를 통해 받은 문자를 찾고 처음으로 찾은 주소를 반환
char *StrFindChr(const char* pcHaystack, int c)
{
  assert(pcHaystack); //assert로 입력된 pcHaystack이 NULL인지 확인
  while (*pcHaystack != (char)c) //다르면 계속 while문 반복
  {
    if (*pcHaystack == '\0') //반복하다가 pcHaystack 문자열의 끝부분 만나면 NULL반환
      return NULL;
    pcHaystack++;
  }
  return (char*)pcHaystack; //같으면 while문 탈출 후 처음으로 같은 문자가 발견된 주소 반환
}

/*------------------------------------------------------------------------*/
// 문자열 pcHaystack내에서 pcNeedle을 찾고, 존재하면 시작 지점을 반환, 없으면 NULL 반환
// 만약 pcNeedle이 비었다면 그대로 반환
char *StrFindStr(const char* pcHaystack, const char *pcNeedle)
{
  assert(pcHaystack); //assert로 입력된 pcHaystack과 pcNeedle이 NULL인지 확인
  assert(pcNeedle);
  
  if (*pcNeedle == '\0') //찾는 문자열이 비었다면 주소 그대로 반환
    return (char*)pcHaystack;

  const char * test_str; //테스트 변수 test_str, test_find
  const char * test_find;
  while (*pcHaystack) //pcHaystack 값이 끝날때까지
  {
    if (*pcHaystack==*pcNeedle) //만약 첫 문자가 같다면
    {
      test_str = pcHaystack; 
      test_str++; //test_str에 그 다음 주소 저장
      test_find = pcNeedle;
      test_find++; //test_find에 그 다음 주소 저장
      /* 그 다음 주소를 임시로 저장해서 그 주소 이후에 계속 똑같은지 확인, 아니면 되돌아옴 */
      while (*test_str && *test_find && *test_str == *test_find)
      /* test_str, test_find의 값이 존재하고 그 값이 계속 같다면 진행, 다르면 빠져나감 */
      {
        test_str++;
        test_find++;
      }
      /* test_find가 끝까지 돌았다면 pcNeedle을 찾은 것과 같음 */
      if (*test_find == '\0')
        return (char*)pcHaystack; //아까 수정하지 않았던 시작주소 반환
      /* test_find는 끝까지 돌지 않고 test_str이 끝까지 돌았다면 pcNeedle을 찾다가 끝남 */
      if (*test_str == '\0')
        return (char*)NULL; //pcHaystack에 pcNeedle이 없으니깐 NULL반환
      /* 만약 둘다 끝까지 돌지 못했다면 값이 서로 달라서 중단된 것 */
    }
    pcHaystack++; //일치하지 않으므로 주소 한칸 더 이동
  }
  return (char*)NULL; //시작 글자도 안 겹치면 NULL;
}
/*------------------------------------------------------------------------*/
// pcSrc 문자열을 pcDest의 문자열 뒤로 이어붙임
char *StrConcat(char *pcDest, const char *pcSrc)
{
  assert(pcSrc); //assert로 pcDest와 pcSrc가 NULL인지 확인
  assert(pcDest);
  char *SAVE;
  SAVE = pcDest; //시작 지점의 주소 저장
  while (*pcDest)
  {
    *pcDest = *pcDest; //앞에서부터 pcDest에 저장되어있던 문자 지정
    pcDest++;
  }
  while (*pcSrc) 
  {
    *pcDest = *pcSrc; //pcDest바로 뒤에서부터 pcSrc에 지정된 문자열 붙이기
    pcDest++;
    pcSrc++;
  }
  *pcDest = '\0'; //끝에 NULL
  return (char*)SAVE; //시작 지점 주소 반환
}

/*------------------------------------------------------------------------*/
// 문자열 내에서 십진수를 읽어서 Long int type으로 반환
long int StrToLong(const char *nptr, char **endptr, int base)
{
  /* handle only when base is 10 */
  if (base != 10) return 0; /* 10진수가 아니면 0반환 */

  assert(nptr); /* assert로 nptr이 NULL인지 확인 */

  char *strE = (char*)nptr;
  char *SAVE = strE; /* space부터 반환될 수도 있으므로 미리 저장 */
  int space = 0; /* isspace -> 정수 or isspace -> 문자를 구분하기 위해 이용*/
  while(*strE && isspace(*strE)) /* space/tab은 건너뛰기 */
  {
    strE++;
    space = 1;
  }

  long int LONG = 0; /* 반환할 숫자 */
  long int temp; /* 더할 숫자 */
  int minus = 0; /* 부호 판단. 기본은 0 (0 = 양수) */

  if (*strE == '-')
  {
    minus = 1; /* 만약 마이너스 기호 나오면 minus를 1로 지정 */
    char *strT = strE;
    strT++; /* 그 다음 문자 확인용 */
    if(isdigit(*strT)) /* 마이너스 기호 뒤에 숫자가 나오면 계속 진행 */
      strE++;
  }

  if (*strE == '+') /* 플러스 기호는 뒤에 숫자가 나오면 그대로 진행 */
  {
    char *strT = strE;
    strT++; /* 그 다음 문자 확인용 */
    if(isdigit(*(strT)))
      strE++;
  }

  while(*strE && isdigit(*strE)) /* strE가 존재하고 숫자이면 통과 */
  {
    space = 0;/* isspace가 이어지다가 정수가 나오는지 아니면 문자로 끝나는지 확인 */
    temp = *strE - '0'; /* 새로 더해질 수 */
    long int LONG_S = LONG; /* 이전 값을 저장 */
    int minus_T = 0; /* 부호 테스트용 (오버, 언더플로우) */
    /* 오버플로우나 언더플로우가 발생하면 부호가 변함을 이용 */

    /* 아래 for문 안에서 LONG은 범위를 벗어나면 음수가 됨 */
    for (int i=0;i<(9+temp);i++)
    {
      if (i<9) /* 원래 숫자를 10배하는 과정 */
        LONG = LONG + LONG_S;
      else /* temp를 더하는 과정 */
        LONG = LONG + 1;
      /* 예제처럼 10배 하지 않은 이유는 범위가 여러 번 벗어나면 부호가 중복되어 바뀌기 때문 */
      if(LONG<0) /* 더하는 과정에서 부호가 바뀌면 테스트 부호를 1로 지정 */
        minus_T = 1;
      /* 처음 음수였던 수가 범위를 벗어나면 언더플로우 */
      if(minus_T==1 && minus==1)
      {
        while(*strE && isdigit(*strE))
        {
          strE++; /* 처음으로 숫자가 아닌 부분까지 진행 */
        }
        if(endptr) /* end pointer가 NULL이 아니라면 값 지정 */
          *endptr = strE;
        return LONG_MIN;
      }
      /* 처음 양수였던 수가 범위를 벗어나면 오버플로우 */
      /* 나머지는 위와 동일 */
      if(minus_T==1 && minus==0)
      {  
        while(*strE && isdigit(*strE))
        {
          strE++;
        }
        if(endptr)
          *endptr = strE;
        return LONG_MAX;
      }
    }
    strE++; //그 다음 숫자 읽음
  }
  
  if (endptr) //endptr이 NULL이 아니면 반환 지점 저장
  {
    if (space==0)
      *endptr = strE;
    else
      *endptr = SAVE;
  }
  if (minus == 1) //음수면 -1 곱하기
    return LONG*(-1);
  else
    return LONG; //아니면 LONG 그대로 반환
}