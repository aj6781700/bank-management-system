#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ACCOUNTS 100   // total accounts program can hold (includes initial 10)
#define PRELOAD 10
#define MAX_TRANSACTIONS 1000
#define DATE_LEN 26

struct Account {
    int accNo;
    char name[50];
    char phone[15];
    char address[100];
    float balance;
    char type[10];
    int active; // 1 if used
};

struct Transaction {
    int accNo;
    char type[30];
    float amount;
    char date[DATE_LEN];
};

struct Account accounts[MAX_ACCOUNTS];
int accountCount = 0;

struct Transaction transactions[MAX_TRANSACTIONS];
int transactionCount = 0;

/* Utility prototypes */
void preloadFixedAccounts();
int findAccountIndex(int accNo);
void addTransaction(int accNo, const char *type, float amount);
void printMenu();

/* Operations */
void createAccountInteractive();
void depositMoney();
void withdrawMoney();
void transferMoney();
void balanceEnquiry();
void transactionHistory();

/* Helpers */
void readLine(char *buf, int size);

int main() {
    srand((unsigned)time(NULL));
    preloadFixedAccounts();

    int choice = 0;
    do {
        printMenu();
        if(scanf("%d", &choice) != 1){
            while(getchar()!='\n');
            choice = 0;
        }
        while(getchar()!='\n'); // clear newline
        switch(choice) {
            case 1: createAccountInteractive(); break;
            case 2: depositMoney(); break;
            case 3: withdrawMoney(); break;
            case 4: transferMoney(); break;
            case 5: balanceEnquiry(); break;
            case 6: transactionHistory(); break;
            case 7: printf("Exiting...\n"); break;
            default: printf("❌ Invalid choice!\n");
        }
    } while(choice != 7);

    return 0;
}

/* Print menu */
void printMenu() {
    printf("\n--- Bank Management System (In-memory) ---\n");
    printf("1. Create Account\n2. Deposit\n3. Withdraw\n4. Transfer\n5. Balance Enquiry\n6. Transaction History\n7. Exit\n");
    printf("Enter choice: ");
}

/* Safe line reader */
void readLine(char *buf, int size) {
    if(fgets(buf, size, stdin)) {
        size_t n = strlen(buf);
        if(n > 0 && buf[n-1] == '\n') buf[n-1] = '\0';
    } else {
        buf[0] = '\0';
    }
}

/* Preload exactly 10 fixed accounts (no files) */
void preloadFixedAccounts() {
    const char *names[PRELOAD] = {
        "jatin bhaiya",
        "ram bhaiya",
        "vipin bhaiya",
        "adarsh bhaiya",
        "rudra bhaiya",
        "gaurav bhaiya",
        "alok bhaiya",
        "naveen bhaiya",
        "rohan bhaiya",
        "pranjal bhaiya"
    };
    const char *phones[PRELOAD] = {
        "9000000001","9000000002","9000000003","9000000004","9000000005",
        "9000000006","9000000007","9000000008","9000000009","9000000010"
    };
    const char *address = "RIET JAIPUR";
    const char *type = "Saving";
    float init_balance = 1000.00f;

    accountCount = 0;
    for(int i=0; i<PRELOAD && accountCount < MAX_ACCOUNTS; ++i) {
        struct Account a;
        a.accNo = 1001 + i; // fixed numbers 1001..1010
        strncpy(a.name, names[i], sizeof(a.name)-1); a.name[sizeof(a.name)-1]='\0';
        strncpy(a.phone, phones[i], sizeof(a.phone)-1); a.phone[sizeof(a.phone)-1]='\0';
        strncpy(a.address, address, sizeof(a.address)-1); a.address[sizeof(a.address)-1]='\0';
        a.balance = init_balance;
        strncpy(a.type, type, sizeof(a.type)-1); a.type[sizeof(a.type)-1]='\0';
        a.active = 1;
        accounts[accountCount++] = a;

        // Add a creation transaction
        addTransaction(a.accNo, "Account Created", a.balance);
    }

    printf("✅ Ready: %d fixed saving accounts loaded in memory (no files used).\n", accountCount);
}

/* Find account index in array by accNo, return -1 if not found */
int findAccountIndex(int accNo) {
    for(int i=0;i<accountCount;i++){
        if(accounts[i].active && accounts[i].accNo == accNo) return i;
    }
    return -1;
}

/* Add transaction to transactions array (in-memory) */
void addTransaction(int accNo, const char *type, float amount) {
    if(transactionCount >= MAX_TRANSACTIONS) return;
    struct Transaction t;
    t.accNo = accNo;
    strncpy(t.type, type, sizeof(t.type)-1); t.type[sizeof(t.type)-1] = '\0';
    t.amount = amount;

    time_t now = time(NULL);
    char *timestr = ctime(&now);
    if(timestr) {
        strncpy(t.date, timestr, DATE_LEN-1);
        t.date[DATE_LEN-1] = '\0';
        char *p = strchr(t.date, '\n'); if(p) *p = '\0';
    } else t.date[0] = '\0';

    transactions[transactionCount++] = t;
}

/* Create account interactively (still in-memory).
   If entered accNo exists, show which account owner's name and ask again. */
