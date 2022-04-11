// 20200628 Jinwoogyeol
// Assignment 5
// shell.c
#define _GNU_SOURCE
#define _DEFAULT_SOURCE
#include "dynarray.h"
#include "shell.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/*--------------------------------------------------------------------*/

enum {MAX_LINE_SIZE = 1024};

enum {FALSE, TRUE};

enum TokenType {TOKEN_WORD, TOKEN_SPECIAL};

static void QHandler(int iSig);

/*--------------------------------------------------------------------*/

/* A Token is either a number or a word, expressed as a string. */

struct Token
{
   enum TokenType eType;
   /* The type of the token. */

   char *pcValue;
   /* The string which is the token's value. */
};

/*--------------------------------------------------------------------*/
/* handler for sigint */
void THandler(int iSig)
{
   return;
}

/*--------------------------------------------------------------------*/
/* handler for second ctrl-\ */
void QHandler2(int iSig)
{
   exit(0);
}

/*--------------------------------------------------------------------*/
/* hadler for Alarm */
void AHandler(int iSig)
{
   signal(SIGQUIT, QHandler);
   return;
}

/*--------------------------------------------------------------------*/
/* handler for first ctrl-\ */
static void QHandler(int iSig)
{
   void (*A)(int);
   void (*Q)(int);
   printf("\nType Ctrl-\\ again within 5 seconds to exit.\n");
   Q = signal(SIGQUIT, QHandler2);
   assert(Q!=SIG_ERR);
   alarm(5);
   A = signal(SIGALRM, AHandler);
   assert(A!=SIG_ERR);
   return;
}

/*--------------------------------------------------------------------*/

static void freeToken(void *pvItem, void *pvExtra)

/* Free token pvItem.  pvExtra is unused. */
{
   struct Token *psToken = (struct Token*)pvItem;
   free(psToken->pcValue);
   psToken->pcValue = NULL;
   free(psToken);
   psToken = NULL;
}

/*--------------------------------------------------------------------*/

static struct Token *makeToken(enum TokenType eTokenType,
   char *pcValue)

/* Create and return a Token whose type is eTokenType and whose
   value consists of string pcValue.  Return NULL if insufficient
   memory is available.  The caller owns the Token. */
/* return token */

{
   struct Token *psToken;

   psToken = (struct Token*)malloc(sizeof(struct Token));
   if (psToken == NULL)
      return NULL;

   psToken->eType = eTokenType;

   psToken->pcValue = (char*)malloc(strlen(pcValue) + 1);
   if (psToken->pcValue == NULL)
   {
      free(psToken);
      return NULL;
   }

   strcpy(psToken->pcValue, pcValue);

   return psToken;
}

/*--------------------------------------------------------------------*/

static int lexLine(const char *pcLine, DynArray_T oTokens)

/* Lexical analysis for contents in oTokens */
/* divide word, special character, and check quote is valid */
/* return true or false */

