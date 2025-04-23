// Libraries
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>

using namespace std;

// Variables
const string ignoreKeyword = "@SysIgnore";
const string fileName = "bankData.txt";
const string usersFileName = "users.txt";
const string delimiter = "|.|";
const string addAccOpNo = "-1";
const string adminAccountName = "Admin";
const string adminPermissionLevel = "-1";

short const accNoLen = 4;
double const minWithdrawAmount = 10.0;

enum enAction { DIS = 1, ADD = 2, DEL = 3, UPD = 4, FND = 5, TRNS = 6, USR = 7, EXT = 8 };
enum enSecureTransaction {DEP = 1, WIT = 2, CHK = 3, MNU = 4};
enum enAdminAction {showUsers = 1, addUser = 2, deleteUser = 3, updateUser = 4, findUser = 5, mainMenu = 6};
vector <string> menuActions = {
	"Display Clients List",
	"Add Client",
	"Delete Client",
	"Update Client",
	"Find Client",
	"Transactions",
	"Manage Users",
	"Log-Out",

};
vector <string> secureActions = {
	"Deposit",
	"Withdraw",
	"Check All Blanaces",
	"Main Menu",

};
vector <string> adminActions = {
	"Display Users List",
	"Add User",
	"Delete User",
	"Update User",
	"Find User",
	"Main Menu",

};

struct stPermission {
	string name;
	bool access = false;

};

stPermission viewBankDatabase{ "View Bank Database", false };
stPermission addNewClient{ "Add New Client", false };
stPermission updateClient{ "Update Client", false };
stPermission deleteClient{ "Delete Client", false };
stPermission findClient{ "Find Client", false };
stPermission accessTransactions{ "Access Transactions", false };
stPermission manageUsers{ "Manage Users", false };

vector <stPermission> generalPermissions{
		viewBankDatabase,
		addNewClient,
		deleteClient,
		updateClient,
		findClient,
		accessTransactions,
		manageUsers,

};

struct stBankAccount {
	string accountNumber;
	string PINCode;
	string name;
	string phone;

	float balance = 0.00;

	bool deleteMark = false;

};
struct stUserAccount {
	string name;
	string PINCode;
	string permissionsLevel = "0";

	vector <stPermission> permissions = generalPermissions;

	bool deleteMark = false;

};

stUserAccount activeUser;

// Generic Functions
string tabs(short numberOfTabs)
{
	string tabs;

	for (short i = 0; i < numberOfTabs; i++)
	{
		tabs += "\t";

	}

	return tabs;

}
string spaces(short numberOfSpaces)
{
	string spaces;

	for (short i = 0; i < numberOfSpaces; i++)
	{
		spaces += ' ';

	}

	return spaces;

}

bool readConfirmation()
{
	char n;
	cout << "ARE YOU SURE TO PREOCCED WITH THIS ACTION (Y:YES | N:NO)? ";
	cin >> n;

	return (n == 'Y' || n == 'y');

}

short inputNumberInRange(short from, short to, string message = "")
{
	short number = from - 1;

	if (message == "")
	{
		message = "Please insert a number [" + to_string(from) + " ~ " + to_string(to) + "]:";

	}

	while (number < from || number > to)
	{
		cout << message;
		cin >> number;

		if (cin.fail())
		{
			cin.clear();
			cin.ignore(numeric_limits<std::streamsize>::max(), '\n');

		}
		else
		{
			cin.ignore(numeric_limits<std::streamsize>::max(), '\n');

		}
	}

	return number;

}
double inputNumberInRange(double from, double to, string message = "")
{
	double number = from - 1;

	if (message == "")
	{
		message = "Please insert a number [" + to_string(from) + " ~ " + to_string(to) + "]:";

	}

	while (number < from || number > to)
	{
		cout << message;
		cin >> number;

		if (cin.fail())
		{
			cin.clear();
			cin.ignore(numeric_limits<std::streamsize>::max(), '\n');

		}
		else
		{
			cin.ignore(numeric_limits<std::streamsize>::max(), '\n');

		}
	}

	return number;

}

