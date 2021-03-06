#include <stdio.h>
 
FILE* open_procfs(){
    char procmaps[30];
    sprintf(procmaps, "/proc/%d/maps", getpid());
    return fopen(procmaps, "r");
}
 
int get_addr_from_procfs(FILE* fp, long* pstartaddr, long* pendaddr){
    char line[85];
    char perms[5];
 
    if( !fgets(line, sizeof(line), fp) )
        return -1;
 
    printf("%s", line);
    sscanf(line, "%x-%x %s", pstartaddr, pendaddr, perms);
    if(strncmp(perms, "---p", 4) == 0){
        return -1;
    }
 
    return 0;
}
 
int mem_search(long startaddr, long endaddr){
    unsigned char target_array[] = {0x20, 0x57, 0x7a, 0x00};
    int forward_offset = 0;
    int backward_offset = 4;
    
    unsigned char* paddr;
    unsigned char* printpoint;
    int i;    
 
    for( paddr = (unsigned char*)startaddr; paddr < (unsigned char*)endaddr; paddr++){
        for (i = 0; i < sizeof(target_array); i++){
            if( *(paddr+i) != target_array[i] )
                break;
        }
 
        if( i == sizeof(target_array) ){
            printpoint = paddr - forward_offset;
            printf("%010p : ", printpoint);
 
            while(printpoint < paddr){
                printf("%02x ", *(printpoint++));
            }
 
            printf("[ ");
            for( i = 0; i < sizeof(target_array); i++){
                printf("%02x ", *(printpoint++));
            }
            printf("] ");
 
            for( i = 0; i < backward_offset; i++){
                printf("%02x ", *(printpoint++));
            }
 
            puts(" ");
        }
    }
}
 
int procfs_search(){
    FILE* fp;
    long startaddr = 0x8048000;
    long endaddr = 0xFFFFFFFF;
 
    if( (fp = open_procfs()) == 0){
        puts("procfs error");
    return -1; 
    }
    
    while( get_addr_from_procfs(fp, &startaddr, &endaddr) != -1 ){
        mem_search(startaddr, endaddr); 
    }
    
    fclose(fp);
}