{
   enum LexState {START, WORD, QUOTED1, QUOTED2};

   enum LexState eState = START;

   struct Token *psToken;

   int iLineIndex = 0;
   int iValueIndex = 0;
   char c;
   char acValue[MAX_LINE_SIZE];

   assert(pcLine != NULL);
   assert(oTokens != NULL);

   for (;;)
   {
      /* "Read" the next character from pcLine. */
      c = pcLine[iLineIndex++];

      switch (eState)
      {
         case START:
            if ((c == '\n') || (c == '\0'))
            {
               return TRUE;
            }

            else if (c == '\'')
               eState = QUOTED1;
            
            else if (c == '\"')
               eState = QUOTED2;
            
   else if ((c == '|') || (c == '<') || (c == '>') || (c == '&'))
            {
               acValue[iValueIndex++] = c;
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_SPECIAL, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               iValueIndex = 0;
               eState = START;
            }

            else if (isspace(c))
               eState = START;
            
            else
            {
               acValue[iValueIndex++] = c;
               eState = WORD;
            }
            break;

         case WORD:
            if ((c == '\n') || (c == '\0'))
            {
               /* Create a NUMBER token. */
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               iValueIndex = 0;

               return TRUE;
            }

   else if ((c == '|') || (c == '<') || (c == '>') || (c == '&'))
            {
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               iValueIndex = 0;

               acValue[iValueIndex++] = c;
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_SPECIAL, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               iValueIndex = 0;

               eState = START;
            }
            else if (isspace(c))
            {
               /* Create a NUMBER token. */
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_WORD, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               iValueIndex = 0;

               eState = START;
            }
            else if (c == '\'')
            {
               eState = QUOTED1;
            }
            else if (c == '\"')
            {
               eState = QUOTED2;
            }
            else
            {
               acValue[iValueIndex++] = c;
               eState = WORD;
            }
            break;

         case QUOTED1:
            if ((c == '\n') || (c == '\0'))
            {
               fprintf(stderr, "./ish: Unmatched quote\n");
               fflush(stdout);
               return FALSE;
            }

            else if (c=='\'')
            {
               eState = WORD;
            }

            else
            {
               /* Create a WORD token. */
               acValue[iValueIndex++] = c;
               eState = QUOTED1;
            }
            break;

         case QUOTED2:
            if ((c == '\n') || (c == '\0'))
            {
               fprintf(stderr, "./ish: Unmatched quote\n");
               fflush(stdout);
               return FALSE;
            }

            else if (c=='\"')
            {
               eState = WORD;
            }

            else
            {
               /* Create a WORD token. */
               acValue[iValueIndex++] = c;
               eState = QUOTED2;
            }
            break;

         default:
            assert(FALSE);
      }
   }
}

/*--------------------------------------------------------------------*/
/* make char ** pointer */
char **makeargument()
{
   char **argument = calloc(100, MAX_LINE_SIZE);
   if (argument == NULL)
   {
      fprintf(stderr, "Cannot allocate memory\n");
      return NULL;
   }
   return argument;
}

/*--------------------------------------------------------------------*/
/* check special input */
int check(char *arg)
{
   int a = strcmp(arg, "|");
   int b = strcmp(arg, "<");
   int c = strcmp(arg, ">");
   int d = strcmp(arg, "&");
   if(a==0)
      return 1;
   else if(b==0)
      return 2;
   else if(c==0)
      return 3;
   else if(d==0)
      return 4;
   else
      return 0;
}

