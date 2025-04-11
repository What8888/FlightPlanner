#ifndef BSTSET_INCLUDED
#define BSTSET_INCLUDED

#include <iostream>
#include "fm.h"

template <typename T> 
class BSTSet 
{
private:
	struct Node {
		T value;
		Node* left;
		Node* right;
		Node(const T& value) : value(value), left(nullptr), right(nullptr) {}
	};

	Node* root;
	

	void clear(Node* node) {
		if (!node) {
			return;
		}

		clear(node->left);
		clear(node->right);
		delete node;
	}

	Node* insertHelper(Node* node, const T& value) {
		if (!node) {
			return new Node(value);
		}

		if (value < node->value) {
			node->left = insertHelper(node->left, value);
		}
		else if (value > node->value) {
			node->right = insertHelper(node->right, value);
		}
		else {
			node->value = value;
		}
		return node;
	}

	Node* findHelper(Node* node, const T& value) const {
		if (!node || node->value == value) {
			return node;
		}

		if (value < node->value) {
			return findHelper(node->left, value);
		}

		return findHelper(node->right, value);
	}

public:
	BSTSet() : root(nullptr) {}

	~BSTSet() { clear(root); }

	void insert(const T& value) {
		root = insertHelper(root, value);
	}
	
	class SetIterator
	{
	private:
		struct StackNode
		{
			Node* tree_node;
			StackNode* next;
			StackNode(Node* treeN, StackNode* nxt) : tree_node(treeN), next(nxt) {}
		};

		StackNode* stack_top;
		T minValue;

		void push_path(Node* node) {

			while (node) {

				if (node->value >= minValue) {
					stack_top = new StackNode(node, stack_top);
					node = node->left;
				}
				else {
					node = node->right;
				}

			}
		}


	public:
		SetIterator(Node* node) : stack_top(nullptr), minValue(T())
		{
			if (node) 
			{
				stack_top = new StackNode(node, nullptr);
			}
		}

		SetIterator(Node* root, const T& startValue) : stack_top(nullptr), minValue(startValue)
		{
			push_path(root);
		}

		~SetIterator() {
			while (stack_top) {
				StackNode* temp = stack_top;
				stack_top = stack_top->next;
				delete temp;
			}
		}

		const T* get_and_advance() {
			if (!stack_top) {
				return nullptr;
			}

			Node* node = stack_top->tree_node;
			StackNode* popped = stack_top;
			stack_top = stack_top->next;
			delete popped;

			const T* result = &node->value;
			push_path(node->right);
			return result;

		}

	};

	typename BSTSet<T>::SetIterator find(const T& value) const {

	
		return SetIterator(findHelper(root, value));
	}

	typename BSTSet<T>::SetIterator find_first_not_smaller(const T& value) const {
		return SetIterator(root, value);
	}

};


#endif