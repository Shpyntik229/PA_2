#pragma once
#include <memory>
#include <optional>

template<typename K, typename V>
class RBTree
{
private:
	struct Node
	{
		std::unique_ptr<K> pKey;
		std::unique_ptr<V> pValue;
		bool isRed;

		Node* parent;
		using NodePtr = std::unique_ptr<Node>;
		NodePtr left = nullptr, right = nullptr;

		Node(const K& key, const V& value, bool color, Node* parent) : pKey(std::make_unique<K>(key)), pValue(std::make_unique<V>(value)), isRed(color), parent(parent) {}

		Node* getBrother()
		{
			if (isLeftChild())
			{
				return parent->right.get();
			}
			return parent->left.get();
		}

		bool isBrotherRed()
		{
			if (Node* brother = getBrother())
			{
				return brother->isRed;
			}
			return false;
		}

		bool isLeftChild()
		{
			if (!parent) throw 1;
			if (parent->left.get() == this)
			{
				return true;
			}
			return false;
		}

		bool isRightChild()
		{
			return !isLeftChild();
		}

		void swapColorsWithChildrens()
		{
			if (!left || !right) throw 2;
			left->isRed = isRed;
			right->isRed = isRed;
			isRed = !isRed;
		}

		void changeColors()
		{
			if (!left || !right) throw 2;
			isRed = BLACK;
			left->isRed = RED;
			right->isRed = RED;
		}

		bool hasBlackChilds()
		{
			if (left && left->isRed) return false;
			if (right && right->isRed) return false;
			return true;
		}

		/*void rightTurn()
		{
			Node* grandparent = parent->parent;

			NodePtr temp = std::move(parent->left);
			if (parent->isRightChild())
			{
				swap(grandparent->right, temp);
			}
			else
			{
				swap(grandparent->left, temp);
			}
			parent->left = std::move(right);
			right = std::move(temp);

			parent->parent = this;
			parent->left->parent = parent;
			parent = grandparent;
		}
		void leftTurn()
		{
			Node* grandparent = parent->parent;

			NodePtr temp = std::move(parent->right);
			if (parent->isRightChild())
			{
				swap(grandparent->right, temp);
			}
			else
			{
				swap(grandparent->left, temp);
			}
			parent->right = std::move(left);
			left = std::move(temp);

			parent->parent = this;
			parent->right->parent = parent;
			parent = grandparent;
		}*/
	};
	using NodePtr = std::unique_ptr<Node>;

private:
	Node* createNode(const K& key, const V& value)
	{
		NodePtr* currNode = &m_basicNode;
		Node* parent = nullptr;
		while (*currNode)
		{
			if (*(*currNode)->pKey == key) return nullptr;
			parent = currNode->get();
			if (key < *(*currNode)->pKey) currNode = &(*currNode)->left;
			else currNode = &(*currNode)->right;
		}
		if (!(*currNode)) {
			bool color = RED;
			if (currNode == &m_basicNode) color = BLACK;

			*currNode = std::make_unique<Node>( std::ref(key), std::ref(value), color, parent );
		}
		return currNode->get();
	}

	void rightTurn(Node* node)
	{
		Node* parent = node->parent;
		Node* grandparent = parent->parent;

		NodePtr temp = std::move(parent->left);
		if (grandparent)
		{
			if (parent->isRightChild())
			{
				swap(grandparent->right, temp);
			}
			else
			{
				swap(grandparent->left, temp);
			}
		}
		else
		{
			swap(m_basicNode, temp);
		}
		parent->left = std::move(node->right);
		node->right = std::move(temp);

		parent->parent = node;
		if (parent->left) parent->left->parent = parent;
		node->parent = grandparent;
	}

	void leftTurn(Node* node)
	{
		Node* parent = node->parent;
		Node* grandparent = parent->parent;

		NodePtr temp = std::move(parent->right);
		if (grandparent)
		{
			if (parent->isRightChild())
			{
				swap(grandparent->right, temp);
			}
			else
			{
				swap(grandparent->left, temp);
			}
		}
		else
		{
			swap(m_basicNode, temp);
		}
		parent->right = std::move(node->left);
		node->left = std::move(temp);

		parent->parent = node;
		if (parent->right) parent->right->parent = parent;
		node->parent = grandparent;
	}

	void turnUp(Node* node)
	{
		if (node->isLeftChild())
		{
			rightTurn(node);
		}
		else
		{
			leftTurn(node);
		}
	}

	void balance(Node* node)
	{
		if (Node* parent = node->parent)
		{
			if (parent->isRed)
			{
				if (parent->isBrotherRed())
				{
					parent->parent->swapColorsWithChildrens();
					balance(parent->parent);
				}
				else
				{
					bool isLeft = node->isLeftChild();
					if (isLeft != parent->isLeftChild()) 
					{
						if (isLeft)
						{
							rightTurn(node);
						}
						else
						{
							leftTurn(node);
						}
						std::swap(node, parent);
					}
					else
					{
						if (isLeft)
						{
							rightTurn(parent);
						}
						else
						{
							leftTurn(parent);
						}
						parent->changeColors();
					}
				}
			}
		}
		else
		{
			node->isRed = false;
		}
	}