/*--------------------------------------------------------------------*/
/* do syntactic analysis. */
/* valid -> return 1, invalid -> return 0 */
int Syntactic(int Len, DynArray_T oTokens)
{
   char **argument = makeargument();
   const void* c;
   int ar = 0;
   int ri = 0;
   int ro = 0;
   int pp = 0;
   int bg = 0;
   for (int i=0;i<Len;i++)
   {
      c = DynArray_get(oTokens, i);
      if (c==NULL)
         break;
      struct Token *psToken = (struct Token*)c;
      argument[i] = psToken->pcValue;
      /* if special */
      if(check(argument[i]) && psToken->eType)
      {
         if (check(argument[i])==1)
         {
            if(ar==0)
            {
               free(argument);
               argument = NULL;
               fprintf(stderr, "./ish: Missing command name\n");
               return 0;
            }
            if(ro == 1)
            {
               free(argument);
               argument = NULL;
               fprintf(stderr
      , "./ish: Standard output redirection without file name\n");
               return 0;
            }
            if(ri == 1)
            {
               free(argument);
               argument = NULL;
               fprintf(stderr
      , "./ish: Standard input redirection without file name\n");
               return 0;
            }
            if(ro == 2)
            {
               free(argument);
               argument = NULL;
               fprintf(stderr
      , "./ish: Multiple redirection of standard out\n");
               return 0;
            }
            if(bg)
            {
               free(argument);
               argument = NULL;
               fprintf(stderr
      , "./ish: Invalid use of background\n");
               return 0;
            }
            pp = 1;
            ar = 0;
            ri = 0;
            ro = 0;
         }
         else if (check(argument[i])==2)
         {
            if(ar==0)
            {
               free(argument);
               argument = NULL;
               fprintf(stderr, "./ish: Missing command name\n");
               return 0;
            }
            if(ri==1)
            {
               free(argument);
               argument = NULL;
               fprintf(stderr
      , "./ish: Standard input redirection without file name\n");
               return 0;
            }
            if(ri==2)
            {
               free(argument);
               argument = NULL;
               fprintf(stderr
      , "./ish: Multiple redirection of standard input\n");
               return 0;
            }
            if(pp==2)
            {
               free(argument);
               argument = NULL;
               fprintf(stderr
      , "./ish: Multiple redirection of standard input\n");
               return 0;
            }
            if(ro==1)
            {
               free(argument);
               argument = NULL;
               fprintf(stderr
      , "./ish: Standard output redirection without file name\n");
               return 0;
            }
            if(bg)
            {
               free(argument);
               argument = NULL;
               fprintf(stderr
      , "./ish: Invalid use of background\n");
               return 0;
            }
            ri = 1;
         }
         else if(check(argument[i])==3)
         {
            if(ar==0)
            {
               free(argument);
               argument = NULL;
               fprintf(stderr, "./ish: Missing command name\n");
               return 0;
            }
            if(ro==1)
            {
               free(argument);
               argument = NULL;
               fprintf(stderr
      , "./ish: Standard output redirection without file name\n");
               return 0;
            }
            if(ro==2)
            {
               free(argument);
               argument = NULL;
               fprintf(stderr
      , "./ish: Multiple redirection of standard out\n");
               return 0;
            }
            if(ri==1)
            {
               free(argument);
               argument = NULL;
               fprintf(stderr
      , "./ish: Standard input redirection without file name\n");
               return 0;
            }
            if(bg)
            {
               free(argument);
               argument = NULL;
               fprintf(stderr
      , "./ish: Invalid use of background\n");
               return 0;
            }
            ro = 1;
         }
         else
         {
            if(ar==0)
            {
               free(argument);
               argument = NULL;
               fprintf(stderr, "./ish: Missing command name\n");
               return 0;
            }
            bg = 1;
         }
      }
      else
      {
         ar = 1;
         if(ri==1)
            ri = 2;
         if(ro==1)
            ro = 2;
         if(pp==1)
            pp = 2;
         if(bg)
         {
            free(argument);
            argument = NULL;
            fprintf(stderr, "./ish: Invalid use of background\n");
            return 0;
         }
      }
   }
   if(pp==1)
   {
      free(argument);
      argument = NULL;
      fprintf(stderr, "./ish: Missing command name\n");
      return 0;
   }
   else if(ri==1)
   {
      free(argument);
      argument = NULL;
      fprintf(stderr
   , "./ish: Standard input redirection without file name\n");
      return 0; 
   }
   else if(ro==1)
   {
      free(argument);
      argument = NULL;
      fprintf(stderr
   , "./ish: Standard output redirection without file name\n");
      return 0;
   }
   else if(bg)
   {
      free(argument);
      argument = NULL;
      return 1;
   }
   else
   {
      free(argument);
      argument = NULL;
      return 1;
   }
}

/*--------------------------------------------------------------------*/
/* check pipe is exist in command */
/* return pipe number */
int pipenum(DynArray_T oTokens, int Len)
{
   int pn = 0;
   int i;
   const void* c;
   for (i=0;i<Len;i++)
   {
      c = DynArray_get(oTokens, i);
      if (c==NULL)
         break;
      struct Token *psToken = (struct Token*)c;
      /* if special */
      if(check(psToken->pcValue) && psToken->eType)
      {
         if (check(psToken->pcValue)==1)
         {
            pn++;
         }
      }
   }
   return pn;
}