string inputString(short from = 0, short to = numeric_limits<std::streamsize>::max(), string message = "", string warnning = "")
{
	string result;
	bool repeat = true;

	if (message == "")
	{
		message = "Please insert a text: ";

	}
	if (warnning == "")
	{
		warnning = "Invalid String!";

	}

	while (repeat)
	{
		repeat = false;
		cout << message;
		getline(cin >> ws, result);

		if (result.length() < from || result.length() > to)
		{
			repeat = true;
			cout << "==================\n";
			cout << warnning << "\n" << endl;

		}
	}

	return result;

}
vector <string> splitString(string text, string delimiter)
{
	string word;
	vector <string> result;
	size_t pos;

	while ((pos = text.find(delimiter)) != string::npos)
	{
		word = text.substr(0, pos);

		if (text != "")
		{
			result.push_back(word);

		}

		text = text.substr(pos + delimiter.length(), text.length());

	}

	if (text != "")
	{
		result.push_back(text);

	}

	return result;

}
string reverseString(string text)
{
	string result;

	for (char n : text)
	{
		result = n + result;

	}

	return result;

}

vector <short> dissolveBinaryNumber(short binaryNumber)
{
	vector <short> result;
	short logResult;

	while (binaryNumber > 0)
	{
		logResult = log2(binaryNumber);
		result.push_back(logResult);
		binaryNumber -= pow(2, logResult);

	}

	return result;

}

// Cryptography Functions
string encryptText(string text, short encryptionKey = '0')
{
	string encryptionResult;
	string word;

	for (char n : text)
	{
		word += n + encryptionKey;

		if (word.length() > 4)
		{
			encryptionResult += reverseString(word);
			word.clear();

		}
	}

	if (word != "")
	{
		encryptionResult += reverseString(word);

	}

	return encryptionResult;

}
string decryptText(string text, short decryptionKey = '0')
{
	string decryptionResult;
	string word;

	for (char n : text)
	{
		word += n - decryptionKey;

		if (word.length() > 4)
		{
			decryptionResult += reverseString(word);
			word.clear();

		}
	}

	if (word != "")
	{
		decryptionResult += reverseString(word);

	}

	return decryptionResult;

}

// Functions
vector <string> readFile(string fileName)
{
	vector <string> fileData;
	fstream file;

	short count = 0;

	file.open(fileName, ios::in);

	if (file.is_open())
	{
		string line;

		while (getline(file, line))
		{
			fileData.push_back(decryptText(line, pow('0', 2) + ++count));

		}

		file.close();

	}
	else
	{
		cout << "Error opening file." << endl;

	}

	return fileData;

}
void saveFile(string fileName, vector <string>& records)
{
	fstream file;

	short count = 0;

	file.open(fileName, ios::out);

	if (file.is_open())
	{
		for (string& record : records)
		{
			if (record != ignoreKeyword)
			{
				file << encryptText(record, pow('0', 2) + ++count) << endl;

			}
		}

		file.close();

	}
	else
	{
		cout << "Error opening file." << endl;

	}
}

vector <stPermission> getPermissions(short permissionsLevel)
{
	vector <stPermission> permissionsTemplate = generalPermissions;
	vector <short> permissions;
	
	if (to_string(permissionsLevel) == adminPermissionLevel)
	{
		for (stPermission& n : permissionsTemplate)
		{
			n.access = true;

		}

		return permissionsTemplate;

	}

	permissions = dissolveBinaryNumber(permissionsLevel);

	for (short n : permissions)
	{
		permissionsTemplate.at(n).access = true;

	}

	return permissionsTemplate;

}

