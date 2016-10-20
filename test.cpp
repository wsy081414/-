#pragma once
#include"huffman.h"
#include"FileCompress.h"

void test()
{ 
	FileCompress h;
	h.Compress("test.txt");
	h.uncompression("test.compress");
	


}
int main()
{
	test();
	system("pause");
	return 0;
}