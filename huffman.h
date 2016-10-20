#define _CRT_SECURE_NO_WARNINGS 1


#pragma once
#include<iostream>
#include<cstdlib>
#include"heap.h"
using namespace std;


template<typename T>
struct HuffmanTreeNode
{
	typedef HuffmanTreeNode<T> Node;
	HuffmanTreeNode(const T&  weight =0)
		:_left(NULL)
		, _right(NULL)
		, _wight(weight)
	{}

	Node *_left;
	Node *_right;
	Node *_parent;
	T _wight;
};


template<typename T>
class HuffmanTree
{
	typedef HuffmanTreeNode<T> Node;

public:
	HuffmanTree()=default;
	HuffmanTree( T* arr, int size,const T& Invaild)
	{
		assert(arr);
		_CreateHuffmanTree(arr, size, Invaild);


	}
	
protected:
	void _CreateHuffmanTree(  T* arr, int size,const T& Invaild)
	{
		assert(arr);
		
		struct NodeCompare
		{
			bool operator ()(const Node* a,const Node *b)
			{
				return a->_wight < b->_wight;
			}
		};
		Heap<Node*, NodeCompare> h;

		for (int i = 0; i < size; i++)
		{
			if (arr[i]!=Invaild)
			h.Push(new Node(arr[i]));
		}

		while (h.Size() > 1)
		{
			Node *left = h.Top();
			h.Pop();
			Node *right = h.Top();
			h.Pop();

			Node* parent=new Node(left->_wight + right->_wight);
			parent->_left = left;
			parent->_right = right;
			left->_parent = parent;
			right->_parent = parent;

			h.Push(parent);
		}
		_root = h.Top();
		h.Pop();
	}
public:
	Node* Getroot()
	{
		return _root;
	}
protected:
	Node *_root;
};


