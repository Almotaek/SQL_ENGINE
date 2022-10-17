#include "main.h"

void deallocateBlks(void* indextBlk);

int BLOCKSIZE = 256;

//Global Blocks
void* indextBlk = NULL;
void* tables_ptrs = NULL;
void* db_primary = NULL;
void* tables_names = NULL;
void* att_names = NULL;
void* att_types = NULL;
void* prim_keys = NULL;
// void* att_names_lengths = NULL;

int main(){
    indextBlk = (void*) createIndexBlock(); 
    db_primary = (void*) createBlock(); 
    tables_names = (void*) createBlock(); 
    att_names = (void*) createBlock(); 
    att_types = (void*) createBlock(); 
    prim_keys = (void*) createBlock(); 
    tables_ptrs = (void*) createBlock(); 
    //the first block in tables_ptrs is an int of the number of tables in db_primary
    addInt(tables_ptrs,0);


    
    
    create_table("Student","Name",4,"Name","char(4)","ID","int");
    create_table("profs","Name",4,"Name","char(4)","ID","int");

    insert("profs",2,"Khalid",2001);
    insert("profs",2,"Ali",2004);

    insert("Student",2,"HALAALALALALAL",24728);
    

    selectt("profs",2,"Name","ID=23333");
    // selectt("Student",0);
    

//testing
    // for(int i =0; i<247; i++){
    //     addChar(att_names, 'o');
    // }
    // addChar(att_names, 'n');
    // addChar(att_names, 'o');
    // addChar(att_names, 'i');
    // void* right = getBlockChar(att_names, 249);
    // printf("%c",*(char*)right);

    // for(int i =0; i<20; i++){
    //     insert("profs",2,"Khalid",2001);
    // }

    // addInt(tables_ptrs, 99);
    // addInt(tables_ptrs, 25);
    // addInt(tables_ptrs, 88);
    // // printf("%d",*(int*)(tables_ptrs+(60*4)));
    // int* n = (int*)next((tables_ptrs+(60*4)));
    // printf("%d",*(int*)n);
    // addInt(tables_ptrs, 28);
    // addInt(tables_ptrs, 88);

    // void* location = getBlock(tables_ptrs,63*4);
    // printf("%d",*(int*)location);

    // att_names_lengths = (void*) createBlock(); 
    // for(int i =0; i<20; i++){
    //     create_table("Student","Name",4,"Name","char(4)","ID","int");
    // }

    return 0;
}



void selectt(const char *table_name, int length, ...){
    printf("selecting from %s\n",table_name);

    va_list arg_list;
    va_start(arg_list, length);
    char* selectedAtts = NULL;
    char* where = NULL;

    if (length==2){
        selectedAtts = va_arg(arg_list, char*);
        where = va_arg(arg_list, char*);
        // printf("%s",where);
    }else if(length==1){
        selectedAtts = va_arg(arg_list, char*);
        // printf("%s",selectedAtts);
    }



    //geting the table location:
    void* tableLocation = NULL;

    tableLocation = locateTable(table_name);

    if (tableLocation==NULL){
        printf("Table not found");
        exit(0);
    }

    //known distance value 
    int attNamesDistance = 4;
    int attTypeDistance = 8;
    int rootDistance = 16;
    int numBytesDistance = 20;
    int numAttsDistance = 24;

    void* root = *(void**)getBlock(tableLocation,rootDistance);
    void* attTypes = *(void**)getBlock(tableLocation,attTypeDistance);
    int numAtts = *(int*)getBlock(tableLocation,numAttsDistance);
    char* attNamesBlock = *(char**)getBlock(tableLocation,attNamesDistance);

    //putting att names pointers in array of char pointers
    char* arrayAtt[numAtts];
    char* currentAttNameStart = attNamesBlock;
    int sizeAtt = 0;
    char* smth = NULL;
    for(int i =0; i<numAtts; i++){
        // printf("%s\n",currentAttNameStart);
        arrayAtt[i] = currentAttNameStart;
        // printf("%s\n",arrayAtt[i]);
        sizeAtt = 0;
        getStringSize(currentAttNameStart,&sizeAtt);
        sizeAtt = sizeAtt + 1;
        currentAttNameStart = getBlockChar(currentAttNameStart,sizeAtt);
    }

    //putting att types in an array of chars
    char* type = (char*)attTypes;
    void* att = NULL;
    char arrayTypes[numAtts];
    for(int i = 0; i<numAtts; i++){
        char t = *type;
        type = (char*)(nextChar(type));
        arrayTypes[i] = t;
    }

    int* numBytes = (int*)getBlock(tableLocation,numBytesDistance);

    //printing selected atts:
    void* current = root;
    char* string = NULL;
    int size = 0;
    int bytes = 0;
    char t = 0;
    int totalBytes = *numBytes;
    char* attNow = NULL;
    bool valid = false;
    while(totalBytes>0){
        for(int i =0; i<numAtts; i++){
            printf("|");
            t = arrayTypes[i];
            attNow = arrayAtt[i];
            if(strstr(selectedAtts,attNow) || *selectedAtts = '*'){
                valid = true;
            }else{
                valid = false;
            }
            //GO through only if att value = where value
            // printf("%c",type);
            if(t == 'c'){
                size = 0;
                getStringSize(current,&size);
                getString(current,string);
                bytes = size + 1;
                totalBytes = totalBytes - bytes;
                current = getBlockChar(current,bytes);
            }else if(t == 'i'){
                printf("%d",*(int*)current);
                current = getBlockChar(current,4);
                totalBytes = totalBytes - 4;
            }
        }
        printf("\n");
    }

}



