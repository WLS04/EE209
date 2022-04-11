# include <stdio.h>
# include <ctype.h>
# include <stdlib.h>
int main (void) {
    // string input
    char str[] = "abc/*de\nf/*gh\ni*/jkl*/mno";
    char str2[sizeof(str)];
    int del = 0;
    int newline = 0;
    int character = 0;
    int word = 0;
    int errorline = 1;
    int errorcode = 1;
    int lastzero = 0;
    // number of newline and add number of line
    //printf("%lu\n", sizeof(str));
    for (int i = 0; i<(sizeof(str)-1); i++) {
        str2[i]=str[i];
        printf("%c", str2[i]);
    }
    printf("\n");
    // find empty file
    if (sizeof(str)==1) {
        printf("empty file\n");
        exit(EXIT_SUCCESS);
    }
    for (int i = 0; i<(sizeof(str)-2); i++) {
        int start = 0;
        int end = 0;
        if (str[i]=='/') {
            if (str[i+1]=='*') {
                start = i;
                int NL = 0;
                for (int j = i+2; j<(sizeof(str)-2);j++) {
                    if (str[j]=='\n'){
                        NL++;
                    }
                    if (str[j]=='*') {
                        if (str[j+1]=='/'){
                            end = j+1;
                            printf("start : %d, End : %d\n", start, end);
                            int s1 = start;
                            int e = end;
                            while (s1<e-NL) {
                                str[s1]=0;
                                s1++;
                                del++;
                            }
                            str[e-NL] = ' ';
                            int s2 = e-NL+1;
                            while (s2<=e) {
                                str[s2] = '\n';
                                del++;
                                s2++;
                            }
                            errorcode = 0;
                            break;
                        }
                    }
                }
                if (errorcode == 1){
                    for (int k = 0;k<=i;k++) {
                        if (str2[k] == '\n')
                            errorline++;
                    }
                    printf("Error: line %d: underminated comment\n", errorline);
                    exit(EXIT_FAILURE);
                }
                errorcode = 1;
            }
            if (str[i+1]=='/') {
                start = i;
                for (int j = i+2; j<(sizeof(str)-1);j++){
                    if (str[j]=='\n'){
                        end = j;
                        int s = start;
                        int e = end;
                        while (s<e) {
                            str[s]=0;
                            s++;
                            del++;
                        }
                        str[j]='\n';
                        printf("start : %d, End : %d\n", start, end);
                        break;
                    }
                    else if (j==sizeof(str)-2) {
                        end = sizeof(str)-2;
                        int s = start;
                        int e = end;
                        while (s<=e) {
                            str[s]=0;
                            s++;
                            del++;
                        }
                        printf("start : %d, End : %d\n", start, end);
                        break;
                    }
                }
            }
        }
    }
    printf("\n");
    for (int i = 0; i<(sizeof(str)-1); i++) {
        printf("%c", str[i]);
        if (str[i]=='\n') {
            newline++;
        }
    }
    
    printf("\n");
    char wordstr[sizeof(str)];
    for (int i = 0; i<sizeof(str)-1;i++) {
        if (isspace(str[i])==1 || str[i]==0) {
            wordstr[i] = '0';
        }
        else
            wordstr[i] = '1';
        printf("%c", wordstr[i]);
    }
    for (int i = 0; i<(sizeof(str)-2); i++) {
        if (wordstr[i]!=wordstr[i+1]) {
            word = word+1;
        }
    }
    if (wordstr[sizeof(str)-2]==0) {
        lastzero = 1;
    }
    word = (word-lastzero)/2+1;
    
    for (int p=0;p<sizeof(str)-1;p++){
        if (str[p]!=0)
            character++;
    }
    printf("\n");
    printf("%lu\n", sizeof(str));
    printf("number of deleted character : %d\n", del);
    printf("number of newline : %d\n", newline);
    printf("word : %d\n", word);
    printf("number of character : %d\n", character);
    printf("%d %d %d\n", newline, word, character);
    exit(EXIT_SUCCESS);
    return 0;
}

