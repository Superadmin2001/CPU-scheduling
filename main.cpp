#include <stdlib.h>
#include <stdio.h>

#include "processQueue.h"

#include <string>

void updatePriority(processQueue *processes, uint32 value)
{
	for (uint32 i = 0; i < processes->count; i++)
		processQueueGetElementAt(processes, i)->priority += value;
}

void setPriority(processQueue *processes, uint32 value)
{
	for (uint32 i = 0; i < processes->count; i++)
		processQueueGetElementAt(processes, i)->priority = value;
}

void printSRRStatistics(processQueue *processes, uint32 totalTime)
{
	processQueueSortById(processes);

	char fHalf[] = "id   burst time   arrival time";
	char sHalf[] = "total time   wasted time   reactivity ratio   penalty ratio";
	char time[BUFFER_SIZE * 4];

	uint32 position = 0;
	for (uint32 i = 0; i < totalTime-1; i++)
	{
		char number[3];
		_itoa(i+1, number, 10);
		uint8 len = strlen(number);

		for (uint8 i = 0; i < len; i++)
			time[position++] = number[i];
		time[position++] = ' ';
	}
	time[position] = 0;

	printf("%s   %s   %s\n", fHalf, time, sHalf);

	memset(time, 0, BUFFER_SIZE * 4);
	position = 0;

	for (uint32 i = 0; i < processes->count; i++)
	{
		process *t = processQueueGetElementAt(processes, i);
		printf("%u            %2u             %2u   ", t->id, t->burstTime, t->arrivalTime);

		char sym;
		bool isEnded = false;
		uint32 endingTime;
		for (uint32 j = 0; j < totalTime; j++)
		{
			if (t->timeStats[j] == ENDED)
			{
				isEnded = true;
				endingTime = j;
			}

			if (j == totalTime - 1)
				break;

			if (t->timeStats[j] == ACTIVE)
				sym = 'a';
			else if (j+1 < t->arrivalTime || isEnded)
				sym = '*';
			else
				sym = 'w';

			if (j > 9)
				time[position++] = ' ';

			time[position++] = sym;
			time[position++] = ' ';
		}

		isEnded = false;
		time[position] = 0;
		position = 0;

	    uint32 existenceTime = endingTime - t->arrivalTime;

		uint32 wastedTime = existenceTime - t->burstTime;
		float reactivityRatio = (float)t->burstTime / (float)existenceTime;
		float penaltyRatio = (float)existenceTime / (float)t->burstTime;

		printf("%s            %2u            %2u           %f        %f\n", time, existenceTime, wastedTime, reactivityRatio, penaltyRatio);
	}
}

void srr(processQueue *processes, uint32 quantum)
{
	processQueue holdingQueue, activeQueue, statistics;
	processQueueCreate(&holdingQueue);
	processQueueCreate(&activeQueue);
	processQueueCreate(&statistics);

	processQueueCopy(&holdingQueue, processes);
	setPriority(&holdingQueue, 0);

	uint32 a = 2, b = 1, holdingQueuePriority = 0, totalTime = 1;
	process e;
	processQueueGetElementWhichArrivedEarlierAndPopIt(&holdingQueue, &e);
	processQueuePush(&activeQueue, e);

	for (; activeQueue.count != 0; totalTime += quantum)
	{
		process *t = processQueueFront(&activeQueue);

		t->usedTime += quantum;
		if(totalTime < BUFFER_SIZE)
			t->timeStats[totalTime-1] = ACTIVE;

		if ( (t->burstTime - t->usedTime) < quantum)
		{
			t->timeStats[totalTime] = ENDED;

			processQueuePush(&statistics, *t);
			processQueuePop(&activeQueue);
		}
		else
		{
			//t->burstTime -= quantum;

			process handled = *t;
			processQueuePop(&activeQueue);
			processQueuePush(&activeQueue, handled);
		}

		holdingQueuePriority += a;
		updatePriority(&holdingQueue, a);
		updatePriority(&activeQueue, b);

		if (activeQueue.count == 0 && holdingQueue.count > 0 || holdingQueuePriority >= processQueueGetMaxPriority(&activeQueue) && holdingQueue.count > 0)
		{
			process tempProc;
			processQueueGetElementWhichArrivedEarlierAndPopIt(&holdingQueue, &tempProc);
			processQueuePush(&activeQueue, tempProc);
		}
	}

	printf("Selfish round-robin\n");
	printSRRStatistics(&statistics, totalTime);

	processQueueFree(&statistics);
}

