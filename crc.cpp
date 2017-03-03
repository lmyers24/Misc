#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

class CRCElement{ 				//a nice little thing to pass around so we can keep track of important variables.
public:
	unsigned long table[256]; 	//table of values for crc
	unsigned long polynomial;	//this is just to unify stuff we pass around a little more.
	unsigned long crc;			//will be the generated crc
};

CRCElement makeCRCTable(CRCElement element){
	unsigned long polynomial = 0xedb88320;
	unsigned long loc;

	for (int i=0; i<256; i++){
		loc = (unsigned long) i;
		for (int j = 0; j<8; j++){
			if (loc & 1)
				loc = polynomial ^ (loc >> 1);
			else
				loc = (loc >> 1);
		}
		element.table[i] = loc;
	}
	return element;
}

CRCElement getCRC(CRCElement element, unsigned int *buf, int len){
	unsigned long loc = element.polynomial; 		//ffffffff
	element = makeCRCTable(element); 				//generates the crc table for our crc element
	for (int i=0; i<len; i++){
		loc = element.table[(loc^buf[i]) & 0xff] ^ (loc>>8);
	}
	element.crc = loc ^ element.polynomial;
	return element;
} 	

unsigned int* hexFormatString (char *str, unsigned int *array, int len){
	unsigned char entry[3];
	entry[2] = '\0';							 	//make sure we have the ending in place
	int k=0; 										//to know which character we are on from str
	int loc=0; 										//to know what are position is in array
	for (int i=0; i < len; i++){
		if (k==0){ 									//if this is the first element of our 2 byte portion
			entry[0] = str[i];						//keep the char value
			k=1;									//lets us know we are going to be working with the 2nd element of a 2 byte bit
		}
		else{										//if this is the second element of our 2 byte portion
			entry[1] = str[i];						//create the 2 byte portion we want
			int a = strtol((char*)entry, NULL, 16); //creates an int = the hex value of the 2 byte
			array[loc] = (unsigned int)a;			//assignes the int to the next position in the array
			loc++;									//move position forward
			k=0;									//lets us know we are back to looking for the 1st element in a 2 byte portion
		}
	}
	return array;
}

unsigned long generateValidCRC(char *str){		 	//function to call to generate a crc
	CRCElement element;								//creates an element that will be passed around
	element.polynomial = 0xffffffff;				//creates a polynomial value we will want later
	int len = strlen(str);							//get length of string passed in
	unsigned int *test = new unsigned int[len*3];	//create an int array we know will be long enough to hold decimal values of our hex string
	test = hexFormatString(str, test, len);			//formats the string to send into the getCRC function
	element = getCRC(element, test, 17);			//calls the function that will create the crc
	return element.crc;								//returns the unsigned long form of the crc
}	

int main(){  										//main just here to test and make sure generateValidCRC does what I want.
	char whatever[] = "4948445200000001000000010100000000";
	generateValidCRC(whatever); 					//this will return the long in decimal form. It just needs to be converted to hex.
	return 0;
}
