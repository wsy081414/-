#pragma once
#include"huffman.h"
#include<cstring>
#include<cstdlib>
using namespace std;

typedef long long longtype;

struct CharInfo
{
public:

	CharInfo(longtype count=0)
	:_count(count)
	{
	}

	unsigned char _ch;
	longtype _count;
	string _code;

	bool operator !=(const CharInfo  d)
	{
		return _count != d._count;
	}
	CharInfo operator+(const CharInfo d)
	{
		return CharInfo(_count + d._count);
	}
	bool operator <(const CharInfo d) const
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
	//压缩
	void Compress(const char *filename)
	{
		assert(filename);

		FILE *fout = fopen(filename, "rb");
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
		//size_t pos = CompressFileName.rfind('.');	//查找.最后出现的位置
		//CompressFileName = CompressFileName.substr(0, pos);//取得.之前的字符串
		CompressFileName += ".compress";
		FILE* fin = fopen(CompressFileName.c_str(), "wb");
		assert(fin);

		//统计完次数以后，这个时候fout位于文件流的最后，这个时候我们需要给流进行重新定位到开始处。
		fseek(fout, 0, SEEK_SET);

		//进行压缩的时候首先读取流中的字符
		ch = fgetc(fout);
		int size = 0;
		unsigned char value = 0;
		while (ch != EOF)
		{

			//得到对应字符的哈夫曼编码
			string code = _arr[ch]._code;

			for (size_t i = 0; i < code.size(); i++)
			{
				//进行对哈夫曼编码的写入压缩文件
				//每8位写入一个字节

				//得到哈夫曼编码，将哈夫曼编码存入value中
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

				//当到达1个字节的时候，进行写入。
				if (size == 8)
				{
					//写入文件流当中
					fputc(value, fin);
					size = 0;
					value = 0;
				}
			}
			ch =getc(fout);
		}
		//如果不够一个字节，进行填充至一个字节。
		if (size != 0)
		{
			value <<= (8 - size);
			fputc(value, fin);
		}

	


		//写一个帮助文件，用来保存所有的字符出现的次数。
		string CompressConfigFileName = filename;
		//pos = CompressConfigFileName.rfind('.');	//查找.最后出现的位置
		//CompressConfigFileName = CompressConfigFileName.substr(0, pos);//取得.之前的字符串
		CompressConfigFileName += ".config";
		FILE* fconin = fopen(CompressConfigFileName.c_str(), "wb");
		assert(fconin);

		//帮助文件进行统计每个字符出现的次数。
		string str;
		char buf[128] = { 0 };
		for (int i = 0; i < 256; i++)
		{
			if (_arr[i]._count>0)
			{
				//先写入字符
				fputc(_arr[i]._ch, fconin);
				str += ',';
				//buf用来保存字符对应的次数，将这个次数转换成字符串。
				_itoa((int)_arr[i]._count, buf, 10);
				str += buf;
				//写完一个字符，进行换行。
				str += '\n';

				fputs(str.c_str(), fconin);
				str.clear();
			}

		}
		//这里吧类中的映射表进行归0操作，防止因为任务不同，映射表发生混乱。
		for (int i = 0; i < 256; i++)
		{
			_arr[i]._count = 0;
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

		//二进制流进行配置文件的读取
		FILE *fconfigout = fopen(configname.c_str(), "rb");
		assert(fconfigout);

		longtype count = 0;
		string str;
		//先从配置文件当中读取一行
		while (ReadLine(fconfigout,str))
		{
			
			//如果为空，说明是空行，只需要换行就可以。
			//在这进行考虑‘/n’或者‘/r’。
			if (str.empty())
			{
				str += '\n';

			}
			else
			{	
				//考虑中文字符，所以在这要去取unsigned char
				//这里要知道string中存储的依然是字符，一个字符限定在-128——127之间，汉字是由128之后的字符组成，所以，在这里要转换unsigned char
				//把字符串转化为数字，然后保存到对应字符的count域里面
				_arr[(unsigned char)str[0]]._count = atoi(str.substr(2).c_str());
				
				count += _arr[(unsigned char)str[0]]._count;
				
				str.clear();
			}
		}
		
		//接下来进行哈夫曼树的构建、
		CharInfo Invalid;
		HuffmanTree<CharInfo> ht(_arr, 256, Invalid);

		string uncomperessfilename = filename;
		//pos = uncomperessfilename.rfind('.');	//查找.最后出现的位置
		//uncomperessfilename = uncomperessfilename.substr(0, pos);//取得.之前的字符串
		uncomperessfilename += ".uncompress";

		string compressfilename = filename;
		//pos = compressfilename.rfind('.');	//查找.最后出现的位置
		//compressfilename = compressfilename.substr(0, pos);//取得.之前的字符串
		//compressfilename += ".compress";

		FILE* fin = fopen(uncomperessfilename.c_str(), "wb");
		FILE* fout = fopen(compressfilename.c_str(), "rb");
		HuffmanTreeNode<CharInfo> *cur = ht.Getroot();
		HuffmanTreeNode<CharInfo> *root = ht.Getroot();

		//进行堆的查找，实现压缩所还原对应的字符
		int size = 7;
		//读取哈夫曼编码
		int  ch = fgetc(fout);
		//判断读取的是否为文件结束位
		while (ch!=EOF)
		{
			//这里根据哈夫曼编码的每一位进行堆的遍历，查找到你所需要的字符所对应的叶子节点。
			//对每一位进行位处理判断，如果是1，那么去哈夫曼树的右树。
			if (ch&(1 << size))
			{
				cur = cur->_right;
			}
			//如果是0去哈夫曼的左树查找。
			else
			{
				cur = cur->_left;
			}
			//如果是叶子节点了，这个时候我们需要对此进行输出对应的字符，
			if (cur->_left == NULL&&cur->_right == NULL)
			{
				fputc(cur->_wight._ch, fin);

				//输出后要重新回到根进行比较
				cur = root;
				count--;
				//当所有的字符全部写完，退出。
				if (count == 0)
				{
					break;
				}
			}
			//进行位控制，size还可以维护上面的位运算。
			size--;
			//说明一个字符的每一位都已经取完了。这个时候我们就可以进行下一个字符了。

			if (size < 0)
			{
				ch = fgetc(fout);
				size = 7;
			}
		}
		//最后进行关闭文件流
		fclose(fin);
		fclose(fout);
		fclose(fconfigout);
	}
protected:
	//进行一行的读取。
	bool ReadLine(FILE*& fconfigout, string & str)
	{
		//从配置文件读出一个字符
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
		//while结束后，在最后就是str记录了这一行除了\n以外的字符。在外部，会对\n添加。
		return true;
	}
protected:

	//递归创建哈夫曼编码
	void _CreateHuffmanCode(HuffmanTreeNode<CharInfo >* root, string code)
	{
		//为空，返回
		if (root == NULL)
		{
			return;
		}
		//叶子节点就可以找到对应的字符，这个时候给出它的哈夫曼编码。
		if (root->_left == NULL&&root->_right == NULL)
		{
			_arr[(unsigned char)root->_wight._ch]._code = code;
		}
		if (root->_left)
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