void saveUsersRecords(vector <string>& usersRecords)
{
	saveFile(usersFileName, usersRecords);

}
vector <string> getUsersRecords()
{
	return readFile(usersFileName);

}
stUserAccount unpackUserRecord(string& userRecord)
{
	stUserAccount userAccount;
	vector <string> record = splitString(userRecord, delimiter);

	userAccount.name = record[0];
	userAccount.PINCode = record[1];
	userAccount.permissionsLevel = record[2];
	userAccount.permissions = getPermissions(stoi(userAccount.permissionsLevel));

	return userAccount;

}
vector <stUserAccount> unpackUsersRecords(vector <string>& usersRecords)
{
	vector <stUserAccount> userAccounts;
	stUserAccount account;

	for (string& record : usersRecords)
	{
		account = unpackUserRecord(record);
		userAccounts.push_back(account);

	}

	return userAccounts;

}
string packUserRecord(stUserAccount& account)
{
	string accountRecord = account.name + delimiter;
	accountRecord += account.PINCode + delimiter;
	accountRecord += account.permissionsLevel;

	return accountRecord;

}
vector <string> packUsersRecords(vector <stUserAccount>& usersAccounts)
{
	vector <string> usersRecords;
	string userRecord;

	for (stUserAccount& account : usersAccounts)
	{
		if (account.deleteMark)
		{
			continue;

		}

		userRecord = packUserRecord(account);
		usersRecords.push_back(userRecord);

	}

	return usersRecords;

}

void saveBankRecords(vector <string>& bankRecords)
{
	saveFile(fileName, bankRecords);

}
vector <string> getBankRecords()
{
	return readFile(fileName);

}
stBankAccount unpackAccountRecord(string& accountRecord)
{
	stBankAccount bankAccount;
	vector <string> record = splitString(accountRecord, delimiter);

	bankAccount.accountNumber = record[0];
	bankAccount.PINCode = record[1];
	bankAccount.name = record[2];
	bankAccount.phone = record[3];
	bankAccount.balance = stof(record[4]);

	return bankAccount;

}
vector <stBankAccount> unpackBankRecords(vector <string>& bankRecords)
{
	vector <stBankAccount> bankAccounts;
	stBankAccount account;

	for (string& record : bankRecords)
	{
		account = unpackAccountRecord(record);
		bankAccounts.push_back(account);

	}

	return bankAccounts;

}
string packAccountRecord(stBankAccount& account)
{
	string accountRecord = account.accountNumber + delimiter;
	accountRecord += account.PINCode + delimiter;
	accountRecord += account.name + delimiter;
	accountRecord += account.phone + delimiter;
	accountRecord += to_string(account.balance);

	return accountRecord;

}
vector <string> packBankRecords(vector <stBankAccount>& bankAccounts)
{
	vector <string> bankRecords;
	string accountRecord;

	for (stBankAccount& account : bankAccounts)
	{
		if (account.deleteMark)
		{
			continue;

		}

		accountRecord = packAccountRecord(account);
		bankRecords.push_back(accountRecord);

	}

	return bankRecords;

}

double calculateTotalBalances(vector <stBankAccount>& bankAccounts)
{
	double total = 0;

	for (stBankAccount& account : bankAccounts)
	{
		total += account.balance;

	}

	return total;

}

void displayOptions(vector <string>& list)
{
	for (short i = 0; i < list.size(); i++)
	{
		cout << spaces(2) << '[' << i + 1 << ']' << spaces(2) << list.at(i) << "\n";

	}
}
void displayOptionsScreen(vector <string> list = menuActions, string title = "Main Menu")
{
	cout << "--------------------------------------\n";
	cout << right << setw(25) << title << '\n';
	cout << "--------------------------------------\n";

	displayOptions(list);

	cout << "--------------------------------------\n";

}

