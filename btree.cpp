#include <iostream>
#include <vector>

class Node {
public:
	//int* keys;
	std::vector<int> keys;
	
	//Node** child;
	std::vector<Node*> child;
	
	
	int t;
	int n;
	bool leaf;

public:
	Node(int _t, bool _leaf);

	void insertNonful(int k);
	void splitChild(int i, Node* y);
	void traverse();
	Node* search(int k);
	void remove(int k);

	void removeFromLeaf(int i);
	void removeFromNonLeaf(int i);

	int findKey(int k);
	int pred(int k);
	int suss(int k);
	void fill(int i);
	void marge(int i);
	void borrowFromPrev(int i);
	void borrowFromNext(int i);


	friend class BTree;
};

class BTree
{
public:
	Node* root;
	int t;
	int n;
	bool _empty;

public:
	BTree(int _t)
	{
		this->t = _t;
		root = NULL;
		this->n = 0;
		this->_empty = true;
	}

	void insert(int k);
	void remove(int k);
	int size() const
	{
		return n;
	}
	bool empty() const {
		return _empty;
	}

	void traverse()
	{
		if (root == NULL)
			return;
		root->traverse();
	}
	Node* search(int k)
	{
		return root == NULL ? NULL : root->search(k);
	}
};

Node::Node(int _t, bool _leaf)
{
	this->t = _t;
	this->leaf = _leaf;
	this->n = 0;

	//this->keys = new int[2 * t - 1];
	this->keys.resize(2 * t - 1);

	//this->child = new Node*[2 * t];
	this->child.resize(2 * t);
}

int Node::findKey(int k)
{
	int i = 0;
	while (i < n && keys[i] < k)
		i++;
	return i;
}

int Node::pred(int i)
{
	Node* curr = child[i];
	while (curr->leaf == false)
		curr = curr->child[curr->n];
	return curr->keys[curr->n - 1];
}

int Node::suss(int i)
{
	Node* curr = child[i];
	while (curr->leaf == false)
		curr = curr->child[0];
	return curr->keys[0];
}

void Node::insertNonful(int k)
{
	int i = n - 1;
	if (leaf == true)
	{
		while (i >= 0 && keys[i] > k)
		{
			keys[i + 1] = keys[i];
			i--;
		}
		keys[i + 1] = k;
		n++;
	}
	else {
		while (i >= 0 && keys[i] > k)
			i--;
		if (child[i + 1]->n == 2 * t - 1)
		{
			splitChild(i + 1, child[i + 1]);

			if (keys[i + 1] < k)
				i++;
			child[i + 1]->insertNonful(k);
		}
		else
			child[i + 1]->insertNonful(k);
	}
}

void Node::splitChild(int i, Node* y)
{
	Node* z = new Node(y->t, y->leaf);
	z->n = t - 1;

	for (int j = 0; j < t - 1; j++)
		z->keys[j] = y->keys[t + j];
	if (y->leaf == false)
		for (int j = 0; j < t; j++)
			z->child[j] = y->child[t + j];
	y->n = t - 1;

	for (int j = n - 1; j >= i; j--)
		keys[j + 1] = keys[j];
	keys[i] = y->keys[t - 1];

	for (int j = n; j >= i + 1; j--)
		child[j + 1] = child[j];
	child[i + 1] = z;

	n++;
}


void Node::traverse()
{
	int i;
	for (i = 0; i < n; i++)
	{
		if (leaf == false)
			child[i]->traverse();
		std::cout << " " << keys[i];
	}

	if (leaf == false)
		child[i]->traverse();
}

Node* Node::search(int k)
{
	int i = 0;

	while (i < n && keys[i] < k)
		i++;
	if (i < n && keys[i] == k)
		return this;
	if (leaf == true)
		return NULL;
	return child[i]->search(k);

}


void BTree::insert(int k)
{
	if (root == NULL)
	{
		root = new Node(t, true);
		root->keys[0] = k;
		root->n = 1;
		this->_empty = false;
	}

	else {
		if (root->n == 2 * t - 1)
		{
			Node* s = new Node(t, false);
			s->child[0] = root;

			s->splitChild(0, root);

			int i = 0;
			if (s->keys[0] < k)
				i++;
			s->child[i]->insertNonful(k);

			root = s;
		}
		else
			root->insertNonful(k);
	}
	n++;
}


