# include <stdio.h>
# include <ctype.h>
# include <stdlib.h>
int N = 0; // number of newline
int S = 0; // number of space
int W = 0; // number of new word
int C = 0; // number of normal character (expet * and /)
int errornewline = 0;
void read (void);
void start (void);
void newline (void); // 2
void space (void); // 3
void character (void); // 1
void word (void);// 4
void comment_step1_1 (void);//5
void comment_step1_2 (void);//6

void return_comment_step2_1 (void);//7
void return_comment_step2_2 (void);//8
void return_comment_step2_3 (void);//9
void return_comment_step2_4 (void);//10
void return_comment_step2_5 (void);//11
void return_comment_step2_6 (void);//12

void comment_type1 (void);//13
void comment_type2_step1 (void);//14
void comment_type2_step2 (void);//15
void comment_type2_newline (void);//16
void comment_type2_escape (void);//17

char chr;


// every function exept comment type 2 : EOF -> end(success)
void read (void)
{
  chr = getchar();
}

// start
void start (void)
{
  read();
  if (chr == '\n') newline();
  if (isspace(chr)) space();
  if (chr == '/') comment_step1_2();
  if (chr == EOF)
    {
      printf("0 0 0\n");
      exit(EXIT_SUCCESS);
    }
  else word();
}

// if newline
void newline (void)
{
  N++;
  read();
  if (chr == '\n') newline();
  if (isspace(chr)) space();
  if (chr == '/') comment_step1_2();
  if (chr == EOF)
    {
      C = C+N+S;
      printf("%d %d %d\n", N, W, C); 
      exit(EXIT_SUCCESS);
    }
 else word();
}

// if space
void space (void)
{
  S++;
  read();
  if (chr == '\n') newline();
  if (isspace(chr)) space();
  if (chr == '/') comment_step1_2();
  if (chr == EOF)
    {
      C = C+N+S;
      printf("%d %d %d\n", N, W, C); 
      exit(EXIT_SUCCESS);
    }
  else word();
}

// if chr changed to space/newline -> normal character (exept/)
void word (void)
{
  W++;
  C++;
  read();
  if (chr == '\n') newline();
  if (isspace(chr)) space();
  if (chr == '/') comment_step1_1();
  if (chr == EOF)
    {
      C = C+N+S;
      printf("%d %d %d\n", N, W, C); 
      exit(EXIT_SUCCESS);
    }
  else character();
}

//  keep normal character
void character (void)
{
  C++;
  read();
  if (chr == '\n') newline();
  if (isspace(chr)) space();
  if (chr == '/') comment_step1_1();
  if (chr == EOF)
    {
      C = C+N+S;
      printf("%d %d %d\n", N, W, C); 
      exit(EXIT_SUCCESS);
    }
  else character();
}

// check it is comment or not (normal character -> /)
void comment_step1_1 (void)
{
  read();
  if (chr == '\n') return_comment_step2_2();
  if (isspace(chr)) return_comment_step2_3();
  if (chr == '/') comment_type1();
  if (chr == '*') comment_type2_step1();
  if (chr == EOF)
    {
      C = C+N+S;
      printf("%d %d %d\n", N, W, C); 
      exit(EXIT_SUCCESS);
    }
  else return_comment_step2_1();
}

// check it is comment or not (space, newline -> /)
void comment_step1_2 (void)
{
  read();
  if (chr == '\n') return_comment_step2_5();
  if (isspace(chr)) return_comment_step2_6();
  if (chr == '/') comment_type1();
  if (chr == '*') comment_type2_step1();
  if (chr == EOF)
    {
      C = C+N+S;
      printf("%d %d %d\n", N, W, C); 
      exit(EXIT_SUCCESS);
    }
  else return_comment_step2_4();
}