void displayDatabaseHeader(short accountsNumber)
{
	cout << right << setw(60) << "Accounts List (" << accountsNumber << ") Account(s).";
	cout << "\n------------------------------------------------------------------------------------------------------------------------";
	cout << left;
	cout << setw(20) << "| Account Number";
	cout << setw(10) << "| PIN ";
	cout << setw(50) << "| Client Name ";
	cout << setw(20) << "| Phone ";
	cout << setw(20) << "| Balance";
	cout << "\n------------------------------------------------------------------------------------------------------------------------";

}
void displayBalancesHeader(short accountsNumber)
{
	cout << right << setw(60) << "Accounts List (" << accountsNumber << ") Account(s).";
	cout << "\n------------------------------------------------------------------------------------------------------------------------";
	cout << left;
	cout << setw(20) << "| Account Number";
	cout << setw(50) << "| Client Name ";
	cout << setw(20) << "| Balance";
	cout << "\n------------------------------------------------------------------------------------------------------------------------";

}
void displayActionScreen(enSecureTransaction& transaction)
{
	string actionText;

	cout << "--------------------------\n";

	switch (transaction)
	{
	case DEP:
		actionText = "DEP Operation";
		break;

	case WIT:
		actionText = "WIT Operation";
		break;

	}

	cout << setw(20) << actionText << '\n';
	cout << "--------------------------" << endl;
}
void displayActionScreen(enAdminAction& action)
{
	string actionText;

	cout << "--------------------------\n";

	switch (action)
	{
	case addUser:
		actionText = "Add User Operation";
		break;

	case deleteUser:
		actionText = "DEL User Operation";
		break;

	case updateUser:
		actionText = "UPD User Operation";
		break;

	case findUser:
		actionText = "FND User Operation";
		break;

	}

	cout << setw(20) << actionText << '\n';
	cout << "--------------------------" << endl;
}
void displayActionScreen(enAction& action)
{
	string actionText;

	cout << "--------------------------\n";

	switch (action)
	{
	case ADD:
		actionText = "ADD Account Screen";
		break;

	case DEL:
		actionText = "DEL Account Screen";
		break;

	case UPD:
		actionText = "UPD Account Screen";
		break;

	case FND:
		actionText = "FND Account Screen";
		break;

	case TRNS:
		actionText = "Transaction Screen";
		break;

	case EXT:
	default:
		actionText = " Exit Application ";
		break;

	}

	cout << setw(22) << actionText << '\n';
	cout << "--------------------------" << endl;

}
void displayAccountData(stBankAccount& bankAccount)
{
	cout << left;
	cout << setw(20) << "| " + bankAccount.accountNumber;
	cout << setw(10) << "| " + bankAccount.PINCode;
	cout << setw(50) << "| " + bankAccount.name;
	cout << setw(20) << "| " + bankAccount.phone;
	cout << setw(20) << "| " + to_string(bankAccount.balance) << endl;

}
void displayAccountBalance(stBankAccount& bankAccount)
{
	cout << left;
	cout << setw(20) << "| " + bankAccount.accountNumber;
	cout << setw(50) << "| " + bankAccount.name;
	cout << setw(20) << "| " + to_string(bankAccount.balance) << endl;

}
void displayBankAccountCard(stBankAccount& bankAccount)
{
	cout << endl;
	displayDatabaseHeader(1);
	displayAccountData(bankAccount);
	cout << "\n------------------------------------------------------------------------------------------------------------------------\n" << endl;

}
void displayAccountsData(vector <stBankAccount>& bankAccounts)
{
	for (stBankAccount& account : bankAccounts)
	{
		if (account.deleteMark)
		{
			continue;

		}

		displayAccountData(account);

	}
}
void displayAccountsBalances(vector <stBankAccount>& bankAccounts)
{
	for (stBankAccount& account : bankAccounts)
	{
		if (account.deleteMark)
		{
			continue;

		}

		displayAccountBalance(account);

	}
}
void displayBankDatabase(vector <stBankAccount>& bankAccounts)
{
	displayDatabaseHeader((short)bankAccounts.size());
	displayAccountsData(bankAccounts);

	cout << "\n------------------------------------------------------------------------------------------------------------------------\n" << endl;

}
void displayBankBalances(vector <stBankAccount>& bankAccounts)
{
	displayBalancesHeader((short)bankAccounts.size());
	displayAccountsBalances(bankAccounts);

	cout << "\n------------------------------------------------------------------------------------------------------------------------" << endl;
	cout << setw(72) << right << "Total Balances | " << calculateTotalBalances(bankAccounts)<< endl;
	cout << "------------------------------------------------------------------------------------------------------------------------\n" << endl;

}

