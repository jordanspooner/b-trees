#include <cassert>
#include <cstdlib>
#include <utility>
#include <iostream>
#include <optional>
#include "BTrees.hpp"
using namespace std;

/**
 * A Node in the Tree,
 *
 * NodeSize is the out-degree (the number of children),
 *
 * T is the type of the held value
 */
template <size_t NodeSize, typename T> struct BTreeNode {
	/**
	 * An element in the node consisting of a pivot and the subtree to its left
	 */
	struct NodeElement {
		/**
		 * The node to the left of the pivot
		 */
		BTreeNode<NodeSize, T>* childToLeft{};
		/**
		 * The pivot, we're using the optional template to encode pivots slots that don't contain a
		 * value
		 */
		optional<T> pivot{};
	};

	/**
	 * A struct to (temporarily) hold newly created splits
	 */
	struct NewSplit {
		/**
		 * the newly introduced pivot, this needs merging into a node
		 */
		T newPivot;
		/**
		 * the newly create subtree to the right of the pivot
		 */
		BTreeNode<NodeSize, T>* childToTheRightOfPivot;
	};

	/**
	 * since the out-degree is NodeSize, we have NodeSize-1 pivots
	 */
	NodeElement pivots[NodeSize - 1]{};
	/**
	 * the child to the very right of the node, it is not associated/paired with a pivot
	 */
	BTreeNode<NodeSize, T>* rightMost = nullptr;

	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////// There is no need for you to change anything above this line//////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////

	/**
	 * Work out whether a pivot exists at an index.
	 * @param index The index to check.
	 * @return Whether a pivot is present.
	 */
	 bool pivotIsPresent(size_t index) {
		return index < NodeSize - 1 && pivot(index).has_value();
	}

	/**
	 * Work out value at a pivot.
	 * @param index The index, between `0` and `NodeSize - 2` (inclusive), to check.
	 * @return The value at the pivot (pivot must be present).
	 */
	optional<T>& pivot(size_t index) {
		assert(index >= 0 && index < NodeSize - 1);
		return pivots[index].pivot;
	}

	/**
	 * Work out the child at an index.
	 * @param index The index, between `0` and `NodeSize - 1` (inclusive), to check.
	 * @return The child at this index (should be `nullptr` if no such child exists).
	 */
	BTreeNode<NodeSize, T>*& child(size_t index) {
	 	assert(index >= 0 && index < NodeSize);
	 	if (index == NodeSize - 1) return rightMost;
		return pivots[index].childToLeft;
	}

	/**
	 * Clear the node at an index and the child to the right of that index.
	 * The pivot value is reset and the child to its right is set to NULL.
	 * @param index The index, between 0 and `NodeSize - 1` (inclusive), to clear.
	 */
	void clear(size_t index) {
		pivot(index).reset();
		child(index + 1) = nullptr;
	}

	/**
	 * Work out if this node needs to be split before inserting a new key.
	 * @return Whether this BTreeNode is full (any addition will require a split).
	 */
	bool isFull() {
		return pivotIsPresent(NodeSize - 2);
	}

	/**
	 * Work out whether this node is a leaf.
	 * @return Whether this BTreeNode is a leaf (has any children).
	 */
	bool isLeaf() {
		return child(0) == nullptr;
	}

	/**
	 * Copy all NodeElements from the index @param index to `NodeSize - 2` (inclusive) to a new node.
	 * Also clears the copied values from the original node (this).
	 * @param index The node from which elements should be copied from the end.
	 * @return A new node, with all copied elements at the beginning.
	 */
	BTreeNode<NodeSize, T>* moveToNewNode(size_t index) {
		auto* newNode = new BTreeNode<NodeSize, T>();
		// Copy to new node.
		for (size_t i = index; i < NodeSize - 1; i++) {
			newNode->pivots[i - index] = pivots[i];
		}
		newNode->child(NodeSize - 1 - index) = child(NodeSize - 1);
		// Clear from old node.
		for (size_t i = index; i < NodeSize - 1; i++) clear(i);
		return newNode;
	}

	NewSplit insertIntoNonFullNode(size_t index, T valueToInsert) {
		child(NodeSize - 1) = child(NodeSize - 2);
		for (size_t i = NodeSize - 2; i > index; i--) {
			pivots[i] = pivots[i - 1];
		}
		pivot(index) = valueToInsert;
		return {};
	}

	NewSplit insertIntoNonFullNode(size_t index, NewSplit newSplit) {
		insertIntoNonFullNode(index, newSplit.newPivot);
		child(index + 1) = newSplit.childToTheRightOfPivot;
		return {};
	}

	NewSplit insertIntoFullNode(size_t index, T valueToInsert) {
		T newPivot;
		BTreeNode<NodeSize, T>* newRightChild;

		size_t middle = (NodeSize - 1) / 2;
		if (index < middle) {
			// Case 1: inserted value is to the left of the median.
			newPivot = pivot(middle - 1).value();
			newRightChild = moveToNewNode(middle);
			clear(middle - 1);
			insertIntoNonFullNode(index, valueToInsert);
		} else if (index == middle) {
			// Case 2: inserted value is the median.
			newPivot = valueToInsert;
			newRightChild = moveToNewNode(middle);
		} else {
			// Case 3: inserted value is to the right of the median.
			newPivot = pivot(middle).value();
			newRightChild = moveToNewNode(middle + 1);
			clear(middle);
			newRightChild->insertIntoNonFullNode(index - middle - 1, valueToInsert);
		}

		return {newPivot, newRightChild};
	}

	NewSplit insertIntoFullNode(size_t index, NewSplit split) {
		T newPivot;
		BTreeNode<NodeSize, T>* newRightChild;

		size_t middle = (NodeSize - 1) / 2;
		if (index < middle) {
			// Case 1: inserted value is to the left of the median.
			newPivot = pivot(middle - 1).value();
			newRightChild = moveToNewNode(middle);
			pivot(middle - 1).reset();
			child(middle) = nullptr;
			insertIntoNonFullNode(index, split);
		} else if (index == middle) {
			// Case 2: inserted value is the median.
			newPivot = split.newPivot;
			newRightChild = moveToNewNode(middle);
			newRightChild->child(0) = split.childToTheRightOfPivot;
		} else {
			// Case 3: inserted value is to the right of the median.
			newPivot = pivot(middle).value();
			newRightChild = moveToNewNode(middle + 1);
			pivot(middle).reset();
			child(middle + 1) = nullptr;
			newRightChild->insertIntoNonFullNode(index - middle - 1, split);
		}

		return {newPivot, newRightChild};
	}

	/**
	 *s
	 * @param valueToInsert
	 * @return
	 */
	NewSplit insert(T valueToInsert) {
		size_t index = 0;
		while (pivotIsPresent(index) && valueToInsert > pivot(index).value())
			index++;

		if (isLeaf()) {
			if (!isFull()) {
				return insertIntoNonFullNode(index, valueToInsert);
			}
			return insertIntoFullNode(index, valueToInsert);
		}

		NewSplit newSplit = child(index)->insert(valueToInsert);
		if (newSplit.childToTheRightOfPivot == nullptr) return {};
		if (!isFull()) {
			return insertIntoNonFullNode(index, newSplit);
		}
		return insertIntoFullNode(index, newSplit);
	}

	/**
	 * Count the number of occurrences of a value.
	 * We assume uniqueness of values, thus this function returns either 0 of 1.
	 * @param valueToFind The pivot/key value to check for.
	 * @return 1 if present, 0 otherwise.
	 */
	size_t count(T valueToFind) {
		size_t index = 0;
		while (pivotIsPresent(index) && valueToFind > pivot(index).value())
			index++;
		if (pivotIsPresent(index) && valueToFind == pivot(index).value())
			return 1;
		else if (isLeaf())
			return 0;
		else
			return child(index)->count(valueToFind);
	}
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////// There is no need for you to change anything below this line//////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template <size_t NodeSize, typename T> void BTree<NodeSize, T>::insert(T v) {
	auto newNode = root->insert(v);
	auto oldRoot = root;
	if(newNode.childToTheRightOfPivot) {
		root = new BTreeNode<NodeSize, T>{};
		root->pivots[0].pivot = newNode.newPivot;
		root->pivots[0].childToLeft = oldRoot;
		root->pivots[1].childToLeft = newNode.childToTheRightOfPivot;
	}
}

