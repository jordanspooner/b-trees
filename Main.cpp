#include "BTrees.hpp"
#include <iostream>
#include <random>
#include <algorithm>
#include <stdexcept>
#include <sstream>
using namespace std;

template <size_t N, typename T> ostream& operator<<(ostream& o, BTree<N, T> const& v);

template <size_t NodeSize> void testBTreeWithNodeSize() {
	BTree<NodeSize> b(34);
	b.insert(13);
	b.insert(15);
	b.insert(11);
	b.insert(22);
	b.insert(1);
	b.insert(38);
	b.insert(28);
	b.insert(6);
	b.insert(9);
	b.insert(32);
	b.insert(8);
	b.insert(40);
	b.insert(37);
	b.insert(3);
	b.insert(16);
	b.insert(49);
	b.insert(44);
	b.insert(39);
	b.insert(19);
	b.insert(50);
	b.insert(17);
	b.insert(36);
	b.insert(20);
	b.insert(30);
	b.insert(4);
	b.insert(35);
	b.insert(48);
	b.insert(12);
	b.insert(2);
	b.insert(14);
	b.insert(7);
	b.insert(46);
	b.insert(27);
	b.insert(47);
	b.insert(23);
	b.insert(10);
	b.insert(43);
	b.insert(42);
	b.insert(29);
	b.insert(24);
	b.insert(31);
	b.insert(21);
	b.insert(33);
	b.insert(26);
	b.insert(25);
	b.insert(41);
	b.insert(5);
	b.insert(18);
	b.insert(45);
	cout << "*** NodeSize: " << NodeSize << " ***" << endl;
	for (long i = 1; i < 50; i++) {
		if (b.count(i) != 1) cerr << "Didn't find " << i << " but it should be there!" << endl;
	}
	if (b.count(0) != 0) cerr << "Found " << 0 << " but it shouldn't be there!" << endl;
	if (b.count(51) != 0) cerr << "Found " << 51 << " but it shouldn't be there!" << endl;
	if (NodeSize == 4) {
		cout << "It should look like:    [[[[1], 2, [3, 4, 5], 6, [7, 8], 9, [10, 11, 12]], 13, [[14, 15], 16, [17, 18], 19, [20, 21]]], 22, [[[23], 24, [25, 26], 27, [28], 29, [30, 31]], 32, [[33], 34, [35, 36], 37, [38, 39]], 40, [[41, 42, 43], 44, [45, 46, 47], 48, [49, 50]]]]" << endl;
	}
	if (NodeSize == 5) {
		cout << "It should look like:    [[[1, 2, 3], 4, [5, 6, 7, 8], 9, [10, 11], 12, [13, 14]], 15, [[16, 17, 18], 19, [20, 21], 22, [23, 24, 25, 26], 27, [28, 29, 30, 31]], 32, [[33, 34], 35, [36, 37], 38, [39, 40]], 41, [[42, 43], 44, [45, 46, 47], 48, [49, 50]]]" << endl;
	}
	if (NodeSize == 6) {
		cout << "It should look like:    [[[1, 2, 3, 4, 5], 6, [7, 8], 9, [10, 11, 12], 13, [14, 15, 16]], 17, [[18, 19, 20, 21], 22, [23, 24, 25, 26, 27], 28, [29, 30, 31]], 32, [[33, 34, 35, 36], 37, [38, 39], 40, [41, 42], 43, [44, 45, 46], 47, [48, 49, 50]]]" << endl;
	}
	cout << "Your tree looks like:   " << b << endl;
}

int main() {
	testBTreeWithNodeSize<4>();
	testBTreeWithNodeSize<5>();
	testBTreeWithNodeSize<6>();
	return 0;
}