void insert(const char *table_name, int length, ...){
    printf("Insterting to %s\n",table_name);

    va_list arg_list;
    va_start(arg_list, length);

    //geting the table location:
    void* tableLocation = NULL;

    tableLocation = locateTable(table_name);

    if (tableLocation==NULL){
        printf("Table not found");
        exit(0);
    }
    //known distance value 
    int attTypeDistance = 8;
    int rootDistance = 16;
    int numBytesDistance = 20;
    int numAttsDistance = 24;

    void* root = *(void**)getBlock(tableLocation,rootDistance);
    void* attTypes = *(void**)getBlock(tableLocation,attTypeDistance);
    int numAtts = *(int*)getBlock(tableLocation,numAttsDistance);
    int* numBytes = (int*)getBlock(tableLocation,numBytesDistance);

    //getting the types to add the elements using the right type (char,int...etc)
    char* type = (char*)attTypes;
    void* att = NULL;
    // int* att = va_arg(arg_list, int*);;
    // printf("%d",att);
    for(int i = 0; i<numAtts; i++){
        char t = *type;
        type = (char*)(nextChar(type));
        // printf("%c",t);
        if(t=='i'){
            att = (int*) va_arg(arg_list, int*);
            int number = (int) att;
            addInt(root,number);
            //updating # of records
            *numBytes = (*numBytes)+4;  
            // printf("%d\n",number);
            // printf("%d\n",att);
        }else if(t=='c'){
            att = (char*) va_arg(arg_list, char*);
            int v = 0;
            while(*(char*)(att+v) != '\0'){
                // printf("%c",*(char*)(att+v));
                addChar(root,*(char*)(att+v));
                //updating # of records
                *numBytes = (*numBytes)+1;
                v++;
            }    
            addChar(root,'\0'); 
            //updating # of records
            *numBytes = (*numBytes)+1;   
        }
    }




}

void* locateTable(const char *table_name){
    void* tableLocation = NULL;
    int numTables = *(int*)tables_ptrs;
    void* tablePtr = tables_ptrs;
    for (int i = 0; i<numTables; i++){
        tablePtr = next(tablePtr);
        // printf("%s",**(char***)(tablePtr));
        char* string = **(char***)(tablePtr);
        if(strcmp(string,table_name)==0){
            tableLocation = *(void**)(tablePtr);
        }
    } 
    return tableLocation;
}

void* getBlockChar(void* start, int ByteSize){
    if (ByteSize==0){
        return (start);
    }else{
        if(*(char*)(start+1)=='@'){
            void* expandedBlockPtr = *(void**)(start+6);
            getBlockChar(expandedBlockPtr,ByteSize-1);
        }else{
            getBlockChar(start+1,ByteSize-1);
        }
    }
}

void* getBlock(void* start, int ByteSize){
    if (ByteSize==0){
        return (start);
    }else{
        if(*(char*)(start+7)=='@'){
            void* expandedBlockPtr = *(void**)(start+12);
            getBlock(expandedBlockPtr,ByteSize-4);
        }else{
            getBlock(start+4,ByteSize-4);
        }
    }
}

void* next(void* start){
    if(*(char*)(start+7)=='@'){
        void* expandedBlockPtr = *(void**)(start+12);
        return expandedBlockPtr;
    }else{
        void* next = start + 4;
        return next;
    }
}

void* nextChar(void* start){
    if(*(char*)(start+1)=='@'){
        void* expandedBlockPtr = *(void**)(start+6);
        return expandedBlockPtr;
    }else{
        void* next = start + 1;
        return next;
    }
}