void displayLoginScreen()
{
	cout << "--------------------------\n";
	cout << setw(20) << "Log-in Screen" << '\n';
	cout << "--------------------------" << endl;

}
void displayUserPermissions(stUserAccount& user)
{
	cout << "========================\n";
	cout << "User Name: " << activeUser.name << endl;
	cout << "Is Admin? " << (activeUser.permissionsLevel == adminPermissionLevel ? "Yes" : "No") << endl;
	cout << "Permission Level: " << activeUser.permissionsLevel << endl;

	for (stPermission& perm : activeUser.permissions)
	{
		cout << "================\n";
		cout << "Can " << perm.name << "? " << (perm.access ? "Yes" : "No") << endl;

	}
}
void displayUsersHeader(short usersNumber)
{
	cout << right << setw(60) << "User List (" << usersNumber << ") Users(s).";
	cout << "\n------------------------------------------------------------------------------------------------------------------------";
	cout << left;
	cout << setw(40) << "| User ";
	cout << setw(20) << "| PIN ";
	cout << setw(20) << "| Permission Level";
	cout << setw(15) << "| is Admin";
	cout << "\n------------------------------------------------------------------------------------------------------------------------";

}
void displayUserData(stUserAccount& user)
{
	string isAdmin = user.permissionsLevel == adminPermissionLevel ? "Yes" : "No";

	cout << left;
	cout << setw(40) << "| " + user.name;
	cout << setw(20) << "| " + user.PINCode;
	cout << setw(20) << "| " + user.permissionsLevel;
	cout << setw(15) << "| " + isAdmin << endl;

}
void displayUsersData(vector <stUserAccount>& usersAccounts)
{
	for (stUserAccount& account : usersAccounts)
	{
		if (account.deleteMark)
		{
			continue;

		}

		displayUserData(account);

	}
}
void displayUserCard(stUserAccount& userAccount)
{
	cout << endl;
	displayUsersHeader(1);
	displayUserData(userAccount);
	cout << "\n------------------------------------------------------------------------------------------------------------------------\n" << endl;

}
void displayUsers(vector <stUserAccount>& usersAccounts)
{
	displayUsersHeader((short)usersAccounts.size());
	displayUsersData(usersAccounts);

	cout << "\n------------------------------------------------------------------------------------------------------------------------\n" << endl;

}

tuple <bool, short> findAccount(vector <stUserAccount>& userAccounts, string& userName)
{
	for (int i = 0; i < userAccounts.size(); i++)
	{
		if (userAccounts[i].name == userName && !userAccounts[i].deleteMark)
		{
			return make_tuple(true, i);

		}
	}

	return make_tuple(false, 0);

}
short fetchAccountPos(vector <stUserAccount>& userAccounts)
{
	string userName;
	short accountPos;
	bool isFound = false;

	while (!isFound)
	{
		cout << "Please insert user name (-1 : Quit): ";
		getline(cin >> ws, userName);
		tie(isFound, accountPos) = findAccount(userAccounts, userName);

		if (!isFound)
		{
			cout << "========================================\n";
			cout << "Error (404): USER WAS NOT FOUND!\n" << endl;

		}
		if (userName == "-1")
		{
			return -1;

		}
	}

	system("cls");
	return accountPos;

}
stUserAccount fetchAccountData(vector <stUserAccount>& userAccounts, short& accPos)
{
	return userAccounts[accPos];

}

