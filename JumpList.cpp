#include "JumpList.h"

#include <iomanip>
#include <iostream>
#include <stdexcept>

Node::Node(const string& s, Node* next, Node* jump, int gap){
	data_= s;  //initialise Node with parameters
	next_ = next;
    jump_ = jump;
    gap_ = gap;

}

Node::~Node() {
	// IMPLEMENT ME

}

JumpList::JumpList() {
	head_ = nullptr; //initialise empty list
}

JumpList::JumpList(int size, const string* arr) {
	// DO NOT CHANGE
	// You do not need to understand the code here to complete the assignment
	const string s[] = {"a", "b", "blah", "C", "cat", "d", "etc", "ooo...", "x", "zzz"};
	const string* sp = (arr == nullptr) ? s : arr;

	Node** npp = new Node*[size];
	npp[size-1] = new Node(sp[size-1], nullptr, nullptr, 0);
	for(int i = size-2; i >= 0; i--)
		npp[i] = new Node(sp[i], npp[i+1], nullptr, 0);

	if (arr == nullptr) {

		if (size==1) npp[0]->gap_ = 1;
		else if (size==3) npp[0]->gap_ = 3;
		else if (size==10) {
			npp[0]->jump_ = npp[5];
			npp[5]->jump_ = npp[8];
			npp[0]->gap_ = 5;
			npp[5]->gap_ = 3;
			npp[8]->gap_ = 2;
		}
		else throw std::invalid_argument("size must be 1, 3 or 10");

	}
	else {

		// jump node every MAX_GAP_SIZE
		int i;
		for(i=0; i < size-MAX_GAP_SIZE; i += MAX_GAP_SIZE) {
			npp[i]->jump_ = npp[i+MAX_GAP_SIZE];
			npp[i]->gap_ = MAX_GAP_SIZE;
		}
		npp[i]->gap_ = size % MAX_GAP_SIZE;
		if (npp[i]->gap_ == 0) npp[i]->gap_ = MAX_GAP_SIZE; // 0 need to become 5!

	}

	head_ = npp[0];
	delete [] npp; // note that the local array npp gets destroyed here but the nodes stay alive!

}

JumpList::~JumpList() {
	while(head_ != nullptr){ //loop through nodes and deletes
		if (head_ == nullptr) return;
		Node* toBeDeleted = head_;
		head_ = head_->next_;
		delete toBeDeleted;
	}
}

int JumpList::size() const {
	int counter =0; //loops through nodes with a counter to return sizw
	Node * currentNode = head_;
    while (currentNode != nullptr) {
        counter++;
        currentNode = currentNode->next_;
    }
	return counter;
}

bool JumpList::find(const string& s) const {
	// DONE FOR YOU, DO NOT CHANGE
	if (head_ == nullptr) return false;

	// moving along the fast lane
	Node* tmp = head_;
	while(tmp->jump_ != nullptr && tmp->jump_->data_ < s)
		tmp = tmp->jump_;

	// tmp now points to the jump node at the start of the segment where s could be

	// now slow lane
	while(tmp != nullptr) {
		if (tmp->data_ == s) return true; // match
		else if (tmp->data_ > s) return false; // went past without finding s
		else tmp = tmp->next_;
	}
	return false; // end of list
}

string JumpList::get(int i) const {
	string output = "";
	if (i < 0 || i >= size()) {//bounds
		return output;
	}
	int index=0;
	Node* tmp = head_;
	while(tmp->jump_ != nullptr&& tmp->gap_ + index <= i) {//fast lane adds gap to index
		index +=tmp->gap_;
		tmp = tmp->jump_;
	}
	while(tmp != nullptr && index < i) {//slow lane
		tmp = tmp->next_;
		index++;
	}
	if(tmp != nullptr &&index==i) {//retrieve node data when index is reawched
		output=tmp->data_;
	}

	return output;
}

string JumpList::print() const {
	if (head_ == nullptr) return "";

	string nodeStr;
	string jmpNodeStr;
	string jmpNodeGapStr;

	Node* tmp = head_;

	while (tmp != nullptr) {//loop through list of nodes
		nodeStr += tmp->data_ + " "; // adds
		if (tmp->gap_ != 0 || tmp->jump_ != nullptr) {//cgeck if its a jump node and adds data+gap to strings
			jmpNodeStr += tmp->data_ + " ";
			jmpNodeGapStr += std::to_string(tmp->gap_) + " ";
		}
		tmp = tmp->next_;
	}
	return nodeStr + "\n" + jmpNodeStr + "\n" + jmpNodeGapStr;

}

string JumpList::prettyPrint() const {//im sorry to whoever has to follow this :/
    if (head_ == nullptr) return "";

    string nodeStr;
    string jmpNodeStr;
    string jmpNodeGapStr;

    Node* tmp = head_;
    const int distanceBetweenNodes = 5;  // size of" --> "
    const int space = 3;   // accounts for space before and after the arrow head (">")

    int nodeSize = 0;
    int nodeGap = 0;

    while (tmp != nullptr) {
        if (tmp->gap_ == 0 && tmp->next_ != nullptr) {//adds size of all non jumpnode strings
            nodeSize += tmp->data_.size();
        }
        if (tmp == head_) {//head is a special case so...
            nodeStr += tmp->data_;
        } else {
            nodeStr += " --> " + tmp->data_;  // add separator and node data
        }

        if (tmp->gap_ != 0 || tmp->jump_ != nullptr) {//looks for jumpnodes
            if (tmp == head_) {
                nodeGap = tmp->gap_;
                jmpNodeStr += tmp->data_;  // start building the jump node line with the first node
                jmpNodeGapStr += std::to_string(tmp->gap_);
            } else {
                int distanceBetweenJumpNodes = nodeGap * distanceBetweenNodes + nodeSize;//this took me 2 hours to figure out :|
                jmpNodeGapStr += std::string(distanceBetweenJumpNodes, ' ') + std::to_string(tmp->gap_);
                distanceBetweenJumpNodes -= space;// gets the actual hyphen length to make arrow
                jmpNodeStr += " " + std::string(distanceBetweenJumpNodes, '-') + "> " + tmp->data_; //makes arrow from one jump to next
                nodeGap = tmp->gap_;
            }
            nodeSize = 0;//resets for the next jumpnode arrow calculation
        }
        tmp = tmp->next_;
    }

    return nodeStr + "\n" + jmpNodeStr + "\n" + jmpNodeGapStr;
}


bool JumpList::insert(const string& s) {
	if(find(s)) {
		return false;
	}

	Node* tmp = head_;
	Node* prevJumpNode = nullptr;

	while(tmp->jump_ != nullptr&& tmp->jump_->data_ < s) {
		tmp = tmp->jump_;
		prevJumpNode = tmp;
	}

	while(tmp != nullptr && tmp->next_ && tmp->next_->data_ < s) {
		tmp = tmp->next_;
	}
	Node *prevNode = tmp;

	Node* newNode = new Node(s, prevNode->next_, nullptr, 0);
	prevNode->next_ = newNode;

	if(prevJumpNode!= nullptr) {
		prevJumpNode->gap_ ++;
	}

	return true;


}

bool JumpList::erase(const string& s) {

	return false; // dummy
}

int main() {
	JumpList jl(10);

	std::cout<<jl.find("C");;
}