#include "processQueue.h"

process processCreate(uint32 arrivalTime, uint32 burstTime, uint32 priority, uint32 usedTime, uint32 procesId)
{
	process p;
	p.arrivalTime = arrivalTime;
	p.burstTime = burstTime;
	p.priority = priority;
	p.usedTime = usedTime;
	p.id = procesId;

	return p;
}

void printProcessInfo(process* p)
{
	printf("[processId = %u, arrivalTime = %u, burstTime = %u, usedTime = %u]\n", p->id, p->arrivalTime, p->burstTime, p->usedTime);
}

void processQueueCreate(processQueue *q)
{
	q->root = NULL;
	q->count = 0;
}

void processQueueFree(processQueue *q)
{
	if (q == NULL || q->root == NULL)
		return;

	node *t = NULL;
	for (; q->root != NULL; )
	{
		t = q->root->next;
		free(q->root);
		q->root = t;
	}

	q->count = 0;
}

void processQueueCopy(processQueue *dst, processQueue *src)
{
	if (src == NULL || src->root == NULL)
		return;

	process t;
	for (uint32 i = 0; i < src->count; i++)
	{
		t = *(processQueueGetElementAt(src, i));
		processQueuePush(dst, t);
	}
}

process* processQueueFront(processQueue *q)
{
	if(q != NULL && q->root != NULL)
		return &q->root->data;

	return NULL;
}


process* processQueueBack(processQueue *q)
{
	if (q == NULL || q->root == NULL)
		return NULL;

	node *t = q->root;
	for ( ; t->next != NULL; t = t->next)
		;

	return &t->data;
}

void processQueuePush(processQueue *q, process p)
{
	if (q == NULL)
		return;

	if (q->root == NULL)
	{
		q->root = (node*)malloc(sizeof(node));
		q->root->data = p;
		q->root->next = NULL;
		q->count++;
		return;
	}

	node *t = q->root;
	for (; t->next != NULL; t = t->next)
		;

	t->next = (node*)malloc(sizeof(node));
	t->next->data = p;
	t->next->next = NULL;

	q->count++;
}

void processQueuePop(processQueue *q)
{
	if (q == NULL || q->root == NULL)
		return;

	if (q->root->next == NULL)
	{
		free(q->root);
		q->root = NULL;
		q->count = 0;
		return;
	}

	node *first = q->root;
	node *next = first->next;

	q->root = next;
	free(first);
	q->count--;

	return;
}

process* processQueueGetElementAt(processQueue *q, uint32 index)
{
	if (q == NULL || q->root == NULL || index >= q->count)
		return NULL;

	node *t = q->root;
	for (uint32 i = 0; i < index; t = t->next, i++)
		;

	return &t->data;
}

bool processQueueGetElementWhichArrivedEarlierAndPopIt(processQueue *q, process *p)
{
	if (q == NULL || q->root == NULL)
		return false;

	if (q->count == 1)
	{
		*p = q->root->data;
		processQueuePop(q);
		return p;
	}

	node *t = q->root->next;
	uint32 minIndex = 0;
	uint32 min = q->root->data.arrivalTime;
	for (uint32 i = 1; i < q->count; t = t->next, i++)
	{
		if (t->data.arrivalTime < min)
		{
			min = t->data.arrivalTime;
			minIndex = i;
		}
	}

	t = q->root;
	for (uint32 i = 0; i < minIndex; t = t->next, i++)
		;

	if (minIndex == 0)
	{
		*p = t->data;
		processQueuePop(q);
		return true;
	}

	if (t->next == NULL)
	{
		*p = t->data;

		t = q->root;
		for (uint32 i = 0; i < minIndex - 1; t = t->next, i++)
			;

		free(t->next);
		t->next = NULL;

		q->count--;
		return true;
	}

	*p = t->data;

	t = q->root;
	for (uint32 i = 0; i < minIndex-1; t = t->next, i++)
		;

	node *del = t->next;
	t->next = t->next->next;
	free(del);
	q->count--;

	return true;
}

uint32 processQueueGetMaxPriority(processQueue *q)
{
	if (q == NULL || q->root == NULL)
		return 0;

	node *t = q->root->next;
	uint32 max = q->root->data.priority;
	for (; t != NULL; t = t->next)
	{
		if (t->data.priority > max)
		{
			max = t->data.arrivalTime;
		}
	}

	return max;
}

static node* processQueueGetNodeAt(processQueue *q, uint32 index)
{
	if (q == NULL || q->root == NULL || index >= q->count)
		return NULL;

	node *t = q->root;
	for (uint32 i = 0; i < index; t = t->next, i++)
		;

	return t;
}

void processQueueSortById(processQueue *q)
{
	if (q->count < 2)
		return;

	node *cur, *next;
	process t;

	for (uint32 i = 0; i < q->count; i++)
	{
		for (uint32 j = 0; j < q->count - 1; j++)
		{
			cur = processQueueGetNodeAt(q, j);
			next = processQueueGetNodeAt(q, j + 1);

			if (cur->data.id > next->data.id)
			{
				t = cur->data;
				cur->data = next->data;
				next->data = t;
			}
		}
	}
}