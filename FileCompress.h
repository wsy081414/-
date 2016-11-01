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
	//ѹ��
	void Compress(const char *filename)
	{
		assert(filename);

		FILE *fout = fopen(filename, "rb");
		assert(fout);
		//ͳ�Ƴ��ֵĴ���
		int ch = fgetc(fout);

		while (ch != EOF)
		{
			_arr[ch]._count++;
			ch = fgetc(fout);
		}

		//������������
		CharInfo Invalid;
		HuffmanTree<CharInfo> ht(_arr, 256, Invalid);

		//��������������
		string code;
		_CreateHuffmanCode(ht.Getroot(), code);

		//ѹ��
		string CompressFileName = filename;
		//size_t pos = CompressFileName.rfind('.');	//����.�����ֵ�λ��
		//CompressFileName = CompressFileName.substr(0, pos);//ȡ��.֮ǰ���ַ���
		CompressFileName += ".compress";
		FILE* fin = fopen(CompressFileName.c_str(), "wb");
		assert(fin);

		//ͳ��������Ժ����ʱ��foutλ���ļ�����������ʱ��������Ҫ�����������¶�λ����ʼ����
		fseek(fout, 0, SEEK_SET);

		//����ѹ����ʱ�����ȶ�ȡ���е��ַ�
		ch = fgetc(fout);
		int size = 0;
		unsigned char value = 0;
		while (ch != EOF)
		{

			//�õ���Ӧ�ַ��Ĺ���������
			string code = _arr[ch]._code;

			for (size_t i = 0; i < code.size(); i++)
			{
				//���жԹ����������д��ѹ���ļ�
				//ÿ8λд��һ���ֽ�

				//�õ����������룬���������������value��
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

				//������1���ֽڵ�ʱ�򣬽���д�롣
				if (size == 8)
				{
					//д���ļ�������
					fputc(value, fin);
					size = 0;
					value = 0;
				}
			}
			ch =getc(fout);
		}
		//�������һ���ֽڣ����������һ���ֽڡ�
		if (size != 0)
		{
			value <<= (8 - size);
			fputc(value, fin);
		}

	


		//дһ�������ļ��������������е��ַ����ֵĴ�����
		string CompressConfigFileName = filename;
		//pos = CompressConfigFileName.rfind('.');	//����.�����ֵ�λ��
		//CompressConfigFileName = CompressConfigFileName.substr(0, pos);//ȡ��.֮ǰ���ַ���
		CompressConfigFileName += ".config";
		FILE* fconin = fopen(CompressConfigFileName.c_str(), "wb");
		assert(fconin);

		//�����ļ�����ͳ��ÿ���ַ����ֵĴ�����
		string str;
		char buf[128] = { 0 };
		for (int i = 0; i < 256; i++)
		{
			if (_arr[i]._count>0)
			{
				//��д���ַ�
				fputc(_arr[i]._ch, fconin);
				str += ',';
				//buf���������ַ���Ӧ�Ĵ��������������ת�����ַ�����
				_itoa((int)_arr[i]._count, buf, 10);
				str += buf;
				//д��һ���ַ������л��С�
				str += '\n';

				fputs(str.c_str(), fconin);
				str.clear();
			}

		}
		//��������е�ӳ�����й�0��������ֹ��Ϊ����ͬ��ӳ��������ҡ�
		for (int i = 0; i < 256; i++)
		{
			_arr[i]._count = 0;
		}

		fclose(fconin);
		fclose(fout);
		fclose(fin);
	}
	//��ѹ��
	void uncompression(const char *filename)
	{
		assert(filename);
		//��ȡ�����ļ�
		string configname = filename;
		size_t pos = configname.rfind('.');	//����.�����ֵ�λ��
		configname = configname.substr(0, pos);
		configname += ".config";

		//�����������������ļ��Ķ�ȡ
		FILE *fconfigout = fopen(configname.c_str(), "rb");
		assert(fconfigout);

		longtype count = 0;
		string str;
		//�ȴ������ļ����ж�ȡһ��
		while (ReadLine(fconfigout,str))
		{
			
			//���Ϊ�գ�˵���ǿ��У�ֻ��Ҫ���оͿ��ԡ�
			//������п��ǡ�/n�����ߡ�/r����
			if (str.empty())
			{
				str += '\n';

			}
			else
			{	
				//���������ַ�����������Ҫȥȡunsigned char
				//����Ҫ֪��string�д洢����Ȼ���ַ���һ���ַ��޶���-128����127֮�䣬��������128֮����ַ���ɣ����ԣ�������Ҫת��unsigned char
				//���ַ���ת��Ϊ���֣�Ȼ�󱣴浽��Ӧ�ַ���count������
				_arr[(unsigned char)str[0]]._count = atoi(str.substr(2).c_str());
				
				count += _arr[(unsigned char)str[0]]._count;
				
				str.clear();
			}
		}
		
		//���������й��������Ĺ�����
		CharInfo Invalid;
		HuffmanTree<CharInfo> ht(_arr, 256, Invalid);

		string uncomperessfilename = filename;
		//pos = uncomperessfilename.rfind('.');	//����.�����ֵ�λ��
		//uncomperessfilename = uncomperessfilename.substr(0, pos);//ȡ��.֮ǰ���ַ���
		uncomperessfilename += ".uncompress";

		string compressfilename = filename;
		//pos = compressfilename.rfind('.');	//����.�����ֵ�λ��
		//compressfilename = compressfilename.substr(0, pos);//ȡ��.֮ǰ���ַ���
		//compressfilename += ".compress";

		FILE* fin = fopen(uncomperessfilename.c_str(), "wb");
		FILE* fout = fopen(compressfilename.c_str(), "rb");
		HuffmanTreeNode<CharInfo> *cur = ht.Getroot();
		HuffmanTreeNode<CharInfo> *root = ht.Getroot();

		//���жѵĲ��ң�ʵ��ѹ������ԭ��Ӧ���ַ�
		int size = 7;
		//��ȡ����������
		int  ch = fgetc(fout);
		//�ж϶�ȡ���Ƿ�Ϊ�ļ�����λ
		while (ch!=EOF)
		{
			//������ݹ����������ÿһλ���жѵı��������ҵ�������Ҫ���ַ�����Ӧ��Ҷ�ӽڵ㡣
			//��ÿһλ����λ�����жϣ������1����ôȥ����������������
			if (ch&(1 << size))
			{
				cur = cur->_right;
			}
			//�����0ȥ���������������ҡ�
			else
			{
				cur = cur->_left;
			}
			//�����Ҷ�ӽڵ��ˣ����ʱ��������Ҫ�Դ˽��������Ӧ���ַ���
			if (cur->_left == NULL&&cur->_right == NULL)
			{
				fputc(cur->_wight._ch, fin);

				//�����Ҫ���»ص������бȽ�
				cur = root;
				count--;
				//�����е��ַ�ȫ��д�꣬�˳���
				if (count == 0)
				{
					break;
				}
			}
			//����λ���ƣ�size������ά�������λ���㡣
			size--;
			//˵��һ���ַ���ÿһλ���Ѿ�ȡ���ˡ����ʱ�����ǾͿ��Խ�����һ���ַ��ˡ�

			if (size < 0)
			{
				ch = fgetc(fout);
				size = 7;
			}
		}
		//�����йر��ļ���
		fclose(fin);
		fclose(fout);
		fclose(fconfigout);
	}
protected:
	//����һ�еĶ�ȡ��
	bool ReadLine(FILE*& fconfigout, string & str)
	{
		//�������ļ�����һ���ַ�
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
		//while��������������str��¼����һ�г���\n������ַ������ⲿ�����\n��ӡ�
		return true;
	}
protected:

	//�ݹ鴴������������
	void _CreateHuffmanCode(HuffmanTreeNode<CharInfo >* root, string code)
	{
		//Ϊ�գ�����
		if (root == NULL)
		{
			return;
		}
		//Ҷ�ӽڵ�Ϳ����ҵ���Ӧ���ַ������ʱ��������Ĺ��������롣
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