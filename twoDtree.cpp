
/**
 *
 * twoDtree (pa3)
 * slight modification of a Kd tree of dimension 2.
 * twoDtree.cpp
 * This file will be used for grading.
 *
 */

#include "twoDtree.h"


/* node constructor given */
twoDtree::Node::Node(pair<int,int> ul, pair<int,int> lr, HSLAPixel a)
	:upLeft(ul),lowRight(lr),avg(a),LT(NULL),RB(NULL)
	{}

/* destructor given */
twoDtree::~twoDtree(){
	clear();
}

/* copy constructor given */
twoDtree::twoDtree(const twoDtree & other) {
	copy(other);
}


/* operator= given */
twoDtree & twoDtree::operator=(const twoDtree & rhs){
	if (this != &rhs) {
		clear();
		copy(rhs);
	}
	return *this;
}

/* twoDtree constructor */
twoDtree::twoDtree(PNG & imIn){ 
	height = imIn.height();
	width = imIn.width();
	pair<int, int> origin (0,0);
	pair<int, int> end (width-1, height-1);
	stats s = stats(imIn);
	root = buildTree(s, origin, end, true);
}

/* buildTree helper for twoDtree constructor */
twoDtree::Node * twoDtree::buildTree(stats & s, pair<int,int> ul, pair<int,int> lr, bool vert) {
	pair<int, int> LTul;
	pair<int, int> LTlr;
	pair<int, int> RBul;
	pair<int, int> RBlr;
	bool LTvert = !vert;
	bool RBvert = !vert;
	Node * n = new Node(ul, lr, s.getAvg(ul, lr));
	if (ul == lr) return n;
	double bestEntropy;
	long totalArea = s.rectArea(ul, lr);
	if (vert) {
		for (int i = ul.first; i < lr.first; i++){
			pair<int, int> LTulTemp (ul.first, ul.second);
			pair<int, int> LTlrTemp (i, lr.second);
			pair<int, int> RBulTemp (i+1, ul.second);
			pair<int, int> RBlrTemp (lr.first, lr.second);
			double weighted_entropy = s.entropy(LTulTemp, LTlrTemp)*s.rectArea(LTulTemp, LTlrTemp)/totalArea 
									  + s.entropy(RBulTemp, RBlrTemp)*s.rectArea(RBulTemp, RBlrTemp)/totalArea;
			if (i == ul.first) {
				bestEntropy = weighted_entropy;
				LTul = LTulTemp;
				LTlr = LTlrTemp;
				RBul = RBulTemp;
				RBlr = RBlrTemp;
			} else if (weighted_entropy <= bestEntropy) {
				bestEntropy = weighted_entropy;
				LTul = LTulTemp;
				LTlr = LTlrTemp;
				RBul = RBulTemp;
				RBlr = RBlrTemp;
			// } else if (weighted_entropy == bestEntropy) {
			// 	if (s.rectArea(RBulTemp, RBlrTemp) < s.rectArea(RBul, RBlr)) {
			// 		LTul = LTulTemp;
			// 		LTlr = LTlrTemp;
			// 		RBul = RBulTemp;
			// 		RBlr = RBlrTemp;
			// 	}
			}
		}
	} else {
		for (int j = ul.second; j < lr.second; j++){
			pair<int, int> LTulTemp (ul.first, ul.second);
			pair<int, int> LTlrTemp (lr.first, j);
			pair<int, int> RBulTemp (ul.first, j+1);
			pair<int, int> RBlrTemp (lr.first, lr.second);
			double weighted_entropy = s.entropy(LTulTemp, LTlrTemp)*s.rectArea(LTulTemp, LTlrTemp)/totalArea 
									  + s.entropy(RBulTemp, RBlrTemp)*s.rectArea(RBulTemp, RBlrTemp)/totalArea;
			if (j == ul.second) {
				bestEntropy = weighted_entropy;
				LTul = LTulTemp;
				LTlr = LTlrTemp;
				RBul = RBulTemp;
				RBlr = RBlrTemp;
			} else if (weighted_entropy <= bestEntropy) {
				bestEntropy = weighted_entropy;
				LTul = LTulTemp;
				LTlr = LTlrTemp;
				RBul = RBulTemp;
				RBlr = RBlrTemp;
			// } else if (weighted_entropy == bestEntropy) {
			// 	if (s.rectArea(RBulTemp, RBlrTemp) < s.rectArea(RBul, RBlr)) {
			// 		LTul = LTulTemp;
			// 		LTlr = LTlrTemp;
			// 		RBul = RBulTemp;
			// 		RBlr = RBlrTemp;
			// 	}
			}
		}
	}
	// A 1xw rectangle will only have vertical
    // splits remaining, and a hx1 rectangle will only have 
    // horizontal splits remaining.

	if      (LTul.first == LTlr.first)   LTvert = false;
	else if (LTul.second == LTlr.second) LTvert = true;
	
	if (RBul.first == RBlr.first)   RBvert = false;
	else if (RBul.second == RBlr.second) RBvert = true;

	n->LT = buildTree(s, LTul, LTlr, LTvert);
	n->RB = buildTree(s, RBul, RBlr, RBvert);

	return n;
}