tuple <bool, short> findAccount(vector <stBankAccount>& bankAccounts, string& accNo)
{
	for (int i = 0; i < bankAccounts.size(); i++)
	{
		if (bankAccounts[i].accountNumber == accNo && !bankAccounts[i].deleteMark)
		{
			return make_tuple(true, i);

		}
	}

	return make_tuple(false, 0);

}
short fetchAccountPos(vector <stBankAccount>& bankAccounts)
{
	string accountNo;
	short accountPos;
	bool isFound = false;

	while (!isFound)
	{
		cout << "Please insert account number (-1 : Quit): ";
		getline(cin >> ws, accountNo);
		tie(isFound, accountPos) = findAccount(bankAccounts, accountNo);

		if (!isFound)
		{
			cout << "========================================\n";
			cout << "Error (404): ACCOUNT WAS NOT FOUND!\n" << endl;

		}
		if (accountNo == "-1")
		{
			return -1;

		}
	}

	system("cls");
	return accountPos;

}
stBankAccount fetchAccountData(vector <stBankAccount>& bankAccounts, short& accPos)
{
	return bankAccounts[accPos];

}
bool isUniqueBankNumber(vector <stBankAccount>& bankAccounts, string accountNumber)
{
	for (stBankAccount account : bankAccounts)
	{
		if (accountNumber == account.accountNumber)
		{
			if (account.deleteMark)
			{
				break;

			}

			cout << "\n=================\n";
			cout << "This account number is already used!\n" << endl;

			return false;

		}
	}

	return true;

}
string getValidUniqueNumber(vector <stBankAccount>& bankAccounts)
{
	string AccountNumber;
	string escapeSeq;

	for (short i = 0; i < accNoLen; i++)
	{
		escapeSeq += '0';

	}

	do
	{
		AccountNumber = inputString(accNoLen, accNoLen, "Enter account number (" + escapeSeq + " to Quit) : ", "INVALID ACCOUNT NUMBER!");

	} while (!isUniqueBankNumber(bankAccounts, AccountNumber) && AccountNumber != escapeSeq);

	return AccountNumber;

}

tuple <vector <stPermission>, string> readUserPermissions()
{
	short permissionLevel = 0;
	char answer;

	cout << "Give Full Access? (Y/N)";
	cin >> answer;

	if (answer == 'Y' || answer == 'y')
	{
		permissionLevel = stoi(adminPermissionLevel);

	}
	else
	{
		for (short i = 0; i < generalPermissions.size(); i++)
		{
			cout << generalPermissions.at(i).name << "? (Y/N)";
			cin >> answer;

			if (answer == 'Y' || answer == 'y')
			{
				permissionLevel += pow(2, i);

			}
		}
	}

	return make_tuple(getPermissions(permissionLevel), to_string(permissionLevel));

}
stUserAccount readUserAccount(string accNumber = addAccOpNo)
{
	stUserAccount account;
	string specialWord = "Enter ";

	if (accNumber != addAccOpNo)
	{
		specialWord = "Enter a new ";

	}
	else
	{
		cout << specialWord << "User Name: ";
		getline(cin >> ws, account.name);

	}

	cout << specialWord << "PIN code: ";
	getline(cin >> ws, account.PINCode);

	tie(account.permissions, account.permissionsLevel) = readUserPermissions();

	return account;

}
stBankAccount readBankAccount(string accNumber = addAccOpNo)
{
	stBankAccount account;
	string specialWord = "Enter ";

	if (accNumber != addAccOpNo)
	{
		specialWord = "Enter a new ";
		account.accountNumber = accNumber;

	}

	cout << specialWord << "PIN code: ";
	getline(cin >> ws, account.PINCode);

	cout << specialWord << "name: ";
	getline(cin, account.name);

	cout << specialWord << "phone: ";
	getline(cin, account.phone);

	cout << specialWord << "balance: ";
	cin >> account.balance;

	return account;

}

void addUserAccount(vector <stUserAccount>& userAccounts)
{
	userAccounts.push_back(readUserAccount());

}
void deleteUserAccount(stUserAccount& userAccount)
{
	if (userAccount.name != adminAccountName)
	{
		userAccount.deleteMark = true;

	}
}
void updateUserAccount(stUserAccount& userAccount)
{
	userAccount = readUserAccount();

}

void addBankAacount(vector <stBankAccount>& bankAccounts)
{
	string accNo = getValidUniqueNumber(bankAccounts);

	if (accNo == "0000")
	{
		return;

	}

	stBankAccount newAccount = readBankAccount(accNo);
	newAccount.accountNumber = accNo;

	bankAccounts.push_back(newAccount);

}
void deleteBankAccount(stBankAccount& bankAccount)
{
	bankAccount.deleteMark = true;

}
void updateBankAccount(stBankAccount& bankAccount)
{
	bankAccount = readBankAccount(bankAccount.accountNumber);

}

