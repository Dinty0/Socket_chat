//Pizza.cpp
//Created and Editted on May 7th, 2008

#include <iostream>
#include <iomanip>
#include <string>

using std::cout;
using std::cin;
using std::endl;
using std::setw;
using std::string;

string menu();
void recieved(float [], string []);
void inventory(float [], int, string [], int);
void used(float [], string []);
void convert();

string procede = "";

string menu()
{
	char choice = 'a';
	string type = "";
	//Purpose: To show the menu so the user can show
	//Recieves: Nothing
	//Returns: Nothing
	
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl
		<< "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl
		<< "~          The Restraunt of Hashbrowns and Pizza          ~" << endl
		<< "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl
		<< "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl
		<< "                                                           " << endl
		<< "::::::::::::::::::::::INSTRUCTIONS:::::::::::::::::::::::::" << endl
		<< "                                                           " << endl
		<< "            Please Enter the Letter of The Menu            " << endl
		<< "                                                           " << endl
		<< ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::" << endl
		<< "~~              a) Inventory Display                     ~~ " << endl
		<< "~~              b) Orders Recieved                       ~~" << endl
		<< "~~              c) Quantity Used                         ~~" << endl
		<< "~~              d) Conversions                           ~~" << endl
		<< ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::" << endl
		<< "Please enter a letter of your choice [a,b,c,d]: ";
	cin >> choice;
	cin.ignore(256,'\n');

	switch (choice)
	{
		case 'a':
		case 'A':
			type = "inventory";
			break;
		case 'b':
		case 'B':
			type = "recieved";
			break;
		case 'c':
		case 'C':
			type = "used";
			break;
		case 'd':
		case 'D':
			type = "convert";
			break;
		default:
			system("cls");
			cout << " You have not entered a valid choice! " << endl << endl;
			precede = menu();
	}
	return type;
}

void inventory(float amount[], int amountSize, string food[], int foodSize)
{
	system("cls");
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl
		<< "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl
		<< "~               The Restraunt of Hashbrowns and Pizza              ~" << endl
		<< "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl
		<< "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
	cout << "#####################################################################" << endl
		<< "########                   The Inventory                     ########" << endl
		<< "#####################################################################" << endl << endl
		<< "* There are: " << endl << endl;
	for (int count = 0; count < 10; count++)
	{
		cout << amount[count] << " " << food[count] << endl;
	}

}
void used()
{
	cout << "USED PROCEDURE" << endl;
}
void recieved(float amount[], string food[])
{
	int chooseNum  = 0;
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl
		<< "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl
		<< "~               The Restraunt of Hashbrowns and Pizza              ~" << endl
		<< "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl
		<< "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
	cout << "####################################################################" << endl
		<< "#######             Supplies Recieved [BOXES]                #######" << endl
		<< "####################################################################" << endl << endl
		<< "Please choose an option to update: " << endl;
	for (int count = 0; count < 10; count++)
	{
		chooseNum = count + 1;
		cout << chooseNum << ") " << food[count] << " has " << amount[count] << " of boxes." << endl;
	}
	cout << "Please enter a number: ";
	cin >> chooseNum;
	cin.ignore(256,'\n');
	
	switch (chooseNum)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			cout << "Please enter the number of boxes for " << food[(chooseNum-1)] <<" : ";
			cin >> amount[(chooseNum-1)];
			cin.ignore(256,'\n');
			break;
		default:
			system("cls");
			cout << "Please enter a correct option! You have been redirected to the main menu to retry.";
			precede = menu();
	}
	system("cls");
	cout << "The new updated inventory is: " << endl;
	for (int count = 0; count < 10; count++)
	{
		chooseNum = count + 1;
		cout << chooseNum << ") " << food[count] << " has " << amount[count] << " of boxes." << endl;
	}
	system("pause");
	

	//cout << "RECIEVED PROCEDURE" << endl;
}
void convert()
{
	cout << "CONVERT PROCEDURE" << endl;
}


int main()
{
	//Declare the Variables
	string names[10] = {"Pepperoni","Italian Sausage","Bacon","Green Pepper","Onion","Green Olives","Mushrooms","Pizza Sauce","Dough","Cheese"};
	float quantity[10];
	int convert [10];
	string procede = "";
	cin.ignore(256,'\n');
	for (int loop = 0; loop < 10; loop++)
	{
		quantity[loop] = 0.0;
	}
	for (int loop2 = 0; loop2 < 10; loop2++)
	{
		convert[loop2] = 0;
	}

	//Tell the user what is going to happen
	cout << "This program will track the inventory of the pizza company." << endl;
	procede = menu();
	//system("pause");
	if (procede == "inventory")
	{
		inventory(quantity, 10, names, 10);
	}
	else if (procede == "used")
	{

	}
	else if (procede == "recieved")
	{
		recieved(quantity, names);
	}
	else
	{

	}
	//system("cls");
	system("pause");

	return 0;
}