/*--------------------------------------------------------------------*/
/* make command by pipe */
/* return one line of command */
int Commandline(int start, int Len, DynArray_T oTokens, 
         char ** argument, char **arg, char **input, char **output)
{
   int i;
   const void* c;
   int num = 0;
   for (i=start;i<Len;i++)
   {
      c = DynArray_get(oTokens, i);
      if (c==NULL)
         break;
      struct Token *psToken = (struct Token*)c;
      argument[i] = psToken->pcValue;
      if (argument[0])
      {
         if (strcmp(argument[0], "exit")==0)
         {
            printf("\n");
            free(arg);
            arg = NULL;
            free(argument);
            argument = NULL;
            free(input);
            input = NULL;
            free(output);
            output = NULL;
            DynArray_map(oTokens, freeToken, NULL);
            DynArray_free(oTokens);
            exit(0);
         }
      }
      /* if special */
      if(check(argument[i]) && psToken->eType)
      {
         if (check(argument[i])==1)
         {
            break;
         }
         if (check(argument[i])==2)
         {
            i++;
            c = DynArray_get(oTokens, i);
            if (c==NULL)
               break;
            struct Token *psToken = (struct Token*)c;
            argument[i] = psToken->pcValue;
            input[0] = argument[i];
         }
         if(check(argument[i])==3)
         {
            i++;
            c = DynArray_get(oTokens, i);
            if (c==NULL)
               break;
            struct Token *psToken = (struct Token*)c;
            argument[i] = psToken->pcValue;
            output[0]=argument[i];
         }
      }
      else
      {
         arg[num]=argument[i];
         num++;
      }
   }
   return i;
}
/*--------------------------------------------------------------------*/
/* clear pipe */
void PipeClear(int (*fd)[2], int pn)
{
   for(int i=0;i<pn;i++)
   {
      close(fd[i][0]);
      close(fd[i][1]);
   }
}

/*--------------------------------------------------------------------*/

