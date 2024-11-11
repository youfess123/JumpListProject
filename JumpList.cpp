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
	const string s[] = {"apple", "b", "blah", "c", "cat", "d", "etc", "ooo...", "x", "zzz"};
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
	if (head_ == nullptr) return "";
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

string JumpList::prettyPrint() const {
    if (head_ == nullptr) return "";

    string nodeStr;
    string jmpNodeStr;
    string jmpNodeGapStr;

    Node* tmp = head_;
    const int arrowLength = 5;  // Length of " --> "
    const int arrowSpacing = 3; // Additional space adjustment for alignment

    int nonJumpNodeSize = 0;
    int cumulativeLength = 0;
    int previousJumpNodeGap = 0;

    while (tmp != nullptr) {//loop nodes
        if (tmp == head_) {
            nodeStr += tmp->data_;
        } else {
            nodeStr += " --> " + tmp->data_;
        }

        if (tmp->gap_ == 0 && tmp->next_ != nullptr) {//adds size of all non jumpnode strings
            nonJumpNodeSize += tmp->data_.size();
        }

        if (tmp->gap_ != 0 || tmp->jump_ != nullptr) {//looks for jumpnodes
            if (tmp == head_) {// starts building first part of the 2last string
                previousJumpNodeGap = tmp->gap_;
                jmpNodeStr += tmp->data_;
                jmpNodeGapStr += std::to_string(tmp->gap_);
            } else {
                int distanceBetweenJumpNodes = previousJumpNodeGap * arrowLength + nonJumpNodeSize;
                int distanceBetweenGaps = std::max(0, cumulativeLength + distanceBetweenJumpNodes - static_cast<int>(jmpNodeGapStr.size()));//calcs correct distance for gaps
                jmpNodeGapStr += std::string(distanceBetweenGaps, ' ') + std::to_string(tmp->gap_);
                distanceBetweenJumpNodes -= arrowSpacing;
                jmpNodeStr += " " + std::string(distanceBetweenJumpNodes, '-') + "> " + tmp->data_;
                previousJumpNodeGap = tmp->gap_;
            }
            cumulativeLength = jmpNodeStr.size();
            nonJumpNodeSize = 0;//reset for next ddistanceBetweenJumpNodes
        }
        tmp = tmp->next_;
    }
    return nodeStr + "\n" + jmpNodeStr + "\n" + jmpNodeGapStr;
}

bool JumpList::insert(const string& s) {
	if( ! head_) {// empty list case
		Node* newNode = new Node(s,nullptr,nullptr,0);
		head_ = newNode;
		return true;
	}
	if(find(s)) {//duplicate check
		return false;
	}
	if (s < head_->data_) {
		Node* oldHead = head_;

		if (head_->gap_ < MAX_GAP_SIZE) {
			// Case when the head's gap is less than 5
			Node* newHeadNode = new Node(s, oldHead, oldHead->jump_, head_->gap_ + 1);
			head_ = newHeadNode;
			oldHead->gap_ = 0; // Make the old head a normal node
			oldHead->jump_ = nullptr;
		} else if (head_->gap_ == MAX_GAP_SIZE) {//head gap=5, so its segmented
			Node* secondJumpNode = oldHead;
			for (int i = 0; i < 2; i++) {
				secondJumpNode = secondJumpNode->next_;
			}
			Node* newHeadNode = new Node(s, oldHead, secondJumpNode, 3);
			secondJumpNode->gap_ = 3;
			secondJumpNode->jump_ = oldHead->jump_;
			head_ = newHeadNode;
			oldHead->gap_ = 0;
			oldHead->jump_ = nullptr;

		}
	}else {//s is not going to be head
		Node* actualNode = head_;

		while(actualNode->jump_ != nullptr&& actualNode->jump_->data_ < s) {//fast lane
			actualNode = actualNode->jump_;
		}

		Node* tmpNode= actualNode;
		while(tmpNode != nullptr && tmpNode->next_ && tmpNode->next_->data_ < s) {//slow lane
			tmpNode = tmpNode->next_;
		}

		Node* prevNode = tmpNode;
		Node* newNode = new Node(s, prevNode->next_, nullptr, 0);//make new node for insertion
		prevNode->next_ = newNode;

		if(actualNode->gap_ +1>MAX_GAP_SIZE) {//if it messes with jump nodes segment it
			actualNode->gap_=3;
			Node* targetNode = actualNode;
			for(int i=0; i < 3; i++) {
				targetNode = targetNode->next_;
			}
			targetNode->gap_ =3;
			targetNode->jump_= actualNode->jump_;
			actualNode->jump_ = targetNode;
		}else{								//if it doesnt mess with jump nodes add 1 to previous jumpnode gap
			actualNode->gap_++;
		}
	}
	return true;
}

bool JumpList::erase(const string &s) {
	if (!head_)
		return false;

	Node* tmpNode = head_;
	Node* prevJumpNode = nullptr;

	while (tmpNode->jump_ != nullptr && tmpNode->jump_->data_ < s) {//fast lane
		prevJumpNode = tmpNode;
		tmpNode = tmpNode->jump_;
	}

	bool found = false;
	while (tmpNode->next_ != nullptr) {
		if (tmpNode->next_->data_ == s) {
			found = true;
			break;
		} else if (tmpNode->next_->data_ > s) {
			found = false;
			break;
		} else {//slow lane
			tmpNode = tmpNode->next_;
		}
	}

	if (found) {//node deleted is not jump node
		Node* nodeToBeDeleted = tmpNode->next_;
		if (prevJumpNode != nullptr && nodeToBeDeleted->gap_ > 0) {
			prevJumpNode->gap_--;
		}
		tmpNode->next_ = nodeToBeDeleted->next_;
		delete nodeToBeDeleted;
		if (prevJumpNode != nullptr) {
			Node* targetNode = prevJumpNode;
			if (prevJumpNode->gap_ == 0 && prevJumpNode->jump_ != nullptr) {
				prevJumpNode->jump_ = targetNode->next_;
			}
		}
		return true;
	}
	return false;
}
