#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>

using namespace std;

int main(void)
{

	int nb_elm;

	cerr << "Hello" << endl;

	cin >> nb_elm;cin.ignore();
//cin.ignore();


//	cerr << "NB:" << nb_elm << endl;
	cout << nb_elm << endl;

	for (int i = 0; i <= nb_elm; i++) {
		float weight;
		float volume;

		cin >> weight;cin.ignore();
		cin >> volume;cin.ignore();
//cin.ignore();

//		cerr << "Received " << i << "[" << weight << "/" << volume << "]" << endl;
//		cout << weight << volume << endl;
//		fprintf(stdout, "%8.8f %8.8f\n", weight, volume);
std::cout << std::fixed << std::setprecision(3) << weight;
cout << " ";
std::cout << std::fixed << std::setprecision(3) << volume;
cout << endl;
	}

	return 0;
}
