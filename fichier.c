int f(int i, int j){
    return i+j;
}
int g(int i, int j){
    return f(i,j)*i;
}

int main(){
    printf("UXU %d", funct(4, 5));
    int i=0;
    printf("%dtest", i);
    while(i != 4){
        i = i+1;
        if(i==3){
            printf("%dtest", i);
        }
    }
    if(i==3){
        printf("%dtest", i);
    } else {
        i=i+1;
    }
}
int funct(int i, int j){
    return i+j;
}
/*erika miam*/