void printSJFStatistics(processQueue *processes, uint32 totalTime)
{
	processQueueSortById(processes);

	char fHalf[] = "id   burst time   arrival time";
	char sHalf[] = "total time   wasted time   reactivity ratio   penalty ratio";
	char time[BUFFER_SIZE * 4];

	uint32 position = 0;
	for (uint32 i = 0; i < totalTime - 1; i++)
	{
		char number[3];
		_itoa(i + 1, number, 10);
		uint8 len = strlen(number);

		for (uint8 i = 0; i < len; i++)
			time[position++] = number[i];
		time[position++] = ' ';
	}
	time[position] = 0;

	printf("%s   %s   %s\n", fHalf, time, sHalf);

	memset(time, 0, BUFFER_SIZE * 4);
	position = 0;

	for (uint32 i = 0; i < processes->count; i++)
	{
		process *t = processQueueGetElementAt(processes, i);
		printf("%u            %2u             %2u   ", t->id, t->burstTime, t->arrivalTime);

		char sym;
		bool isEnded = false;
		uint32 endingTime;
		for (uint32 j = 0; j < totalTime; j++)
		{
			if (t->timeStats[j] == ENDED)
			{
				isEnded = true;
				endingTime = j;
			}

			if (j == totalTime - 1)
				break;

			if (t->timeStats[j] == ACTIVE)
				sym = 'a';
			else if (j + 1 < t->startTime || isEnded)
				sym = '*';

			if (j > 9)
				time[position++] = ' ';

			time[position++] = sym;
			time[position++] = ' ';
		}

		isEnded = false;
		time[position] = 0;
		position = 0;

		uint32 existenceTime = endingTime - t->startTime + 1;
		uint32 wastedTime = existenceTime - t->burstTime;
		float reactivityRatio = (float)t->burstTime / (float)existenceTime;
		float penaltyRatio = (float)existenceTime / (float)t->burstTime;

		printf("%s            %2u            %2u           %f        %f\n", time, existenceTime, wastedTime, reactivityRatio, penaltyRatio);
	}
}

void sjfSort(process *processes, uint32 count)
{
	for (size_t i = 0; i < count; i++)
	{
		for (uint32 j = 0; j < count - 1; j++)
		{
			if (processes[j].burstTime > processes[j + 1].burstTime || processes[j].burstTime == processes[j + 1].burstTime && processes[j].arrivalTime < processes[j + 1].arrivalTime)
			{
				process t = processes[j];
				processes[j] = processes[j + 1];
				processes[j + 1] = t;
			}
		}
	}
	
}

void sjf(process *processes, uint32 count, uint32 quantum)
{
	sjfSort(processes, count);

	processQueue q, statistics;
	processQueueCreate(&q);
	processQueueCreate(&statistics);

	for (uint32 i = 0; i < count; i++)
		processQueuePush(&q, processes[i]);

	uint32 totalTime = 1;
	process *t = processQueueFront(&q);
	bool isAccepted = false;
	for ( ; q.count != 0; totalTime += quantum)
	{
		t->usedTime += quantum;

		if (!isAccepted)
		{
			t->startTime = totalTime;
			isAccepted = true;
		}

		if (totalTime < BUFFER_SIZE)
			t->timeStats[totalTime - 1] = ACTIVE;

		if (t->burstTime - t->usedTime < quantum)
		{
			t->timeStats[totalTime] = ENDED;

			processQueuePush(&statistics, *t);
			processQueuePop(&q);
			t = processQueueFront(&q);

			isAccepted = false;
		}
	}

	printf("Shortest job first\n");
	printSJFStatistics(&statistics, totalTime);
	processQueueFree(&statistics);
}

void test()
{
	process processes[4] = {
		{ 1, 0,  10, 0, 0, 0, 0 },
		{ 2, 6,  5,  0, 0, 0, 0 },
		{ 3, 3,  4,  0, 0, 0, 0 },
		{ 4, 5,  8,  0, 0, 0, 0 }
	};

	processQueue q;
	processQueueCreate(&q);
	processQueuePush(&q, processes[0]);
	processQueuePush(&q, processes[1]);
	processQueuePush(&q, processes[2]);
	processQueuePush(&q, processes[3]);

	srr(&q, 1);
	printf("\n");
	sjf(processes, 4, 1);
	printf("\n");
}

void scheduling()
{
	uint32 processesCount, burstTimeMin, burstTimeMax, quantum;
	getUint32("Processes count: ", &processesCount);
	getUint32("Burst time (min): ", &burstTimeMin);
	getUint32("Burst time (max): ", &burstTimeMax);
	getUint32("quantum: ", &quantum);

	processQueue q;
	processQueueCreate(&q);
	process *processes = (process*)malloc(sizeof(process)*processesCount);
	for (uint32 i = 0; i < processesCount; i++)
	{
		processes[i].burstTime = getRandomValueWithinRangeUint32(burstTimeMin, burstTimeMax);
		processes[i].arrivalTime = getRandomValueWithinRangeUint32(0, 15);
		processes[i].usedTime = 0;
		processes[i].id = i;

		processQueuePush(&q, processes[i]);
		printProcessInfo(&processes[i]);
	}

	srr(&q, quantum);
	printf("\n");
	sjf(processes, processesCount, quantum);
	printf("\n");

	free(processes);
}

int main()
{
	test();
	
	system("pause");
	return 0;
}