template <size_t NodeSize, typename T>
BTree<NodeSize, T>::BTree(T v)
		: root(new BTreeNode<NodeSize, T>) {
	root->pivots[0].pivot = v;
}

template <size_t NodeSize, typename T> size_t BTree<NodeSize, T>::count(T v) {
	return root->count(v);
}

template <size_t NodeSize, typename T>
ostream& operator<<(ostream& o, typename BTreeNode<NodeSize, T>::NodeElement const& v) {
	return v.childToLeft ? (o << *(v.childToLeft) << ", " << *v.pivot) : (o << *v.pivot);
}

template <size_t NodeSize, typename T> ostream& operator<<(ostream& o, BTreeNode<NodeSize, T> const& v) {
	o << "[";
	operator<<<NodeSize, T>(o, v.pivots[0]);
	for(auto it = next(begin(v.pivots)); it != end(v.pivots); ++it) {
		if(it->pivot)
			operator<<<NodeSize, T>(o << ", ", *it);
		else if(it->childToLeft)
			o << ", " << *it->childToLeft;
	}
	return (v.rightMost ? (o << ", " << *v.rightMost) : o) << "]";
}

template <size_t NodeSize, typename T>
ostream& operator<<(ostream& o, BTree<NodeSize, T> const& v) {
	return o << *v.root << endl;
}

template class BTree<3>;
template ostream& operator<<(ostream& o, BTree<3, long> const& v);
template class BTree<4>;
template ostream& operator<<(ostream& o, BTree<4, long> const& v);
template class BTree<5>;
template ostream& operator<<(ostream& o, BTree<5, long> const& v);
template class BTree<6>;
template ostream& operator<<(ostream& o, BTree<6, long> const& v);
