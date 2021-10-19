#include <stdio.h>

void display(FILE *);

void updateRecord(FILE *);

void newRecord(FILE *);

void deleteRecord(FILE *);

void displayCopy(FILE *);

void updateRecordCopy(FILE *);

int newRecordCopy(FILE *);

void deleteFile(FILE *);

void deleteStudentIndex(FILE *fPtr,int ogrNo, int line);

struct kayit_index getStudentIndexFile(FILE *fPtr,int ogrNo);
void newRecordIndexFile(FILE *fPtr,int ogrNo, int line);

void deleteStudentBinary(FILE *indexFPtr,FILE *binaryFPtr,int ogrNo);

int enterChoice(void);

struct clientData {
    int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

struct kayit {
    int ogrNo;
    int dersKodu;
    double puan;
    char ogrName[15];
    char dersName[10];
};

struct kayit_index {
    int ogrNoAnahtar;
    int lineArr[20];
};

void main() {
    FILE *cfPtr;
    FILE *binaryfPtr;
    FILE *indexfPtr;
    int choice;
    if(1==0) {
        if ((cfPtr = fopen("../credit.dat", "r+")) == NULL)
            printf("File could not be opened.\n");
        else {
            while ((choice = enterChoice()) != 5) {
                switch (choice) {
                    case 1:
                        display(cfPtr);
                        break;
                    case 2:
                        updateRecord(cfPtr);
                        break;
                    case 3:
                        newRecord(cfPtr);
                        break;
                    case 4:
                        deleteRecord(cfPtr);
                        break;
                }
            }
            fclose(cfPtr);
        }
    }
    else{
        if ((binaryfPtr = fopen("../binary_data.bin", "r+b")) == NULL && (indexfPtr = fopen("../text_data.txt", "r+")) == NULL)
            printf("Binary dosya veya text dosyası acılmadı...\n");
        else {
            while ((choice = enterChoice()) != 5) {
                switch (choice) {
                    case 1:
                        displayCopy(binaryfPtr);
                        break;
                    case 2:
                        updateRecordCopy(binaryfPtr);
                        break;
                    case 3:
                        int tmpOgrNo = newRecordCopy(binaryfPtr);
                        int line = ftell(binaryfPtr)/sizeof (struct  kayit);
                        newRecordIndexFile(indexfPtr, tmpOgrNo,line);
                        break;
                    case 4:
                        int ogrNo;
                        printf("Silinecek OgrNo Girin : ");
                        scanf("%d", &ogrNo);
                        deleteStudentBinary(indexfPtr,binaryfPtr,ogrNo);
                        break;
                    case 6:
                        deleteFile(binaryfPtr);
                        break;
                }
            }
            fclose(binaryfPtr);
        }
    }
}

void display(FILE *readPtr) {
    struct clientData client = {0, "", "", 0.0};
    rewind(readPtr);
    printf("%-6s%-16s%-11s%10s\n",
           "Hes#", "Soyad", "Ad", "Alacak");
    while (!feof(readPtr)) {
        fread(&client, sizeof(struct clientData), 1, readPtr);
        if (client.acctNum != 0 && !feof(readPtr))
            printf("%-6d%-16s%-11s%10.2f\n",
                   client.acctNum, client.lastName,
                   client.firstName, client.balance);
    }
}

void displayCopy(FILE *readPtr) {
    struct kayit client = {0,0,0.0, "", ""};
    rewind(readPtr);
    printf("%-9s%-10s%-10s%-16s%16s\n",
           "ogrNo", "dersKodu", "Puan", "ogrName","dersName");
    while (!feof(readPtr)) {
        fread(&client, sizeof(struct kayit), 1, readPtr);
        if (!feof(readPtr))
            printf("%-9d%-10d%-10.2f%-16s%16s\n",
                   client.ogrNo, client.dersKodu,
                   client.puan, client.ogrName,client.dersName);
    }
}

void indexDosyasiOlustur(FILE *indexfPtr,FILE *binaryfPtr) {
    struct kayit client = {0,0,0.0, "", ""};
    rewind(binaryfPtr);
    int count = 1;
    while (!feof(binaryfPtr)) {
        fread(&client, sizeof(struct kayit), 1, binaryfPtr);
        newRecordIndexFile(indexfPtr,client.ogrNo,count);
        count++;
    }

    printf("%d tane kayıt icin index olusturuldu\n",count);
}

void updateRecord(FILE *fPtr) {
    int account;
    double transaction;
    struct clientData client = {0, "", "", 0.0};
    printf("Enter account to update ( 1 - 100 ): ");
    scanf("%d", &account);
    fseek(fPtr, (account - 1) * sizeof(struct clientData),
          SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    if (client.acctNum == 0)
        printf("Acount #%d has no information.\n", account);
    else {
        printf("%-6d%-16s%-11s%10.2f\n\n", client.acctNum,
               client.lastName, client.firstName, client.balance);
        printf("Enter charge ( + ) or payment ( - ): ");
        scanf("%lf", &transaction);
        client.balance += transaction;
        printf("%-6d%-16s%-11s%10.2f\n",
               client.acctNum, client.lastName,
               client.firstName, client.balance);
        fseek(fPtr, (account - 1) * sizeof(struct clientData),
              SEEK_SET);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    }
}

void updateRecordCopy(FILE *fPtr) {
    int ogrNo;
    int count=1;
    struct kayit client = {-1,-1,0.0, "", ""};
    printf("Öğrenci Numarası Girin : ");
    scanf("%d", &ogrNo);
    rewind(fPtr);
    while (!feof(fPtr)) {

        fread(&client, sizeof(struct kayit), 1, fPtr);

        if (!feof(fPtr) && client.ogrNo==ogrNo){
            printf("Enter ogrNo, dersKodu, puan, ogrName, dersName\n? ");
            scanf("%d%d%lf%s%s", &client.ogrNo, &client.dersKodu,&client.puan,
                  &client.ogrName,&client.dersName);
            fseek(fPtr, (count - 1) * sizeof(struct kayit),
                  SEEK_SET);
            fwrite(&client, sizeof(struct kayit), 1, fPtr);
            break;
        }
        count++;

    }
}

void deleteRecord(FILE *fPtr) {
    struct clientData client, blankClient = {0, "", "", 0.0};
    int accountNum;
    printf("Enter account number to delete ( 1 - 100 ): ");
    scanf("%d", &accountNum);
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0)
        printf("Account %d does not exist.\n", accountNum);
    else {
        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData),
              SEEK_SET);
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    }
}

