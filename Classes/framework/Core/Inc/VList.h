#pragma once
#ifndef __VLIST_H__
#define __VLIST_H__

template<typename T>
struct VListNode
{
	inline VListNode(const T& E) :Element(E), PrevNode(nullptr), NextNode(nullptr){}
	T Element;
	VListNode* PrevNode;
	VListNode* NextNode;
};

//template<typename T>
//struct VListNodeAllocator
//{
//	typedef typename VListNode<T> ListNode;
//	inline  ListNode* AllocNode(const T& Value)
//	{
//		return new ListNode(Value);
//	}
//	inline void FreeNode(ListNode* Node)
//	{
//		delete Node;
//	}
//};


template <typename T>
class VList
{
public:
	typedef VListNode<T> ListNode;

	VList() :mHead(nullptr), mTail(nullptr), mSize(0)
	{
	}

	virtual ~VList()
	{
		Clear();
	}


	inline int Size() const { return mSize;  }
	inline ListNode* GetHead() { return mHead;  }
	inline ListNode* GetTail() { return mTail; }
	void Clear()
	{
		ListNode* pNode;
		while (mHead != NULL)
		{
			pNode = mHead->NextNode;
			FreeNode(mHead);
			mHead = pNode;
		}

		mHead = mTail = NULL;
		mSize = 0;
	}

	void PushFront(const T& Element)
	{
		ListNode* NewNode = AllocNode(Element);
		VASSERT(NewNode);

		// have an existing head node - change the head node to point to this one
		if (mHead != NULL)
		{
			NewNode->NextNode = mHead;
			mHead->PrevNode = NewNode;
			mHead = NewNode;
		}
		else
		{
			mHead = mTail = NewNode;
		}

		mSize++;
	}
	
	void PushBack(const T& Element)
	{
		ListNode* NewNode = AllocNode(Element);
		VASSERT(NewNode);

		if (mTail != NULL)
		{
			mTail->NextNode = NewNode;
			NewNode->PrevNode = mTail;
			mTail = NewNode;
		}
		else
		{
			mHead = mTail = NewNode;
		}

		mSize++;
	}


	inline T PopFront()
	{
		T Value = mHead->Element;
		RemoveNode(mHead);
		return Value;
	}

	inline T PopBack()
	{
		T Value = mTail->Element;
		RemoveNode(mTail);
		return Value;
	}

	/**
	* Remove the node corresponding to InElement
	*
	* @param	InElement	the value to remove from the list
	*/
	void RemoveNode(const T& Element)
	{
		ListNode* Node = FindNode(Element);
		RemoveNode(Node);
	}



	virtual ListNode* AllocNode(const T& Element) = 0;
	virtual void FreeNode(ListNode* Node) = 0;

	ListNode* FindNode(const T& Element) const
	{
		ListNode* pNode = mHead;
		while (pNode != NULL)
		{
			if (pNode->Element == Element)
			{
				break;
			}

			pNode = pNode->NextNode;
		}

		return pNode;
	}

	/**
	* Insert the specified value into the list at an arbitrary point.
	*
	* @param	InElement			the value to insert into the list
	* @param	NodeToInsertBefore	the new node will be inserted into the list at the current location of this node
	*								if NULL, the new node will become the new head of the list
	*
	* @return	whether the node was successfully added into the list
	*/
	void InsertNode(const T& InElement, ListNode* NodeToInsertBefore = NULL)
	{
		if (NodeToInsertBefore == NULL || NodeToInsertBefore == mHead)
		{
			PushFront(InElement);
			return;
		}

		ListNode* NewNode = AllocNode(InElement);
		VASSERT(NewNode);

		NewNode->PrevNode = NodeToInsertBefore->PrevNode;
		NewNode->NextNode = NodeToInsertBefore;

		NodeToInsertBefore->PrevNode->NextNode = NewNode;
		NodeToInsertBefore->PrevNode = NewNode;

		mSize++;
	}

	/**
	* Removes the node specified.
	*/
	void RemoveNode(ListNode* NodeToRemove)
	{
		if (NodeToRemove != NULL)
		{
			// if we only have one node, just call Clear() so that we don't have to do lots of extra checks in the code below
			if (mSize == 1)
			{
				VASSERT(NodeToRemove == mHead);
				Clear();
				return;
			}

			if (NodeToRemove == mHead)
			{
				mHead = mHead->NextNode;
				mHead->PrevNode = NULL;
			}
			else if (NodeToRemove == mTail)
			{
				mTail = mTail->PrevNode;
				mTail->NextNode = NULL;
			}
			else
			{
				NodeToRemove->NextNode->PrevNode = NodeToRemove->PrevNode;
				NodeToRemove->PrevNode->NextNode = NodeToRemove->NextNode;
			}

			FreeNode(NodeToRemove);
			--mSize;
		}
	}
protected:
	ListNode* mHead;
	ListNode* mTail;
	int mSize;
	//NodeAllocator mNodeAllocator;

public:
		class ListIterator
		{
		public:
			explicit inline ListIterator(ListNode* StartingNode)
				: CurrentNode(StartingNode)
			{
			}

			/** conversion to "bool" returning true if the iterator is valid. */
			inline operator bool() const
			{
				return CurrentNode != nullptr;
			}

			inline ListIterator& operator++()
			{
				CurrentNode = CurrentNode->NextNode;
				return *this;
			}

			inline ListIterator& operator--()
			{
				CurrentNode = CurrentNode->PrevNode;
				return *this;
			}

			
			// Accessors.
			inline T& operator->() const
			{
				return CurrentNode->Element;
			}

			inline T& operator*() const
			{
				
				return CurrentNode->Element;
			}

			inline ListNode* GetNode() const
			{
				return CurrentNode;
			}

		private:
			ListNode* CurrentNode;

			friend bool operator==(const ListIterator& Lhs, const ListIterator& Rhs) { return Lhs.CurrentNode == Rhs.CurrentNode; }
			friend bool operator!=(const ListIterator& Lhs, const ListIterator& Rhs) { return Lhs.CurrentNode != Rhs.CurrentNode; }
		};


		typedef ListIterator Iterator;
		inline Iterator      begin() { return Iterator(mHead); }
		inline Iterator      end() { return Iterator(nullptr); }
	
		/*inline Iterator erase(const Iterator& Itr)
		{

		}*/

};

#endif 