void create_table(const char *table_name, const char *primary_key, int length, ...){
    printf("Creating Table %s\n",table_name);
    //getting other paramaters
    va_list arg_list;
    va_start(arg_list, length);

    //tables_ptrs
    void* table_ptr = getLast(db_primary,4);
    addVoidPtr(tables_ptrs,table_ptr);
    //increamenting the number of tables in the DB:
    (*(int*)tables_ptrs) = (*(int*)tables_ptrs)+1;
    // printf("table ptr: %d\n",table_ptr);

    //name
    void* name = getLast(tables_names,1);
    addCharPtr(db_primary,name);
    // printf("name ptr: %d\n",name);

    //att_names
    void* attN = getLast(att_names,1);
    addCharPtr(db_primary,attN);
    // printf("nameN ptr: %d\n",attN);

    //att_names
    void* attT = getLast(att_types,1);
    addCharPtr(db_primary,attT);
    // printf("attT ptr: %d\n",attT);

    //prim_keys
    void* primK = getLast(prim_keys,1);
    addCharPtr(db_primary,primK);

    // //att_name_lengths
    // void* attNL = getLast(att_names_lengths,4);
    // addIntPtr(db_primary,attNL);
    // // printf("attNL ptr: %d\n",attNL);

    //root
    void* root = (void*) createBlock();
    addVoidPtr(db_primary,root);
    // printf("%d",root);
    // printf("root ptr: %d\n",root);
    //element_sizes
    // void* element_sizes = (void*) createBlock();
    // addVoidPtr(db_primary,element_sizes);
    // printf("element_sizes ptr: %d\n",element_sizes);

    //# of records
    addInt(db_primary,0);


    //# of attributes
    addInt(db_primary,length/2);
    // printf("attributes %d\n",length/2);


    // //Table Name Length
    // addInt(db_primary,strlen(table_name));
    // // printf("Name Length %d\n",strlen(table_name));

    //adding table name to table_names block
    for(int i =0; i<strlen(table_name);i++){
        char c = *(table_name+i);
        addChar(tables_names,c);
    }
    addChar(tables_names,'\0');

    //addting prim_key value
    for(int i =0; i<strlen(primary_key);i++){
        char c = *(primary_key+i);
        addChar(prim_keys,c);
    }
    addChar(prim_keys,'\0');

    //(att_type,att_name,att_name_lengths)
    //Getting the att names and types from ... and adding to the approprite blocks (att_type,att_name,att_name_lengths)
    for (int i = 1; i<length+1; i=i+1){
        if(i%2!=0){
            // printf("Att Name\n");
            char* att_name = va_arg(arg_list, char*);
            int k = 0;
            while(*(att_name+k)!='\0'){
                // printf("%c",*(att_name+k));
                addChar(att_names,*(att_name+k));
                k = k + 1;
            }
            addChar(att_names,'\0');
            // printf("\n");
        }else{
            // printf("Att Type\n");
            char* att_name = va_arg(arg_list, char*);
            //switch case char(n) -> c int -> i ..etc
            int k = 0;
            addChar(att_types,*(att_name));
            // printf("%c\n",*(att_name));

        }

    }


}
void* createBlock(){
    void* blk = (void*) malloc(BLOCKSIZE);
    void* newBlkptr = NULL;
    *(int*)(blk+BLOCKSIZE-8) = BLOCKSIZE-9;
    *(void**)(blk+BLOCKSIZE-4) = newBlkptr;
    *(char*)(blk+BLOCKSIZE-9) = '@';
    addVoidPtr(indextBlk,blk);
    return blk;
}

void* createIndexBlock(){
    void* blk = (void*) malloc(BLOCKSIZE);
    void* newBlkptr = NULL;
    *(int*)(blk+BLOCKSIZE-8) = BLOCKSIZE-8;
    *(void**)(blk+BLOCKSIZE-4) = newBlkptr;
    return blk;
}

int getStringSize(void* start,int* size){
    int i = 0;
    while(*(char*)(start+i)!='\0' && *(char*)(start+i)!='@'){
        // printf("%c",*(char*)(start+i));
        i = i + 1;
        (*size) = (*size) + 1;
    }
    if(*(char*)(start+i)=='@'){
        if(*(void**)((start+i)) != NULL){
            void* expandedBlockPtr = *(void**)((start+i+5));
            getStringSize(expandedBlockPtr, size);
        }
    } 
}

