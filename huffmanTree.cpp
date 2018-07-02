#include <iostream>
#include <cmath>
#include <map>
#include <set>
#include <string>
using namespace std;

typedef enum { L, R, M } dir;   //对哈弗曼树进行遍历的方向，M用于访问根节点

class nodetype {
private:
	unsigned freq;
	string str, code;           //对非叶子节点，str表示子树所有叶子构成的字符串，code无意义；对叶子，str仅有一个字符，code表示其编码
	nodetype *left, *right;
public:
	nodetype() {}
	nodetype(unsigned argfreq, char argc) {
		freq = argfreq; str = argc;
	}
	bool operator<(const nodetype& another) const {  //重载比较器，使set自动排序
		return freq < another.freq;
	}
	bool isleaf() {
		return str.length() == 1;
	}
	void print() {
		cout << str << ": " << code << endl;
	}
	void read(string nowstr, dir d, unsigned& totalbitlen) {
		//nowstr表示读到当前节点形成的编码（不能用引用！），d表示当前节点是在上一节点的左边还是右边，最后一个参数用于统计编码长度
		//总体思想是二叉树的先序遍历
		if (d == L) nowstr += '0';
		else if (d == R) nowstr += '1';
		if (isleaf()) {
			code = nowstr;
			totalbitlen += code.length();
			print();
			return;
		}
		left->read(nowstr, L, totalbitlen);
		right->read(nowstr, R, totalbitlen);
	}
	friend nodetype mymerge(nodetype* one, nodetype* another);
};

nodetype mymerge(nodetype* one, nodetype* another) {  //两个子树（叶子）的合并，区分左右
	nodetype res;
	res.freq = one->freq + another->freq;
	res.str = one->str + another->str;
	res.left = one;
	res.right = another;
	return res;
}


int main()
{
	string ipt; cin >> ipt;			//输入一个字符串
	map<char, unsigned> freqs;		
	for (auto i = ipt.begin(); i != ipt.end(); i++) freqs[*i]++;	//统计其中各字符出现频数
	unsigned leafnum = freqs.size();	//记下不同字符的个数，用于计算平均编码长度
	//set会自动根据重载的比较器维持容器的有序状态，multiset允许有重复的排序关键字（即nodetype::freq），为合并建树作准备
	multiset<nodetype> seq;			
	for (auto i = freqs.begin(); i != freqs.end(); i++) {
		seq.emplace(i->second, i->first);
	}
	//合并建树，每次将set前两个抛出来，合并后插入set，合并结束后seq中只剩一个根节点
	for (auto i = seq.begin(); seq.size() != 1; i = seq.begin()) {
		auto j = i; j++;
		//此处必须用new，且不能delete！nodetype temp1 = *i的写法会导致退出循环后node中的string全部丢失
		nodetype *temp1 = new nodetype(*i), *temp2 = new nodetype(*j);
		seq.erase(i); seq.erase(j);
		seq.insert(mymerge(temp1, temp2));
	}
	nodetype root = *seq.begin();
	unsigned totalbitlen = 0;
	root.read(string(""), M, totalbitlen);	//不能写成seq.begin()->read(...)，迭代器不可修改迭代的容器！
	cout << "平均编码长度为：" << totalbitlen / float(leafnum) << endl;
	cout << "而用简单编码方法，每个字符的编码长度为：" << ceil(log2(leafnum)) << endl;
	return 0;
}