void Node::marge(int i)
{
	Node* a = child[i];
	Node* b = child[i + 1];

	a->keys[t - 1] = keys[i];

	for (int j = 0; j < b->n; j++)
		a->keys[t + j] = b->keys[j];

	if (a->leaf == false)
		for (int j = 0; j <= b->n; j++)
			a->child[t + j] = b->child[j];
	for (int j = i; j < n; j++)
		keys[i] = keys[i + 1];

	for (int j = i + 2; j < n; j++)
	{
		child[j - 1] = child[j];
	}

	delete b;
	n--;
}

void Node::borrowFromPrev(int i)
{
	Node* a = child[i];
	Node* b = child[i - 1];

	for (int j = a->n - 1; j >= 0; j--)
		a->keys[j + 1] = a->keys[j];
	a->keys[0] = keys[i];
	keys[i] = b->keys[n - 1];

	if (a->leaf == false)
	{
		for (int j = a->n-1; j >= 0; j--)
			a->child[j] = a->child[j + 1];
		a-> child[0] = b->child[n];
	}

	b->n--;
	a->n++;
}

void Node::borrowFromNext(int i)
{
	Node* a = child[i];
	Node* b = child[i + 1];

	a->keys[a->n] = keys[i];
	keys[i] = b->keys[0];


	for (int j = 0; j < b->n - 1; j++)
		b->keys[j] = b->keys[j + 1];

	if (a->leaf == false)
	{
		a->child[n + 1] = b->child[0];
		for (int j = 0; j < b->n; j++)
			b->child[j] = b->child[j + i];
	}

	b->n--;
	a->n++;
}


void Node::fill(int i)
{
	this;
	if (i != 0 && child[i - 1]->n >= t)
		borrowFromPrev(i);
	else if (i != n && child[i + 1]->n >= t)
		borrowFromNext(i);
	else {
		if (i == n)
			i--;
		marge(i);
	}
}

void Node::removeFromLeaf(int i)
{
	for (int j = i; j < n - 1; j++)
		keys[j] = keys[j + 1];
	n--;
}

void Node::removeFromNonLeaf(int i)
{
	int k = keys[i];

	if (child[i]->n >= t)
	{
		int p = pred(i);
		keys[i] = p;
		child[i]->remove(p);
	}
	else if (child[i + 1]->n >= t)
	{
		int s = suss(i+1);
		keys[i] = s;
		child[i + 1]->remove(s);
	}
	else {
		marge(i);
		child[i]->remove(k);
	}
}

void Node::remove(int k)
{
	int idx = findKey(k);

	if (idx < n && keys[idx] == k)
	{
		if (leaf == true)
			removeFromLeaf(idx);
		else
			removeFromNonLeaf(idx);
	}

	else {


		if (leaf == true)
		{
			std::cout << k << "  is not exist" << std::endl;
			return;
		}


		bool flag = (idx == n) ? true : false;

		if (child[idx]->n < t)
			fill(idx);
		if (flag && idx > n)
			child[idx - 1]->remove(k);
		else
			child[idx]->remove(k);
	}

}

void BTree::remove(int k)
{
	if (root == NULL)
	{
		std::cout << "Tree is Empty" << std::endl;
		return;
	}

	root->remove(k);


	if (root->n == 0)
	{
		Node *temp = root;
		if (root->leaf)
		{
			root = NULL;
			this->_empty = true;
		}
		else
			root = root->child[0];
		delete temp;
	}
	n--;
}

int main()
{

	BTree b(3);

	for (int i = 0; i < 100; i++)
		b.insert(i);
	std::cout << "Size of Tree :" << b.size() << std::endl;
	std::cout << "Tree is Empty :" << b.empty() << std::endl;

	std::cout << "Tree Traverse:" << std::endl;
	b.traverse();
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "Deleting :" << std::endl;
	std::cout << std::endl;
	for (int i = 0; i < 100; i++)
	{
		b.remove(i);
		std::cout << '\r';
		b.traverse();
	}

	std::cout << std::endl;
	std::cin.get();

}