void* getString(void* start, char* string){
    int i = 0;
    while(*(char*)(start+i)!='\0' && *(char*)(start+i)!='@'){
        printf("%c",*(char*)(start+i));
        // *(char*)(string+i) = *(char*)(start+i);
        i = i + 1;
    }
    if(*(char*)(start+i)=='@'){
        if(*(void**)((start+i)) != NULL){
            void* expandedBlockPtr = *(void**)((start+i+5));
            getString(expandedBlockPtr, string);
        }
    }
}

void* getLast(void* block, int byteSize){
    void* last = NULL;
    int numAvalBytes = *(int*)(block+BLOCKSIZE-8);
    int index = BLOCKSIZE-9-numAvalBytes;
    if (numAvalBytes >= byteSize){
        last = block+BLOCKSIZE-9-numAvalBytes;
        return last;
    }else{
        *(int*)(block+BLOCKSIZE-8) = 0;
        if(*(void**)(block+BLOCKSIZE-4) == NULL){
            // printf("NULL!\n");
            void* indextBlkExpaneded = (void*) createBlock(); 
            *(void**)(block+BLOCKSIZE-4) = indextBlkExpaneded;
            getLast(indextBlkExpaneded, byteSize);
        }else{
            // printf("EXAPNEDED BLK\n");
            void* expandedBlockPtr = *(void**)(block+BLOCKSIZE-4);
            getLast(expandedBlockPtr, byteSize);
        }
    
    }
}

void addChar(void* block, char letter){
    int numAvalBytes = *(int*)(block+BLOCKSIZE-8);
    int index = BLOCKSIZE-9-numAvalBytes;
    if (numAvalBytes >= 1){
        *(char*)(block+index) = letter;
        *(int*)(block+BLOCKSIZE-8) = *(int*)(block+BLOCKSIZE-8) - 1;
    }else{
        *(int*)(block+BLOCKSIZE-8) = 0;
//        printf("blk is full!\n");
        if(*(void**)(block+BLOCKSIZE-4) == NULL){
           printf("NULL!\n");
            void* indextBlkExpaneded = (void*) createBlock(); 
            *(void**)(block+BLOCKSIZE-4) = indextBlkExpaneded;
            addChar(indextBlkExpaneded, letter);
        }else{
         printf("EXAPNEDED BLK\n");
            void* expandedBlockPtr = *(void**)(block+BLOCKSIZE-4);
            addChar(expandedBlockPtr, letter);
        }
    }
    
}

void addInt(void* block, int num){
    int numAvalBytes = *(int*)(block+BLOCKSIZE-8);
    int index = BLOCKSIZE-9-numAvalBytes;
    if (numAvalBytes >= 4){
        *(int*)(block+index) = num;
        *(int*)(block+BLOCKSIZE-8) = *(int*)(block+BLOCKSIZE-8) - 4;
    }else{
        *(int*)(block+BLOCKSIZE-8) = 0;
//        printf("blk is full!\n");
        if(*(void**)(block+BLOCKSIZE-4) == NULL){
        //    printf("NULL!\n");
            void* indextBlkExpaneded = (void*) createBlock(); 
            *(void**)(block+BLOCKSIZE-4) = indextBlkExpaneded;
            addInt(indextBlkExpaneded, num);
        }else{
        //    printf("EXAPNEDED BLK\n");
            void* expandedBlockPtr = *(void**)(block+BLOCKSIZE-4);
            addInt(expandedBlockPtr, num);
        }
    }
    
}

void addIntPtr(void* block, int* blkToBeAdded){
    int numAvalBytes = *(int*)(block+BLOCKSIZE-8);
    int index = BLOCKSIZE-9-numAvalBytes;
    if (numAvalBytes >= 4){
        *(int**)(block+index) = blkToBeAdded;
        *(int*)(block+BLOCKSIZE-8) = *(int*)(block+BLOCKSIZE-8) - 4;
    }else{
        *(int*)(block+BLOCKSIZE-8) = 0;
//        printf("blk is full!\n");
        if(*(void**)(block+BLOCKSIZE-4) == NULL){
//            printf("NULL!\n");
            void* indextBlkExpaneded = (void*) createBlock(); 
            *(void**)(block+BLOCKSIZE-4) = indextBlkExpaneded;
            addIntPtr(indextBlkExpaneded, blkToBeAdded);
        }else{
//            printf("EXAPNEDED BLK\n");
            void* expandedBlockPtr = *(void**)(block+BLOCKSIZE-4);
            addIntPtr(expandedBlockPtr, blkToBeAdded);
        }
    }
    
}