	std::optional<Node*> find(const K& key)
	{
		lastCheckCount = 0;
		Node* curr = m_basicNode.get();
		while (curr && *curr->pKey != key)
		{
			if (key < *curr->pKey)
			{
				curr = curr->left.get();
			}
			else
			{
				curr = curr->right.get();
			}
			++lastCheckCount;
		}
		if (!curr) return std::nullopt;
		return curr;
	}

	std::optional<const Node*> find(const K& key) const
	{
		lastCheckCount = 0;
		const Node* curr = m_basicNode.get();
		while (curr && *curr->pKey != key)
		{
			if (key < *curr->pKey)
			{
				curr = curr->left.get();
			}
			else
			{
				curr = curr->right.get();
			}
			++lastCheckCount;
		}
		if (!curr) return std::nullopt;
		return curr;
	}

	Node* findFarRight(Node* node)
	{
		if (!node) throw 4;
		while (node->right)
		{
			node = node->right.get();
		}
		return node;
	}

	Node* findFarLeft(Node* node)
	{
		if (!node) throw 4;
		while (node->left)
		{
			node = node->left.get();
		}
		return node;
	}

	void swapValues(Node* first, Node* second)
	{
		swap(first->pKey, second->pKey);
		swap(first->pValue, second->pValue);
	}

	void deleteNode(Node* node)
	{
		if (node->right && node->left)
		{
			Node* farNode = findFarRight(node);
			swapValues(node, farNode);
			node = farNode;
		}
		if (node->right)
		{
			swapValues(node, node->right.get());
			node = node->right.get();
		}
		if (node->left)
		{
			swapValues(node, node->left.get());
			node = node->left.get();
		}
		deleteNodeWithoutChilds(node);
	}

	void balanceBeforeDelete(Node* node)
	{
		if (!node->isRed)
		{
			Node* parent = node->parent;
			if (parent)
			{
				Node* brother = node->getBrother(); /*brother can be null????*/
				if (parent->isRed)
				{
					if (brother->hasBlackChilds())
					{
						parent->isRed = BLACK;
						brother->isRed = RED;
					}
					else
					{
						if (brother->isLeftChild())
						{
							rightTurn(brother);
						}
						else
						{
							leftTurn(brother);
						}
						brother->swapColorsWithChildrens();
					}
				}
				else
				{
					Node* secChild;
					bool isBrotherLeft = brother->isLeftChild();
					if (isBrotherLeft)
					{
						secChild = brother->right.get();
					}
					else
					{
						secChild = brother->left.get();
					}
					if (brother->isRed)
					{
						if (secChild->hasBlackChilds())
						{
							if (isBrotherLeft)
							{
								rightTurn(brother);
							}
							else
							{
								leftTurn(brother);
							}
							brother->isRed = BLACK;
							secChild->isRed = RED;
						}
						else
						{
							turnUp(secChild);
							turnUp(secChild);
							if (secChild->isLeftChild())
							{
								secChild->right->isRed = BLACK;
							}
							else
							{
								secChild->left->isRed = BLACK;
							}
						}
					}
					else
					{
						if (brother->hasBlackChilds())
						{
							brother->isRed = RED;
							balanceBeforeDelete(parent);
						}
						else
						{
							turnUp(secChild);
							turnUp(secChild);
							secChild->isRed = BLACK;
						}
					}
				}
			}
		}
	}

	void deleteNodeWithoutChilds(Node* node)
	{
		if (node->right || node->left) throw 5;
		balanceBeforeDelete(node);
		destroyNode(node);
	}

	void destroyNode(Node* node)
	{
		if (!node) throw 4;
		if (node->parent)
		{
			if (node->isLeftChild())
			{
				node->parent->left = nullptr;
			}
			else
			{
				node->parent->right = nullptr;
			}
		}
		else
		{
			m_basicNode = nullptr;
		}
	}

public:
	bool add(const K& key, const V& value)
	{
		Node* newNode = createNode(key, value);
		if (!newNode) return false;
		balance(newNode);
		return true;
	}
	const V& operator[](const K& key) const
	{
		auto opt = find(key);
		if (!opt.has_value()) throw 3;
		return *opt.value()->pValue;
	}
	V& operator[](const K& key)
	{
		auto opt = find(key);
		if (!opt.has_value()) throw 3;
		return *opt.value()->pValue;
	}
	bool remove(const K& key)
	{
		auto optNode = find(key);
		if (!optNode.has_value()) return false;
		deleteNode(optNode.value());
		return true;
	}
	int getLastCheckCount()
	{
		return lastCheckCount;
	}

private:
	NodePtr m_basicNode = nullptr;
	int lastCheckCount = 0;

private:
	static constexpr bool RED = true;
	static constexpr bool BLACK = false;
};