#include <iostream>
#include <conio.h>
#include <ctime>
#include <array>
#define SZ 8
using namespace std;

array<unsigned, SZ> arr;

unsigned random(unsigned low, unsigned high) {
	return low + rand() % (high - low + 1);
}

void init() {
	srand(time(0));
	for (unsigned i = 0; i < SZ; i++)
		arr[i] = rand();
}

class BST_node {
private:
	unsigned num;
	BST_node *left, *right, *parent;
public:
	BST_node() {}
	BST_node(unsigned n, BST_node* par = NULL) : num(n), left(NULL), right(NULL), parent(par) {}
	void grow(unsigned n) {
		if (n < num) left = new BST_node(n, this);
		else right = new BST_node(n, this);
	}
	BST_node* search(unsigned n) {
		if (n < num) {
			if (left) return left->search(n);
			else return this;
		}
		else {
			if (n == num || right == NULL) return this;
			else return right->search(n);
		}
	}
	void inorder_print() {
		if (left) left->inorder_print();
		cout << num << ' ';
		if (right) right->inorder_print();
	}
	bool operator==(const unsigned& n) const {
		return num == n;
	}
	bool isleaf() const {
		return (left == NULL && right == NULL);
	}
	void remove();
};

BST_node* root;

void BST_node::remove() {
	if (right) {
		BST_node* posit = right;
		while (posit->left) posit = posit->left;
		num = posit->num;
		posit->remove();
	}
	else {
		if (left) {
			if (parent == NULL) root = left;
			else {
				if (num < parent->num) parent->left = left;
				else parent->right = left;	
			}
			left->parent = parent;
			delete this;
		}
		else {
			if (num < parent->num) parent->left = NULL;
			else parent->right = NULL;
			delete this;
		}
	}
}

int main()
{
	init(); root = new BST_node(arr[0]);
	for (unsigned i = 1; i < SZ; i++)
		root->search(arr[i])->grow(arr[i]);
	root->inorder_print();
	do {
		cout << endl << "请输入一个数（输入32768退出程序）：";
		unsigned ipt; cin >> ipt;
		if (ipt == 32768) break;
		BST_node* posit = root->search(ipt);
		if (*posit == ipt) {
			cout << "找到了！请问是否要删除它？按Y或y删除，按其他键返回";
			char t = _getche(); cout << endl;
			if (t == 'Y' || t == 'y') {
				if (root->isleaf()) cout << "这是最后一个节点了！不能再删了！";
				else {
					posit->remove();
					cout << "删除成功！" << endl;
					root->inorder_print();
				}
			}
		}
		else {
			cout << "没找到！请问是否要添加它？按Y或y添加，按其他键返回";
			char t = _getche(); cout << endl;
			if (t == 'Y' || t == 'y') {
				posit->grow(ipt);
				cout << "添加成功！" << endl;
				root->inorder_print();
			}		
		}
	} while (true);
	delete root;
	return 0;
}
