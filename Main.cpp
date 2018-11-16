#include "BTrees.hpp"
#include <iostream>
#include <random>
#include <algorithm>
#include <stdexcept>
#include <sstream>
using namespace std;

template <size_t N, typename T> ostream& operator<<(ostream& o, BTree<N, T> const& v);

template <size_t NodeSize> void testBTreeWithNodeSize() {
	(cout << "Node size " << NodeSize).flush();
	BTree<NodeSize> b(400L);
	long input[1024];
	size_t i{};
	for(auto& it : input) {
		if(i % 25 == 0)
			i++;
		it = i++;
	}
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(begin(input), end(input), g);
	for(auto& it : input) {
		b.insert(it);
		if(b.count(it) != 1) {
			stringstream error;
			error << it << " got lost in tree: " << b << endl;
			throw runtime_error(error.str());
		}
	}
	for(size_t shouldNotBePresent = 0; shouldNotBePresent < 1024; shouldNotBePresent += 25) {
		if(shouldNotBePresent != 400 && b.count(shouldNotBePresent)) {
			stringstream error;
			error << shouldNotBePresent << " somehow ended up in the tree: " << b << endl;
			throw runtime_error(error.str());
		}
	}
	cout << ": success" << endl;
}

int main() {
	testBTreeWithNodeSize<4>();
	testBTreeWithNodeSize<5>();
	testBTreeWithNodeSize<6>();
	return 0;
}