// if it is not comment -> return comment
void return_comment_step2_1 (void)
{
  C=C+2;
  read();
  if (chr == '\n') newline();
  if (isspace(chr)) space();
  if (chr == '/') comment_step1_1();
  if (chr == EOF)
    {
      C = C+N+S;
      printf("%d %d %d\n", N, W, C); 
      exit(EXIT_SUCCESS);
    }
  else character();
}

void return_comment_step2_2 (void)
{
  C++;
  N++;
  read();
  if (chr == '\n') newline();
  if (isspace(chr)) space();
  if (chr == '/') comment_step1_2();
  if (chr == EOF)
    {
      C = C+N+S;
      printf("%d %d %d\n", N, W, C); 
      exit(EXIT_SUCCESS);
    }
  else word();
}

void return_comment_step2_3 (void)
{
  C++;
  S++;
  read();
  if (chr == '\n') newline();
  if (isspace(chr)) space();
  if (chr == '/') comment_step1_2();
  if (chr == EOF)
    {
      C = C+N+S;
      printf("%d %d %d\n", N, W, C); 
      exit(EXIT_SUCCESS);
    }
  else word();
}
void return_comment_step2_4 (void)
{
  C=C+2;
  W++;
  read();
  if (chr == '\n') newline();
  if (isspace(chr)) space();
  if (chr == '/') comment_step1_1();
  if (chr == EOF)
    {
      C = C+N+S;
      printf("%d %d %d\n", N, W, C); 
      exit(EXIT_SUCCESS);
    }
  else character();
}

void return_comment_step2_5 (void)
{
  
  C++;
  N++;
  W++;
  read();
  if (chr == '\n') newline();
  if (isspace(chr)) space();
  if (chr == '/') comment_step1_2();
  if (chr == EOF)
    {
      C = C+N+S;
      printf("%d %d %d\n", N, W, C); 
      exit(EXIT_SUCCESS);
    }
  else word();
}

void return_comment_step2_6 (void)
{
  C++;
  S++;
  W++;
  read();
  if (chr == '\n') newline();
  if (isspace(chr)) space();
  if (chr == '/') comment_step1_2();
  if (chr == EOF)
    {
      C = C+N+S;
      printf("%d %d %d\n", N, W, C); 
      exit(EXIT_SUCCESS);
    }
  else word();
}

// if it is comment type 1 (// comment)
void comment_type1 (void)
{
  read();
  if (chr == '\n') newline();
  if (chr == EOF)
    {
      C = C+N+S;
      printf("%d %d %d\n", N, W, C); 
      exit(EXIT_SUCCESS);
    }
  else comment_type1();
}

// if it is comment type 2(/*~*/ comment)

// first, save the newline
void comment_type2_step1 (void)
{
  errornewline = N+1;
  comment_type2_step2();
}

// newline -> count, EOF -> error / other -> keep doing again
void comment_type2_step2 (void)
{
  read();
  if (chr == '\n') comment_type2_newline();
  if (chr == '*') comment_type2_escape();
  if (chr == EOF)
    {
      fprintf(stderr, "Error: line %d: unterminated comment\n", errornewline); 
      exit(EXIT_FAILURE);
    }
  else comment_type2_step2();
}

// if it is newline? -> count / EOF -> error / or going back
void comment_type2_newline (void)
{
  N++;
  read();
  if (chr == '\n') comment_type2_newline();
  if (chr == '*') comment_type2_escape();
  if (chr == EOF)
    {
      fprintf(stderr, "Error: line %d: unterminated comment\n", errornewline); 
      exit(EXIT_FAILURE);
    }
  else comment_type2_step2();
}

// if * entered -> check / EOF -> error / or going back / if escaped -> think it as space
void comment_type2_escape (void)
{
  read();
  if (chr == '\n') comment_type2_newline();
  if (chr == '/') space();
  if (chr == '*') comment_type2_escape();
  if (chr == EOF)
    {
      fprintf(stderr, "Error: line %d: unterminated comment\n", errornewline); 
      exit(EXIT_FAILURE);
    }
  else comment_type2_step2();
}

int main (void)
{
  start();
  return 0;
}