int main(void)
/* Read a line from stdin, and write to stdout each number and word
   that it contains.  Repeat until EOF.  Return 0 iff successful. */
{
   sigset_t set;
   sigemptyset(&set);
   sigaddset(&set, SIGINT);
   sigaddset(&set, SIGQUIT);
   sigaddset(&set, SIGALRM);
   sigprocmask(SIG_UNBLOCK, &set, NULL);
   void (*ter1)(int);
   void (*ter2)(int);
   ter1 = signal(SIGINT, THandler);
   assert(ter1!=SIG_ERR);
   ter2 = signal(SIGQUIT, QHandler);
   assert(ter2!=SIG_ERR);
   char acLine[MAX_LINE_SIZE];
   DynArray_T oTokens;
   int iSuccessful;
   /* input from ./ishrc file */
   char source[MAX_LINE_SIZE];
   strcpy(source, getenv("HOME"));
   char *name = "/.ishrc";
   strcat(source, name);
   fflush(NULL);
   FILE *pFile = fopen(source, "r");
   if(pFile)
   {
   fflush(NULL);
   while (fgets(acLine, MAX_LINE_SIZE, pFile))
   {
      fflush(stdout);
      printf("%% %s", acLine);
      if(acLine[strlen(acLine)-1]!='\n')
      {
         printf("\n"); 
      }
      fflush(stdout);
      int Len = 0;
      int S = 0;
      oTokens = DynArray_new(0);
      if (oTokens == NULL)
      {
         fprintf(stderr, "Cannot allocate memory\n");
         exit(EXIT_FAILURE);
      }
      /* set argument */
      iSuccessful = lexLine(acLine, oTokens);
      Len = DynArray_getLength(oTokens);
      S = Syntactic(Len, oTokens);
      if (iSuccessful && S)
      {
         int N = 0;
         /* if some command exists */
         if (Len > 0)
         {
            int pn = pipenum(oTokens, Len);
            if(pn)
            {
               /* pipe */
               int fd[pn][2];

               for (int i = 0;i<pn;i++)
               {
                  pipe(fd[i]);
               }
               for(int i=0;i<=pn;i++)
               {
                  char **arg = makeargument();
                  char **input = makeargument();
                  char **output = makeargument();
                  char **argument = makeargument();
   N = Commandline(N, Len, oTokens, argument, arg, input, output);
                  if(i==0)
                  {
                  /* start of pipe */
                  /* redirection in -> ok / redirection out -> x */
                     if(input[0])
                     {
                        fflush(NULL);
                        if(fork()==0)
                        {
                           int fdin = open(input[0], O_RDONLY);
                           dup2(fd[i][1], 1);
                           dup2(fdin, 0);
                           close(fdin);
                           PipeClear(fd, pn);
                           fflush(NULL);
                           execvp(arg[0], arg);
   fprintf(stderr, "%s: No such file or directory\n", 
                                   argument[0]);
                           _exit(EXIT_FAILURE);
                        }
                     }                     
                     else
                     {
                        fflush(NULL);
                        if(fork()==0)
                        {
                           dup2(fd[i][1], 1);
                           PipeClear(fd, pn);
                           fflush(NULL);
                           execvp(arg[0], arg);
   fprintf(stderr, "%s: No such file or directory\n", 
                                   arg[0]);
                           _exit(EXIT_FAILURE);
                        }
                     }
                  }
                  else if(i==pn)
                  {
                  /* end of pipe */
                  /* redirection in -> x / redirection out -> ok */
                     if(output[0])
                     {
                        fflush(NULL);
                        if(fork()==0)
                        {
      int fdout = open(output[0],O_CREAT|O_TRUNC|O_WRONLY,0600);
                           dup2(fd[i-1][0], 0);
                           dup2(fdout, 1);
                           close(fdout);
                           PipeClear(fd, pn);
                           fflush(NULL);
                           execvp(arg[0], arg);
               fprintf(stderr, "%s: No such file or directory\n", 
                                    arg[0]);
                           _exit(EXIT_FAILURE);
                        }
                     }
                     else
                     {
                        fflush(NULL);
                        if(fork()==0)
                        {
                           dup2(fd[i-1][0], 0);
                           PipeClear(fd, pn);
                           fflush(NULL);
                           execvp(arg[0], arg);
               fprintf(stderr, "%s: No such file or directory\n", 
                                    arg[0]);
                           _exit(EXIT_FAILURE);
                        }
                     }
                  }
                  else
                  {
                  /* middle of pipe */
                  /* redirection in -> x / redirection out -> x */
                     fflush(NULL);
                     if(fork()==0)
                     {
                        dup2(fd[i-1][0], 0);
                        dup2(fd[i][1], 1);
                        PipeClear(fd, pn);
                        fflush(NULL);
                        execvp(arg[0], arg);
               fprintf(stderr, "%s: No such file or directory\n", 
                                 argument[0]);
                        _exit(EXIT_FAILURE);
                     }
                  }
                  free(arg);
                  free(input);
                  free(argument);
                  free(output);
                  arg = NULL;
                  input = NULL;
                  argument = NULL;
                  output = NULL;
                  if(N<Len)
                  {
                     N++;
                  }
               }
               PipeClear(fd, pn);
               for(int i=0;i<=pn;i++)
               {
                  wait(NULL);
               }
            }
            else
            /* no pipe */
            {
               char **arg = makeargument();
               char **input = makeargument();
               char **output = makeargument();
               char **argument = makeargument();
      Commandline(N, Len, oTokens, argument, arg, input, output);
               execute(arg, input[0], output[0]);
               free(arg);
               free(input);
               free(argument);
               free(output);
               arg = NULL;
               input = NULL;
               argument = NULL;
               output = NULL;
            }           
         }
      }
      /* free malloc */
      DynArray_map(oTokens, freeToken, NULL);
      DynArray_free(oTokens);
      fflush(stdout);
      fflush(NULL);
      fflush(stdin);
   }
   /* close file */
   fclose(pFile);
   }
   /* input from stdin */
   printf("%% ");
   fflush(NULL);
   while (fgets(acLine, MAX_LINE_SIZE, stdin))
   {
      /* setting */
      fflush(NULL);
      fflush(stdin);
      int Len = 0;
      int S = 0;
      oTokens = DynArray_new(0);
      if (oTokens == NULL)
      {
         fprintf(stderr, "Cannot allocate memory\n");
         exit(EXIT_FAILURE);
      }
      /* set argument */
      iSuccessful = lexLine(acLine, oTokens);
      Len = DynArray_getLength(oTokens);
      S = Syntactic(Len, oTokens);
      if (iSuccessful && S)
      {
         int N = 0;
         /* if some command exists */
         if (Len > 0)
         {
            int pn = pipenum(oTokens, Len);
            if(pn)
            {
               /* pipe */
               int fd[pn][2];

               for (int i = 0;i<pn;i++)
               {
                  pipe(fd[i]);
               }
               for(int i=0;i<=pn;i++)
               {
                  char **arg = makeargument();
                  char **input = makeargument();
                  char **output = makeargument();
                  char **argument = makeargument();
      N = Commandline(N, Len, oTokens, argument, arg, input, output);
                  if(i==0)
                  {
                  /* start of pipe */
                  /* redirection in -> ok / redirection out -> x */
                     if(input[0])
                     {
                        fflush(NULL);
                        if(fork()==0)
                        {
                           int fdin = open(input[0], O_RDONLY);
                           dup2(fd[i][1], 1);
                           dup2(fdin, 0);
                           close(fdin);
                           PipeClear(fd, pn);
                           fflush(NULL);
                           execvp(arg[0], arg);
               fprintf(stderr, "%s: No such file or directory\n", 
                                   argument[0]);
                           _exit(EXIT_FAILURE);
                        }
                     }                     
                     else
                     {
                        fflush(NULL);
                        if(fork()==0)
                        {
                           dup2(fd[i][1], 1);
                           PipeClear(fd, pn);
                           fflush(NULL);
                           execvp(arg[0], arg);
               fprintf(stderr, "%s: No such file or directory\n", 
                                   arg[0]);
                           _exit(EXIT_FAILURE);
                        }
                     }
                  }
                  else if(i==pn)
                  {
                  /* end of pipe */
                  /* redirection in -> x / redirection out -> ok */
                     if(output[0])
                     {
                        fflush(NULL);
                        if(fork()==0)
                        {
         int fdout = open(output[0],O_CREAT|O_TRUNC|O_WRONLY,0600);
                     dup2(fd[i-1][0], 0);
                     dup2(fdout, 1);
                           close(fdout);
                           PipeClear(fd, pn);
                           fflush(NULL);
                           execvp(arg[0], arg);
                  fprintf(stderr, "%s: No such file or directory\n", 
                                    arg[0]);
                           _exit(EXIT_FAILURE);
                        }
                     }
                     else
                     {
                        fflush(NULL);
                        if(fork()==0)
                        {
                           dup2(fd[i-1][0], 0);
                           PipeClear(fd, pn);
                           fflush(NULL);
                           execvp(arg[0], arg);
                  fprintf(stderr, "%s: No such file or directory\n", 
                                    arg[0]);
                           _exit(EXIT_FAILURE);
                        }
                     }
                  }
                  else
                  {
                  /* middle of pipe */
                  /* redirection in -> x / redirection out -> x */
                     fflush(NULL);
                     if(fork()==0)
                     {
                        dup2(fd[i-1][0], 0);
                        dup2(fd[i][1], 1);
                        PipeClear(fd, pn);
                        fflush(NULL);
                        execvp(arg[0], arg);
               fprintf(stderr, "%s: No such file or directory\n", 
                                 argument[0]);
                        _exit(EXIT_FAILURE);
                     }
                  }
                  free(arg);
                  free(input);
                  free(argument);
                  free(output);
                  arg = NULL;
                  input = NULL;
                  argument = NULL;
                  output = NULL;
                  if(N<Len)
                  {
                     N++;
                  }
               }
               PipeClear(fd, pn);
               for(int i=0;i<=pn;i++)
               {
                  wait(NULL);
               }
            }
            else
            /* no pipe */
            {
               char **arg = makeargument();
               char **input = makeargument();
               char **output = makeargument();
               char **argument = makeargument();
      Commandline(N, Len, oTokens, argument, arg, input, output);
               execute(arg, input[0], output[0]);
               free(arg);
               free(input);
               free(argument);
               free(output);
               arg = NULL;
               input = NULL;
               argument = NULL;
               output = NULL;
            }    
         }
      }
      /* free oTokens and each token elements */
      DynArray_map(oTokens, freeToken, NULL);
      DynArray_free(oTokens);
      printf("%% ");
   }
   printf("\n");
   return 0;
}