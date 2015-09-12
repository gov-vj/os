#include <stdio.h>
#include <ctype.h>
#define IN 0
#define OUT 1

int t_bytes=0,t_line_no=0,t_word_count=0;

void wc(FILE *ofile, FILE *infile, char *inname) {
    int bytes=0;  //no. of characters
    int line_no=0,word_count=0;
    int c;
    int state=OUT;
    while((c=getc(infile))!=EOF){
        bytes++;
        if(c=='\n')
            line_no++;
        if(c==' '||c=='\t'||c=='\n')
            state=OUT;
        else if(state==OUT){
            state=IN;
            word_count++;
        }
    }
    t_word_count+=word_count;
    t_line_no+=line_no;
    t_bytes+=bytes;

    fprintf(ofile,"%6d %6d %6d %s\n",line_no,word_count,bytes,inname);
}

int main (int argc, char *argv[]) {
    int i=1;
    FILE *infile;
    if(argc==1)
        wc(stdout,stdin,"");
    while(--argc){
        if(argv[i][0]=='-' && argv[i][1]=='\0')
            wc(stdout,stdin,"-");
        else{
            infile=fopen(argv[i],"r");
            if(infile==NULL)
                fprintf(stderr,"wc: %s: No such file or directory\n",argv[i]);
            else{
                wc(stdout,infile,argv[i]);
                if(fclose(infile)==EOF)
                    fprintf(stderr,"error closning file %s\n",argv[i]);
            }
        }
        i++;
    }
    fprintf(stdout,"%6d %6d %6d %s\n",t_line_no,t_word_count,t_bytes,"total");
    return 0;
}