void deleteStudentBinary(FILE *indexFPtr,FILE *binaryFPtr,int ogrNo) {
    int count=1;
    struct kayit client,blankClient = {-1,-1,0.0, "", ""};
    rewind(binaryFPtr);
    while (!feof(binaryFPtr)) {

        fread(&client, sizeof(struct kayit), 1, binaryFPtr);

        if (!feof(binaryFPtr) && client.ogrNo==ogrNo){
            printf("Enter ogrNo, dersKodu, puan, ogrName, dersName\n? ");
            scanf("%d%d%lf%s%s", &client.ogrNo, &client.dersKodu,&client.puan,
                  &client.ogrName,&client.dersName);
            fseek(binaryFPtr, (count - 1) * sizeof(struct kayit),
                  SEEK_SET);
            fwrite(&blankClient, sizeof(struct kayit), 1, binaryFPtr);
            deleteStudentIndex(indexFPtr,ogrNo,count);
            break;
        }
        count++;

    }
}

void deleteFile(FILE *fPtr) {
    fclose(fPtr);
    if (remove("../binary_data.bin")==0)
        printf("Dosya başarılı bir şekilde silindi!");
    else
        perror("Dosya silme hatası");
}



void newRecord(FILE *fPtr) {
    struct clientData client = {0, "", "", 0.0};
    int accountNum;
    printf("Enter new account number ( 1 - 100 ): ");
    scanf("%d", &accountNum);
    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData),
          SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);
    if (client.acctNum != 0)
        printf("Account #%d already contains information.\n",
               client.acctNum);
    else {
        printf("Enter lastname, firstname, balance\n? ");
        scanf("%s%s%lf", &client.lastName, &client.firstName,
              &client.balance);
        client.acctNum = accountNum;
        fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData),
              SEEK_SET);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    }
}

