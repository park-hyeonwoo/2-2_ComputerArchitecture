#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
using namespace std;

class Cache {
public:
	int sets;			// the number of sets in a cache
	int blocks;			// the number of blocks in a set
	int words;			// the number of words in a block
};

int main(int argc, char* argv[]) {
	Cache c;				// create an instance of class Cache
	c.sets = atoi(argv[2]);			// get the value of sets
	c.blocks = atoi(argv[4]);		// get the value of blocks
	c.words = atoi(argv[6]);		// get the value of words
	
	int setnum = (int)log2(c.sets);			// take a log of the values
	int blocknum = (int)log2(c.blocks);
	int wordnum = (int)log2(c.words);

	ifstream input;			// read the input file
	input.open(argv[7]);
	string line[20];
	string adr[20];
	int count = 0;			// the number of addresses in the input file

	if (input.is_open()) {
		while (getline(input, line[count])) {
			count++;
		}
	}
	input.close();

	for (int i = 0; i < count; i++) {				// convert Hex to Binary
		for (int j = 2; j < 10; j++) {
			switch (line[i][j]) {
			case '0': adr[i] += "0000"; break;
			case '1': adr[i] += "0001"; break;
			case '2': adr[i] += "0010"; break;
			case '3': adr[i] += "0011"; break;
			case '4': adr[i] += "0100"; break;
			case '5': adr[i] += "0101"; break;
			case '6': adr[i] += "0110"; break;
			case '7': adr[i] += "0111"; break;
			case '8': adr[i] += "1000"; break;
			case '9': adr[i] += "1001"; break;
			case 'A': adr[i] += "1010"; break;
			case 'B': adr[i] += "1011"; break;
			case 'C': adr[i] += "1100"; break;
			case 'D': adr[i] += "1101"; break;
			case 'E': adr[i] += "1110"; break;
			case 'F': adr[i] += "1111"; break;
			}
		}
	}

	string** content = new string*[c.sets];		// declare and initialize cache block two-dimensional array
	for (int i = 0; i < c.sets; ++i) {
		content[i] = new string[c.blocks];
	}

	for (int i = 0; i < c.sets; i++)
		for (int j = 0; j < c.blocks; j++)
			content[i][j] = "0";

	string block[20];								// obtain block address
	int banum = 32 - (wordnum + 2);
	for (int i = 0; i < count; i++) {
		block[i] = adr[i].substr(0, banum);
	}

	string tag[20];									// obtain tag value
	for (int i = 0; i < count; i++) {
		tag[i] = adr[i].substr(0, banum - setnum);
	}

	int index[20];									// obtain set index value
	for (int i = 0; i < count; i++) {
		string idx = block[i].substr(banum - setnum, setnum);
		index[i] = 0;
		int e = 1;
		for (int j = setnum - 1; j >= 0; j--) {
			for (int k = 0; k < setnum - j - 1; k++)
				e *= 2;
			if (idx[j] == '1')
				index[i] += e;
			e = 1;
		}
	}

	int hitnum = 0;					// the number of cache hits
	int missnum = 0;				// the number of cache misses
	int *LRU = new int[c.sets];		// index value of LRU block
	for (int i = 0; i < c.sets; i++)
		LRU[i] = 0;

	ofstream output;				
	output.open("output.txt");

	for (int i = 0; i < count; i++) {			
		for (int j = 0; j < c.sets; j++) {
			if (index[i] == j) {
				bool check = false;
				for (int k = 0; k < c.blocks; k++) {
					if (content[j][k] == "0") {
						output << line[i] << ": miss" << endl;		// miss (copied from lower level)
						content[j][k] = tag[i];
						check = true;
						missnum++;
						break;
					}
					else if (content[j][k] == tag[i]) {		
						output << line[i] << ": hit" << endl;		// hit (valid bit == 1 && tag == 11)
						check = true;
						hitnum++;
						break;
					}
				}
				if (check == false) {
					output << line[i] << ": miss" << endl;
					content[j][LRU[j]] = tag[i];					// replace LRU block
					missnum++;
					LRU[j] = (LRU[j] + 1) % c.blocks;				// update index value of LRU block
				}
			}
		}
	}

	output << "# of cache hits: " << hitnum << endl;				// write to output file
	output << "# of cache misses: " << missnum << endl;
	output.close();
}