/* render your twoDtree into a png */
PNG twoDtree::render(){
	PNG result = PNG(width, height);
	renderNodes(result, root);
	return result;
}

/* prune function modifies tree by cutting off
 * subtrees whose leaves are all within tol of 
 * the average pixel value contained in the root
 * of the subtree
 */
/*
    *  Prune function trims subtrees as high as possible in the tree.
    *  A subtree is pruned (cleared) if all of the subtree's leaves are within 
    *  tol of the average color stored in the root of the subtree. 
    *  Pruning criteria should be evaluated on the original tree, not 
    *  on any pruned subtree. (we only expect that trees would be pruned once.)
    *  
*/
void twoDtree::prune(double tol) {
	pruneNodes(tol, root);
}

/* helper function for destructor and op= */
/* frees dynamic memory associated w the twoDtree */
void twoDtree::clear() {
	clearNode(root);
}


/* helper function for copy constructor and op= */
void twoDtree::copy(const twoDtree & orig){
	root = copyNodes(orig.root);
	height = orig.height;
	width = orig.width;
	
}

void twoDtree::clearNode(Node * node) {
	if (node->LT == NULL && node->RB == NULL) {
		// go to end of if
	} else {
		clearNode(node->LT);
		clearNode(node->RB);
	}
	free(node);
	node = NULL;
}

void twoDtree::renderNodes(PNG & im, Node * node) {
	if (node->LT == NULL && node->RB == NULL) {
		for (int i = node->upLeft.first; i <= node->lowRight.first; i++) {
			for (int j = node->upLeft.second; j <= node->lowRight.second; j++) {
				HSLAPixel * pixToChange = im.getPixel(i, j);
				*pixToChange = node->avg;
			}
		}
	// } else if (node->LT == NULL) {
	// 	renderNodes(im, node->RB);
	// } else if (node->RB == NULL) {
	// 	renderNodes(im, node->LT);
	} else {
		renderNodes(im, node->LT);
		renderNodes(im, node->RB);
	}
}

void twoDtree::pruneNodes(double tol, Node * node) {
	if (isLeaf(node)) return;
	else if (shouldPrune(tol, node, node->avg)) {
		clearNode(node->LT);
		node->LT = NULL;
		clearNode(node->RB);
		node->RB = NULL;
	} else {
		pruneNodes(tol, node->LT);
		pruneNodes(tol, node->RB);
	}
}


bool twoDtree::isLeaf(Node * node) {
	return (node->LT == NULL && node->RB == NULL);
}

twoDtree::Node * twoDtree::copyNodes(const Node * orig) {
	if (orig == NULL) {
		return NULL;
	} else {
		Node * node = new Node(orig->upLeft, orig->lowRight, orig->avg);
		node->RB = copyNodes(orig->RB);
		node->LT = copyNodes(orig->LT);
		return node;
	}
}


bool twoDtree::shouldPrune(double tol, Node * node, HSLAPixel prev_avg) {
	if (node == NULL) return true;

	if (isLeaf(node)) {
		return (node->avg.dist(prev_avg) < tol);
	} else {
		return shouldPrune(tol, node->LT, prev_avg) && shouldPrune(tol, node->RB, prev_avg);
	}
}