void createAccountInteractive() {
    if(accountCount >= MAX_ACCOUNTS) {
        printf("❌ Max accounts reached.\n");
        return;
    }
    struct Account a;
    int accNo;
    while(1) {
        printf("Enter account number (1000 - 99999): ");
        if(scanf("%d",&accNo)!=1){ while(getchar()!='\n'); accNo = 0; }
        while(getchar()!='\n');
        if(accNo < 1000 || accNo > 99999) {
            printf("❌ Invalid account number! Range 1000 - 99999.\n");
            continue;
        }
        int idx = findAccountIndex(accNo);
        if(idx != -1) {
            printf("❌ Ye account number %s ka hai. Aapko naya account number dalna padega.\n", accounts[idx].name);
            continue;
        }
        break; // valid and not existing
    }
    a.accNo = accNo;

    printf("Enter name: ");
    readLine(a.name, sizeof(a.name));

    int validPhone = 0;
    do {
        printf("Enter phone (10 digits): ");
        readLine(a.phone, sizeof(a.phone));
        if(strlen(a.phone) == 10) validPhone = 1;
        else printf("❌ Invalid phone!\n");
    } while(!validPhone);

    printf("Enter address: ");
    readLine(a.address, sizeof(a.address));

    int validType = 0;
    do {
        printf("Enter account type (Saving/Current): ");
        readLine(a.type, sizeof(a.type));
        if(strcmp(a.type, "Saving")==0 || strcmp(a.type, "Current")==0) validType = 1;
        else printf("❌ Invalid type!\n");
    } while(!validType);

    printf("Enter initial balance: ");
    while(scanf("%f", &a.balance)!=1 || a.balance < 0) {
        while(getchar()!='\n');
        printf("❌ Invalid balance!\n");
    }
    while(getchar()!='\n');

    a.active = 1;
    accounts[accountCount++] = a;
    addTransaction(a.accNo, "Account Created", a.balance);
    printf("✅ Account created in memory! AccNo: %d\n", a.accNo);
}

/* Deposit */
void depositMoney() {
    int accNo;
    printf("Enter account number: ");
    if(scanf("%d",&accNo)!=1){ while(getchar()!='\n'); printf("❌ Invalid input.\n"); return; }
    while(getchar()!='\n');
    int idx = findAccountIndex(accNo);
    if(idx == -1){ printf("❌ Account not found!\n"); return; }
    float amt;
    printf("Enter deposit amount: ");
    while(scanf("%f",&amt)!=1 || amt <= 0){ while(getchar()!='\n'); printf("❌ Invalid amount!\n"); }
    while(getchar()!='\n');
    accounts[idx].balance += amt;
    addTransaction(accNo, "Deposit", amt);
    printf("✅ Deposit successful! New Balance: %.2f\n", accounts[idx].balance);
}

/* Withdraw */
void withdrawMoney() {
    int accNo;
    printf("Enter account number: ");
    if(scanf("%d",&accNo)!=1){ while(getchar()!='\n'); printf("❌ Invalid input.\n"); return; }
    while(getchar()!='\n');
    int idx = findAccountIndex(accNo);
    if(idx == -1){ printf("❌ Account not found!\n"); return; }
    float amt;
    printf("Enter withdrawal amount: ");
    while(scanf("%f",&amt)!=1 || amt <= 0){ while(getchar()!='\n'); printf("❌ Invalid amount!\n"); }
    while(getchar()!='\n');
    if(amt > accounts[idx].balance) { printf("❌ Insufficient balance!\n"); return; }
    accounts[idx].balance -= amt;
    addTransaction(accNo, "Withdraw", amt);
    printf("✅ Withdraw successful! New Balance: %.2f\n", accounts[idx].balance);
}

/* Transfer */
void transferMoney() {
    int src,dst;
    printf("Enter your account number: ");
    if(scanf("%d",&src)!=1){ while(getchar()!='\n'); printf("❌ Invalid input.\n"); return; }
    while(getchar()!='\n');
    printf("Enter recipient account number: ");
    if(scanf("%d",&dst)!=1){ while(getchar()!='\n'); printf("❌ Invalid input.\n"); return; }
    while(getchar()!='\n');

    int iSrc = findAccountIndex(src);
    int iDst = findAccountIndex(dst);
    if(iSrc == -1 || iDst == -1){ printf("❌ Source or destination not found!\n"); return; }

    float amt;
    printf("Enter transfer amount: ");
    while(scanf("%f",&amt)!=1 || amt <= 0){ while(getchar()!='\n'); printf("❌ Invalid amount!\n"); }
    while(getchar()!='\n');

    if(amt > accounts[iSrc].balance){ printf("❌ Insufficient balance!\n"); return; }

    accounts[iSrc].balance -= amt;
    accounts[iDst].balance += amt;
    addTransaction(src, "Transfer Sent", amt);
    addTransaction(dst, "Transfer Received", amt);
    printf("✅ Transfer successful!\n");
}

/* Balance enquiry */
void balanceEnquiry() {
    int accNo;
    printf("Enter account number: ");
    if(scanf("%d",&accNo)!=1){ while(getchar()!='\n'); printf("❌ Invalid input.\n"); return; }
    while(getchar()!='\n');
    int idx = findAccountIndex(accNo);
    if(idx == -1){ printf("❌ Account not found!\n"); return; }
    printf("Account Holder: %s\nAccount No: %d\nBalance: %.2f\n", accounts[idx].name, accounts[idx].accNo, accounts[idx].balance);
}

/* Transaction history (search in-memory) */
void transactionHistory() {
    int accNo;
    printf("Enter account number: ");
    if(scanf("%d",&accNo)!=1){ while(getchar()!='\n'); printf("❌ Invalid input.\n"); return; }
    while(getchar()!='\n');

    int found = 0;
    printf("Transaction History for Account %d:\n", accNo);
    for(int i=0;i<transactionCount;i++){
        if(transactions[i].accNo == accNo) {
            found = 1;
            printf("%s: %.2f on %s\n", transactions[i].type, transactions[i].amount, transactions[i].date);
        }
    }
    if(!found) printf("❌ No transactions found!\n");
}
 