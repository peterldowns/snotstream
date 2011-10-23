#include <stdlib.h>
#include <stdio.h>
#include "node.h"

Node *nd_make(void *data){ // Creates a new linked list, returns head=node
	Node *out = (Node *)malloc(sizeof(Node));
	out->head = NULL;
	out->data = data;
	out->tail = NULL;
	return out;
}

Node *nd_rem(Node *nd){ // remove a Node from a list of other nodes, and return it.
	Node *head = nd->head;
	Node *tail = nd->tail;
	if(head){
		head->tail = tail;
	}
	if(tail){
		tail->head = head;
	}
	return nd;
}

Node *nd_free(Node *nd){ // get rid of a node, returns child (if any)
	Node *tail = nd->tail;
	Node *tmp = nd_rem(nd);
	if(tmp->data){
		free(tmp->data);
	}
	free(tmp);
	return tail;
}

void nd_destroy(Node *nd){ // get rid of a node and and subchildren
	Node *curr = nd;
	while(curr){
		curr = nd_free(curr);
	}
}

Node *nd_last(Node *nd){ // gets last valid element
	Node *last = nd;
	while(last->tail){
		last = last->tail;
	}
	return last;
}

Node *nd_first(Node *nd){ // gets first valid element
	Node *first = nd;
	while(first->head){
		first = first->head;
	}
	return first;
}

Node *nd_append(Node *nd, Node *new_child){ // make and append a new node, inserting if necessary
	Node *old_child = nd->tail;

	nd->tail = new_child;
	new_child->head = nd;

	new_child->tail = old_child;

	if(old_child){
		old_child->head = new_child;
	}
	return new_child;
}

Node *nd_prepend(Node *nd, Node *new_head){ // make and prependa  new node, inserting if necessary
	Node *old_head = nd->head;

	nd->head = new_head;
	
	new_head->tail = nd;
	new_head->head = old_head;

	if(old_head){
		old_head->tail = new_head;
	}
	return new_head;
}

Node *nd_get_nth(Node *nd, int index){ // return the given or last valid element, at offset `index` (- = head, + = tail)
	Node *out = nd;
	int n = 0;
	int limit = abs(index);
	while(out && n < limit){
		out++;
		if(index < 0){
			out = out->head;
		}
		else{
			out = out->tail;
		}
	}
	return out;
}

Node *nd_reverse(Node *nd){
	Node *first = nd;
	Node *end = nd_last(nd);
	Node *next;
	do{
		next = first->tail;

		first->tail->head = NULL;
		first->tail = NULL;

		nd_append(end, first); // insert directly after original ending node
		first = next;
	} while(first != end);
	return first;
}
