#include <stdio.h>
#include <assert.h>
#include <string.h>

int dir_to_int(char a){
    if(a=='N'){
        return 0;
    }
    else if(a=='W'){
        return 1;
    }
    else if(a=='S'){
        return 2;
    }
    else{
        return 3;
    }
}

int main(int argc,char **argv){
    FILE* file = fopen(argv[1], "r");
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int i=0;
    int wasdeadlocked=0;
    int trains[4]={0,0,0,0};
    int line_no = 1;
    while ((read = getline(&line, &len, file)) != -1) {
         char str[100];
         sprintf(str, "%d", line_no);
         char ans[100] = "Failed at line number: ";
         strcat(ans, str);
         printf("%s\n", ans);
        if(wasdeadlocked){
            wasdeadlocked=0;
            assert(line[6]=='E');
            i=dir_to_int(line[31]);
            trains[i]=0;
        }
        else{
            if(line[0]=='D'){
                assert(trains[0]==1);
                assert(trains[1]==1);
                assert(trains[2]==1);
                assert(trains[3]==1);
                wasdeadlocked=1;
            }
            else if(line[6]=='A'){
                i=dir_to_int(line[35]);
                assert(trains[i]==0);
                trains[i]=1;
            }
            else if(line[6]=='E'){
                i=dir_to_int(line[31]);
                assert(trains[(i+1)%4]==0);
                trains[i]=0;
            }
        }
        line_no++;
    }

    for(i=0;i<4;i++){
        assert(trains[i]==0);
    }
    printf("All good\n");
    return 0;
}