void withdraw(stBankAccount& bankAccount)
{
	string withdrawMSG = "Withdraw Amount (Min: " + to_string(minWithdrawAmount) + " | Max: " + to_string(bankAccount.balance) + "): ";
	double withdrawAmount;
	bool confirm;

	displayBankAccountCard(bankAccount);
	withdrawAmount = inputNumberInRange(10.0, bankAccount.balance, withdrawMSG);
	system("cls");

	stBankAccount previewAccount = bankAccount;
	previewAccount.balance -= withdrawAmount;

	displayBankAccountCard(previewAccount);
	confirm = readConfirmation();

	if (confirm)
	{
		bankAccount.balance -= withdrawAmount;

	}
}
void deposit(stBankAccount& bankAccount)
{
	double depositAmount;
	bool confirm;

	displayBankAccountCard(bankAccount);
	depositAmount = inputNumberInRange(0.0, DBL_MAX, "Deposit Amount: ");
	system("cls");

	stBankAccount previewAccount = bankAccount;
	previewAccount.balance += depositAmount;

	displayBankAccountCard(previewAccount);
	confirm = readConfirmation();

	if (confirm)
	{
		bankAccount.balance += depositAmount;

	}
}

void proccessSecureAction(vector <stBankAccount>& bankAccounts, enSecureTransaction& action)
{
	short accountPos = fetchAccountPos(bankAccounts);

	if (accountPos == -1)
	{
		return;

	}

	switch (action)
	{
	case DEP:
		deposit(bankAccounts.at(accountPos));
		break;

	case WIT:
		withdraw(bankAccounts.at(accountPos));
		break;
	}
}
bool performSecureTransactions(vector <stBankAccount>& bankAccounts)
{
	short actionsNumber = secureActions.size();
	string MSG = "What action to take [1 ~ ";
	MSG += actionsNumber + '0';
	MSG += "]? ";

	enSecureTransaction action = enSecureTransaction(inputNumberInRange(1, actionsNumber, MSG));
	
	system("cls");

	if (action != enSecureTransaction::CHK && action != enSecureTransaction::MNU)
	{
		displayActionScreen(action);

	}

	switch (action)
	{
	case DEP:
	case WIT:
		proccessSecureAction(bankAccounts, action);
		break;

	case CHK:
		displayBankBalances(bankAccounts);
		system("pause");
		break;

	case MNU:
	default:
		return false;
		break;
	}

	return true;

}
void startSecurePortal(vector <stBankAccount>& bankAccounts)
{
	bool repeat = true;

	while (repeat)
	{
		system("cls");
		displayOptionsScreen(secureActions, "Operations");
		repeat = performSecureTransactions(bankAccounts);

	}
}

void proccessAdminAction(vector <stUserAccount>& usersAccounts, enAdminAction& action)
{
	short accountPos = fetchAccountPos(usersAccounts);

	if (accountPos == -1)
	{
		return;

	}

	if (action == enAdminAction::deleteUser && usersAccounts.at(accountPos).name == adminAccountName)
	{
		cout << "===========================\n";
		cout << "ADMIN ACCOUNT CAN NOT BE DELETED!" << endl;

		system("pause");

		return;

	}

	switch (action)
	{
	case deleteUser:
		deleteUserAccount(usersAccounts.at(accountPos));
		break;

	case updateUser:
		updateUserAccount(usersAccounts.at(accountPos));
		break;

	case findUser:
	default:
		displayUserCard(usersAccounts.at(accountPos));
		system("pause");
		break;

	}
}
bool performAdminAction(vector <stUserAccount>& usersAccounts)
{
	short actionsNumber = adminActions.size();
	string MSG = "What action to take [1 ~ ";
	MSG += actionsNumber + '0';
	MSG += "]? ";

	enAdminAction action = enAdminAction(inputNumberInRange(1, actionsNumber, MSG));

	system("cls");

	if (action != enAdminAction::showUsers && action != enAdminAction::mainMenu)
	{
		displayActionScreen(action);

	}

	switch (action)
	{
	case showUsers:
		displayUsers(usersAccounts);
		system("pause");
		break;

	case addUser:
		addUserAccount(usersAccounts);
		break;

	case deleteUser:
	case updateUser:
	case findUser:
		proccessAdminAction(usersAccounts, action);
		break;

	case mainMenu:
	default:
		return false;
		break;

	}

	return true;

}
void accessAdminPanel(vector <stUserAccount>& usersAccounts)
{
	bool repeat = true;

	while (repeat)
	{
		system("cls");
		displayOptionsScreen(adminActions, "Admin Panel");
		repeat = performAdminAction(usersAccounts);

	}
}

