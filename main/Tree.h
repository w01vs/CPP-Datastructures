#ifndef TREE_H
#define	TREE_H

#include <string>
#include "HashMap.h"

template <typename T>
class Tree
{
	static_assert(std::is_default_constructible_v<T>, "Attempted to make a Tree with a type that does not implement a default constructor");
	//static_assert(std::is_copy_constructible_v<T>, "Attempted to make a Tree with a type that does not implement a copy constructor");
	static_assert(!std::is_pointer_v<T>, "Attempted to make a Tree with a raw pointer type");

	struct Node
	{
		T data;
		std::string name;
		List<Node> children; // What structure do I use?
	};
public:
	Tree() : count_(0), root(Node(T(), "", List<Node>())) {}
	explicit Tree(T root_data) : count_(1), root(Node(root_data, "", List<Node>())) {}
	explicit Tree(T root_data, std::string root_name) : count_(1), root(Node(root_data, root_name, List<Node>())) {}
	explicit Tree(T root_data, std::string&& root_name) : count_(1), root(Node(root_data, std::move(root_name), List<Node>())) {}

	// no custom copy/move assignments/operators needed

	// the rest of the tree stuff

private:
	size_t count_;
	Node root;
};

#endif // TREE_H