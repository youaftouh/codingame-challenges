#include<stdio.h>
char s[99],d[9];int t[99],f,p,a,b,n;int main(){while(fgets(s,99,stdin)){n=sscanf(s,"%d%d%s%d%d",&f,&p,d,&a,&b);n>3?t[a]=b:n<3?t[f]=p:puts(f>=0&&(t[f]-p)*(79-*d)>0?"BLOCK":"WAIT");}}