void addCharPtr(void* block, char* blkToBeAdded){
    int numAvalBytes = *(int*)(block+BLOCKSIZE-8);
    int index = BLOCKSIZE-9-numAvalBytes;
    if (numAvalBytes >= 4){
        *(char**)(block+index) = blkToBeAdded;
        *(int*)(block+BLOCKSIZE-8) = *(int*)(block+BLOCKSIZE-8) - 4;
    }else{
        *(int*)(block+BLOCKSIZE-8) = 0;
//        printf("blk is full!\n");
        if(*(void**)(block+BLOCKSIZE-4) == NULL){
//            printf("NULL!\n");
            void* indextBlkExpaneded = (void*) createBlock(); 
            *(void**)(block+BLOCKSIZE-4) = indextBlkExpaneded;
            addCharPtr(indextBlkExpaneded, blkToBeAdded);
        }else{
//            printf("EXAPNEDED BLK\n");
            void* expandedBlockPtr = *(void**)(block+BLOCKSIZE-4);
            addCharPtr(expandedBlockPtr, blkToBeAdded);
        }
    }
    
}
void addVoidPtr(void* block, void* blkToBeAdded){
    int numAvalBytes = *(int*)(block+BLOCKSIZE-8);
    int index = BLOCKSIZE-9-numAvalBytes;
    if (numAvalBytes >= 4){
        *(void**)(block+index) = blkToBeAdded;
        *(int*)(block+BLOCKSIZE-8) = *(int*)(block+BLOCKSIZE-8) - 4;
    }else{
        *(int*)(block+BLOCKSIZE-8) = 0;
//        printf("blk is full!\n");
        if(*(void**)(block+BLOCKSIZE-4) == NULL){
//            printf("NULL!\n");
            void* indextBlkExpaneded = (void*) createBlock(); 
            *(void**)(block+BLOCKSIZE-4) = indextBlkExpaneded;
            addIndexBlk(indextBlkExpaneded, blkToBeAdded);
        }else{
//            printf("EXAPNEDED BLK\n");
            void* expandedBlockPtr = *(void**)(block+BLOCKSIZE-4);
            addIndexBlk(expandedBlockPtr, blkToBeAdded);
        }
    }
    
}

void addIndexBlk(void* block, void* blkToBeAdded){
    int numAvalBytes = *(int*)(block+BLOCKSIZE-8);
    int index = BLOCKSIZE-9-numAvalBytes;
    if (numAvalBytes >= 4){
        *(void**)(block+index) = blkToBeAdded;
        *(int*)(block+BLOCKSIZE-8) = *(int*)(block+BLOCKSIZE-8) - 4;
    }else{
        *(int*)(block+BLOCKSIZE-8) = 0;
//        printf("blk is full!\n");
        if(*(void**)(block+BLOCKSIZE-4) == NULL){
//            printf("NULL!\n");
            void* indextBlkExpaneded = (void*) createIndexBlock(); 
            *(void**)(block+BLOCKSIZE-4) = indextBlkExpaneded;
            addIndexBlk(indextBlkExpaneded, blkToBeAdded);
        }else{
//            printf("EXAPNEDED BLK\n");
            void* expandedBlockPtr = *(void**)(block+BLOCKSIZE-4);
            addIndexBlk(expandedBlockPtr, blkToBeAdded);
        }
    }
    
}
//check using valgrind
void deallocateBlks(void* indextBlk){
    printf("Deallocating Blocks...");
    // int numAvalBytes = *(int*)(indextBlk+BLOCKSIZE-8);
}




//createTableV
//createTableF
//addRecord(record,tableName)
//addBlk(void* blkToBeAdded)

//how to use void pointers to go to blocks where there is data:
/*
    void* someBlk = (void*) createBlock();
    *(char*)(someBlk) = 'y';
    void* startOfNewBlkPtr = *(void**)(indexblk+BLOCKSIZE-4) = someBlk;
    char* castingStation = (char*) startOfNewBlkPtr;
    printf("%c",*castingStation);
*/
/*
    //testing: making end blk point to a new blk
    *(int*)(datablk) = 1;
    *(int**)(indextBlk+BLOCKSIZE-4) = datablk;


    void* datablk = (void*) createBlock();
    *(int*)(datablk) = 1;
    *(int**)(indextBlk+BLOCKSIZE-4) = datablk;

    //getting it:
    void* expandedBlockPtr = *(void**)(indextBlk+BLOCKSIZE-4);
    *(int*)(expandedBlockPtr+4) = 8;
*/

/*
    for(int i =0; i<61; i++){
        addInt(db_primary,i);
    }
    addChar(db_primary,'o');
    addInt(db_primary,77);
    void* last = (void*) getLast(db_primary,4);
    printf("%d",last);
    addChar(db_primary,'o');
    last = (void*) getLast(db_primary,1);
    printf("%d",last);

*/