void proccessAction(vector <stBankAccount>& bankAccounts, enAction& action)
{
	short accountPos = fetchAccountPos(bankAccounts);

	if (accountPos == -1)
	{
		return;

	}

	switch (action)
	{
	case DEL:
		deleteBankAccount(bankAccounts.at(accountPos));
		break;

	case UPD:
		updateBankAccount(bankAccounts.at(accountPos));
		break;

	case FND:
	default:
		displayBankAccountCard(bankAccounts.at(accountPos));
		system("pause");
		break;

	}
}
bool performAction(vector <stUserAccount>& usersAccounts, vector <stBankAccount>& bankAccounts)
{
	short actionsNumber = menuActions.size();
	string MSG = "What action to take [1 ~ ";
	MSG += actionsNumber + '0';
	MSG += "]? ";

	enAction action = enAction(inputNumberInRange(1, actionsNumber, MSG));

	system("cls");

	if ((action - 1) < generalPermissions.size() && !activeUser.permissions.at(action - 1).access)
	{
		cout << "\n===========================\n";
		cout << "Access Denied!" << endl;
		cout << "Access Level: " << action - 1 << endl;
		cout << "Access: " << activeUser.permissions.at(action - 1).name << endl;

		system("pause");

		return true;

	}

	if (action != enAction::DIS && action != enAction::TRNS || action != enAction::EXT)
	{
		displayActionScreen(action);

	}

	switch (action)
	{
	case DIS:
		displayBankDatabase(bankAccounts);
		system("pause");
		break;

	case ADD:
		addBankAacount(bankAccounts);
		break;

	case DEL:
	case UPD:
	case FND:
		proccessAction(bankAccounts, action);
		break;

	case TRNS:
		startSecurePortal(bankAccounts);
		break;

	case USR:
		accessAdminPanel(usersAccounts);
		break;

	case EXT:
	default:
		return false;

	}

	return true;

}

bool logIn(vector <stUserAccount>& usersAccounts)
{
	displayLoginScreen();
	activeUser = stUserAccount();

	string userName;
	string PINCode;
	short accountPos;
	bool isFound = false;

	cout << "Enter User Name: ";
	getline(cin, userName);

	cout << "Enter PIN Code: ";
	getline(cin, PINCode);

	tie(isFound, accountPos) = findAccount(usersAccounts, userName);

	if (isFound)
	{
		activeUser = usersAccounts.at(accountPos);

		if (usersAccounts.at(accountPos).PINCode == PINCode)
		{

			system("cls");
			displayUserPermissions(activeUser);
			system("pause");

			return false;

		}
	}

	return true;

}

void startProgram()
{
	vector <string> bankRecords = getBankRecords();
	vector <stBankAccount> bankAccounts = unpackBankRecords(bankRecords);
	
	vector <string> usersRecords = getUsersRecords();
	vector <stUserAccount> usersAccounts = unpackUsersRecords(usersRecords);

	bool relog = true;
	bool repeat = true;

	while (true)
	{
		repeat = true;
		relog = logIn(usersAccounts);

		if (relog)
		{
			system("cls");
			cout << "Invalid User Name/PINCode!\n" << endl;
			continue;

		}

		while (repeat)
		{
			system("cls");
			displayOptionsScreen();
			repeat = performAction(usersAccounts, bankAccounts);

			bankRecords = packBankRecords(bankAccounts);
			usersRecords = packUsersRecords(usersAccounts);

		}
	}

	saveBankRecords(bankRecords);
	saveUsersRecords(usersRecords);

}

int main()
{
	startProgram();

	return 0;

}