void newRecordIndexFile(FILE *fPtr,int ogrNo, int line) {
    struct kayit_index kayitIndex = getStudentIndexFile(fPtr,ogrNo);

    fseek(fPtr, (ogrNo - 1) * sizeof(struct kayit_index),
          SEEK_SET);
    if(kayitIndex.ogrNoAnahtar==-1) {
        kayitIndex.ogrNoAnahtar = ogrNo;
        kayitIndex.lineArr[0] = line;
        fwrite(&kayitIndex, sizeof(struct kayit_index), 1, fPtr);
    }
    else{
        for (int i=0;i<20;i++){
            if(kayitIndex.lineArr[i]==-1) {
                kayitIndex.lineArr[i] = line;
                break;
            }
        }
        fwrite(&kayitIndex, sizeof(struct kayit_index), 1, fPtr);
    }
}

void deleteStudentIndex(FILE *fPtr,int ogrNo, int line) {
    struct kayit_index kayitIndex = getStudentIndexFile(fPtr,ogrNo);

    fseek(fPtr, (ogrNo - 1) * sizeof(struct kayit_index),
          SEEK_SET);
    if(kayitIndex.ogrNoAnahtar!=-1) {
        for (int i=0;i<20;i++){
            if(kayitIndex.lineArr[i]==line) {
                kayitIndex.lineArr[i] = -1;
                if(i==0){
                    kayitIndex.ogrNoAnahtar = -1;
                }
                break;
            }
        }
        fwrite(&kayitIndex, sizeof(struct kayit_index), 1, fPtr);
    } else{
        printf("Zaten index yok!!!");
    }
}

struct kayit_index getStudentIndexFile(FILE *fPtr,int ogrNo) {

    struct kayit_index kayitIndex;

    kayitIndex.ogrNoAnahtar = -1;
    for (int i=0;i<20;i++){
        kayitIndex.lineArr[i] = -1;
    }

    fseek(fPtr, (ogrNo - 1) * sizeof(struct kayit_index),
          SEEK_SET);
    fread(&kayitIndex, sizeof(struct kayit_index), 1, fPtr);

    return kayitIndex;
}

int newRecordCopy(FILE *fPtr) {
    struct kayit client = {0, 0,0.0,"", ""};
    fseek(fPtr, 0,
          SEEK_END);
    printf("Enter ogrNo, dersKodu, puan, ogrName, dersName\n? ");
    scanf("%d%d%lf%s%s", &client.ogrNo, &client.dersKodu,&client.puan,
          &client.ogrName,&client.dersName);
    fwrite(&client, sizeof(struct kayit), 1, fPtr);
    return client.ogrNo;
}



int enterChoice(void) {
    int menuChoice;
    printf("\nSecenekler\n"
           "1 - Hesap Goruntuleme\n"
           "2 - Hesap Guncelleme\n"
           "3 - Yeni Hesap\n"
           "4 - Hesap Sil\n"
           "6 - Dosyayı sil\n"
           "5 - Cikis\n? ");
    scanf("%d", &menuChoice);
    return menuChoice;
}

struct kayit kayitBul(FILE *indexFPtr,FILE *binaryFPtr, int ogrNo){

    struct kayit_index kayitIndex;
    kayitIndex.ogrNoAnahtar = -1;
    for (int i=0;i<20;i++){
        kayitIndex.lineArr[i] = -1;
    }
    int lineCount = ftell(indexFPtr) / sizeof (struct kayit_index);

    while (!feof(indexFPtr)){

        fseek(indexFPtr, (lineCount - 1) * sizeof(struct kayit_index),
              SEEK_SET);
        fread(&kayitIndex, sizeof(struct kayit_index), 1, indexFPtr);

        if (!feof(indexFPtr) && kayitIndex.ogrNoAnahtar==ogrNo){
            break;
        }
    }

    int count = ftell(binaryFPtr)/sizeof(struct  kayit);

    fseek(indexFPtr, (lineCount - 1) * sizeof(struct kayit),
          SEEK_SET);

    struct kayit client;
    fread(&client, sizeof(struct kayit), 1, binaryFPtr);

    return client;
}


int binarySearch(int array[]){
    int low       = 0;
    int high   	  = 10;
    int flag      = 0;
    int s         = 4;

    while(low <= high){
        int index = low+(high-low)/2;
        if(array[index] == s){
            flag = 1;
            printf("Founded: %d \n",index);
            return index;
            break;
        }
        else if(array[index] < s){
            low   = index+1;
        }
        else{
            high = index-1;
        }
    }
    if(flag == 0){
        printf("Not Found!\n");
        return -1;
    }
}
