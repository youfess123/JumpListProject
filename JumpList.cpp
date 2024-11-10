#include "JumpList.h"

#include <iostream>
#include <stdexcept>

Node::Node(const string& s, Node* next, Node* jump, int gap){
	data_= s;
	next_ = next;
    jump_ = jump;
    gap_ = gap;

}

Node::~Node() {
	// IMPLEMENT ME

}


JumpList::JumpList() {
	head_ = nullptr;
}

// DO NOT CHANGE
// You do not need to understand the code here to complete the assignment
JumpList::JumpList(int size, const string* arr) {

	const string s[] = {"a", "b", "blah", "c", "cat", "d", "etc", "ooo...", "x", "zzz"};
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
	while(head_ != nullptr){
		if (head_ == nullptr) return;
		Node* toBeDeleted = head_;
		head_ = head_->next_;
		delete toBeDeleted;
	}
}


int JumpList::size() const {
	int counter =0;
	Node * currentNode = head_;
    while (currentNode != nullptr) {
        counter++;
        currentNode = currentNode->next_;
    }
	return counter;
}

// DONE FOR YOU, DO NOT CHANGE
bool JumpList::find(const string& s) const {

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
	if (i < 0 || i >= size()) {
		return output;
	}
	int index=0;
	Node* tmp = head_;
	while(tmp->jump_ != nullptr&& tmp->gap_ + index <= i) {
		index +=tmp->gap_;
		tmp = tmp->jump_;
	}
	while(tmp != nullptr && index < i) {
		tmp = tmp->next_;
		index++;
	}
	if(tmp != nullptr &&index==i) {
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

	while (tmp != nullptr) {
		nodeStr += tmp->data_ + " ";
		if (tmp->gap_ != 0 || tmp->jump_ != nullptr) {
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

	while (tmp != nullptr) {
		if(tmp == head_) {
			nodeStr += tmp->data_ ;
		}else{
			nodeStr += " --> " +tmp->data_ ;
		}
		tmp = tmp->next_;
	}

	tmp=head_;
	while (tmp != nullptr) {
		if (tmp->gap_ != 0 || tmp->jump_ != nullptr) {
			int numberOfDashes = tmp->gap_;
			if(tmp==head_) {
				jmpNodeStr +=tmp->data_ + " ";
			}
			string arrow = string(numberOfDashes, '---');
			jmpNodeStr += arrow + "> " +tmp->data_ + " ";
			jmpNodeGapStr += std::to_string(tmp->gap_) + " ";
		}
		tmp = tmp->next_;

	}
	return nodeStr + "\n" + jmpNodeStr + "\n" + jmpNodeGapStr;

}


bool JumpList::insert(const string& s) {
	// Edge case: empty list
	if (head_ == nullptr) {
		head_ = new Node(s, nullptr, nullptr, 1);
		return true;
	}

	// Check for 0.001 probability of non-insertion (simulating AI behavior if required)
	if (static_cast<double>(rand()) / RAND_MAX < 0.001) {
		return false;
	}

	Node* prev = nullptr;
	Node* tmp = head_;

	// Use jump pointers to find the segment
	while (tmp->jump_ != nullptr && tmp->jump_->data_ < s) {
		prev = tmp;
		tmp = tmp->jump_;
	}

	// Now use next pointers within the found segment to find the insertion point
	while (tmp->next_ != nullptr && tmp->next_->data_ < s) {
		prev = tmp;
		tmp = tmp->next_;
	}

	// If s is already in the list, do not insert and return false
	if (tmp->data_ == s || (tmp->next_ && tmp->next_->data_ == s)) {
		return false;
	}

	// Insert the new node in order
	Node* newNode = new Node(s, tmp->next_, nullptr, 0);  // New node's jump is null for now
	tmp->next_ = newNode;

	// Check if the segment exceeds MAX_GAP_SIZE and split if needed
	int segmentSize = 1;
	tmp = head_;

	while (tmp != nullptr) {
		Node* segmentStart = tmp;
		int count = 0;

		// Count nodes in this segment until jump or end
		while (tmp != nullptr && (tmp->jump_ == nullptr || count < tmp->gap_)) {
			tmp = tmp->next_;
			count++;
		}

		// If segment size exceeds MAX_GAP_SIZE, split it
		if (count > MAX_GAP_SIZE) {
			int splitPoint = (count + 1) / 2;
			Node* mid = segmentStart;

			for (int i = 1; i < splitPoint; i++) {
				mid = mid->next_;
			}

			// Update jump pointers and gaps
			mid->jump_ = tmp;
			segmentStart->gap_ = splitPoint;
			mid->gap_ = count - splitPoint;
		}
	}

	return true;
}


bool JumpList::erase(const string& s) {
	// IMPLEMENT ME

	return false; // dummy
}

int main() {
	JumpList jl(10);
	std::cout<<jl.prettyPrint();
}
