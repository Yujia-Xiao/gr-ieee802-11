/*
 * shm.cpp
 * 
 * copy these files along with this file
 * sshm.c sshm.h
 * 
 * compile using 
 * gcc -o sshm.o sshm.c
 * g++ shm shm.cpp sshm.o
 * 
 * 
 * run
 * ./shm
 * 
 * 
 * */
 

#include <iostream>
#include "sshm.h"

using namespace std;

int main()
{
	double input;
	
	int segmentid;
	double * power;
	segmentid=shm_get(123456,(void**)&power,sizeof(double));
		
	if (segmentid<0) {
		cout << "Error creating segmentid" << endl; 
		return 0;
	}
	else {
		cout << "segment ID is " << segmentid << endl;
		cout << "address of power " <<power <<endl;
	}
	
	while (input!=0) {
		cout << "Enter the power level\n";
		cin >> input;
		*power = input;
		cout << "Power level: " << *power << endl;
	}
	cout << "removing shared memory " <<segmentid <<endl;
	shm_rm(segmentid);
	
}
