#pragma once
#include"huffman.h"
#include<cstring>
#include<cstdlib>
using namespace std;


struct CharInfo
{
public:

	CharInfo(int count=0)
	:_count(count)
	{
	}

	unsigned char _ch;
	size_t _count;
	string _code;

	bool operator !=(const CharInfo&  d)
	{
		return _count != d._count;
	}
	CharInfo operator+(const CharInfo& d)
	{
		return CharInfo(_count + d._count);
	}
	bool operator <(const CharInfo& d) const
	{
		return _count < d._count;
	}
};


class FileCompress
{
public:
	FileCompress()
	{
		for (int i = 0; i < 256; i++)
		{
			_arr[i]._ch = i;
			_arr[i]._count = 0;
		}
	}
public:
	void Compress(const char *filename)
	{
		assert(filename);

		FILE *fout = fopen(filename, "r");
		assert(fout);
		//统计出现的次数
		int ch = fgetc(fout);

		while (ch != EOF)
		{
			_arr[ch]._count++;
			ch = fgetc(fout);
		}

		//构建哈夫曼树
		CharInfo Invalid;
		HuffmanTree<CharInfo> ht(_arr, 256, Invalid);

		//创建哈夫曼编码
		string code;
		_CreateHuffmanCode(ht.Getroot(), code);

		//压缩
		string CompressFileName = filename;
		size_t pos = CompressFileName.rfind('.');	//查找.最后出现的位置
		CompressFileName = CompressFileName.substr(0, pos);//取得.之前的字符串
		CompressFileName += ".compress";
		FILE* fin = fopen(CompressFileName.c_str(), "w");
		assert(fin);
		fseek(fout, 0, SEEK_SET);
		ch = (unsigned char)fgetc(fout);
		int size = 0;
		char value = 0;
		while (ch != EOF)
		{
			string code = _arr[ch]._code;
			for (size_t i = 0; i < code.size(); i++)
			{
				if (code[i] == '1')
				{
					value <<= 1;
					value |= 1;
					size++;
				}
				else if (code[i] == '0')
				{
					value <<= 1;
					size++;
				}
				if (size == 8)
				{
					fputc(value, fin);
					size = 0;
					value = 0;
				}
			}
			ch = fgetc(fout);
		}
		if (size != 0)
		{
			value <<= (8 - size);
			fputc(value, fin);
		}
		//写一个帮助文件，用来保存所有的字符出现的次数。
		string CompressConfigFileName = filename;
		pos = CompressConfigFileName.rfind('.');	//查找.最后出现的位置
		CompressConfigFileName = CompressConfigFileName.substr(0, pos);//取得.之前的字符串
		CompressConfigFileName += ".config";
		FILE* fconin = fopen(CompressConfigFileName.c_str(), "w");
		assert(fconin);

		string str;
		char buf[128] = { 0 };
		for (int i = 0; i < 256; i++)
		{
			if (_arr[i]._count>0)
			{
				str += _arr[i]._ch;
				str += ',';
				_itoa(_arr[i]._count, buf, 10);
				str += buf;
				str += '\n';

				fputs(str.c_str(), fconin);
				str.clear();
			}

		}


		fclose(fconin);
		fclose(fout);
		fclose(fin);
	}
	//解压缩
	void uncompression(const char *filename)
	{
		assert(filename);
		//读取配置文件
		string configname = filename;
		size_t pos = configname.rfind('.');	//查找.最后出现的位置
		configname = configname.substr(0, pos);
		configname += ".config";
		FILE *fconfigout = fopen(configname.c_str(), "r");
		assert(fconfigout);

		size_t count = 0;
		string str;
		while (ReadLine(fconfigout,str))
		{
			if (str.empty())
			{
				str += '\n';
				count += 1;
				str.clear();
			}
			else
			{	
				//考虑中文字符，所以在这要去取unsigned char
				_arr[(unsigned char)str[0]]._count = atoi(str.substr(2).c_str());
				count += _arr[(unsigned char)str[0]]._count;
				str.clear();
			}
		}
		
		//接下来进行哈夫曼树的构建、
		CharInfo Invalid;
		HuffmanTree<CharInfo> ht(_arr, 256, Invalid);

		string uncomperessfilename = filename;
		pos = uncomperessfilename.rfind('.');	//查找.最后出现的位置
		uncomperessfilename = uncomperessfilename.substr(0, pos);//取得.之前的字符串
		uncomperessfilename += ".uncompress";

		string compressfilename = filename;
		pos = compressfilename.rfind('.');	//查找.最后出现的位置
		compressfilename = compressfilename.substr(0, pos);//取得.之前的字符串
		compressfilename += ".compress";

		FILE* fin = fopen(uncomperessfilename.c_str(), "w");
		FILE* fout = fopen(compressfilename.c_str(), "r");
		HuffmanTreeNode<CharInfo> *cur = ht.Getroot();

		int size = 7;
		size_t ch = fgetc(fout);
		while (ch!=EOF)
		{
			if (ch&(1 << size))
			{
				cur = cur->_right;
			}
			else
			{
				cur = cur->_left;
			}
			size--;
			if (cur->_left == NULL&&cur->_right == NULL)
			{
				fputc(cur->_wight._ch, fin);
				cur = ht.Getroot();
				if ((--count) == 0)
				{
					break;
				}

			}
			if (size < 0)
			{
				ch = fgetc(fout);
				size = 7;

			}

		}
		fclose(fin);
		fclose(fout);
		fclose(fconfigout);
	}
protected:
	//进行一行的读取。
	bool ReadLine(FILE*& fconfigout, string & str)
	{
		int ch = fgetc(fconfigout);
		if (ch == EOF)
		{
			return false;
		}
		while (ch != EOF&&ch != '\n')
		{
			str += ch;
			ch = fgetc(fconfigout);
		}
		return true;
	}
protected:
	void _CreateHuffmanCode(HuffmanTreeNode<CharInfo >* root, string code)
	{
		if (root == NULL)
		{
			return;
		}
		if (root->_left == NULL&&root->_right == NULL)
		{
			_arr[root->_wight._ch]._code = code;
		}
		if (root->_left )
		{
			_CreateHuffmanCode(root->_left, code + '0');
		}
		if (root->_right)
		{
			_CreateHuffmanCode(root->_right, code + '1');
		}
		
		
	}

protected:
	CharInfo _arr[256];
};