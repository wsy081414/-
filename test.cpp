#pragma once
#include"huffman.h"
#include"FileCompress.h"
#include<time.h>

void test()
{ 
	FileCompress h;
	clock_t start, finish;
	start = clock();
	h.Compress("s.mp4");
	finish = clock();
	cout << finish - start << endl;
	h.uncompression("s.mp4.compress");
	


}
int main()
{
	test();
	system("pause");
	return 0;
}