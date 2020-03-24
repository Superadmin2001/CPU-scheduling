#ifndef PROCESS_QUEUE_H
#define PROCESS_QUEUE_H

#include "utils.h"

typedef struct process
{
	uint32 id;
	uint32 arrivalTime;
	uint32 burstTime;
	uint32 priority;
	uint32 startTime;
	uint32 usedTime;
	uint32 endTime;
	uint8  timeStats[BUFFER_SIZE] = {0};
} process;

process processCreate(uint32 arrivalTime, uint32 burstTime, uint32 priority, uint32 usedTime, uint32 procesId);

void printProcessInfo(process* p);

typedef struct node
{
	process  data;
	node    *next;
} node;

typedef struct processQueue
{
	node   *root;
	uint32  count;
} processQueue;

void processQueueCreate(processQueue *q);
void processQueueFree(processQueue *q);
void processQueueCopy(processQueue *dst, processQueue *src);

process* processQueueFront(processQueue *q);
process* processQueueBack(processQueue *q);

void processQueuePush(processQueue *q, process p);
void processQueuePop(processQueue *q);

process* processQueueGetElementAt(processQueue *q, uint32 index);

bool processQueueGetElementWhichArrivedEarlierAndPopIt(processQueue *q, process *p);

uint32 processQueueGetMaxPriority(processQueue *q);

void processQueueSortById(processQueue *q);

#endif 

