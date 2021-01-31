#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ActorSlave.h"

bool IsFile(FILE* indexTable, FILE* database, char* error)
{
	if (indexTable == NULL || database == NULL)
	{
		strcpy(error, "database doesn't exist\n");
		return false;
	}

	return true;
}

bool IsIndex(FILE* indexTable, char* error, int id)
{
	fseek(indexTable, 0, SEEK_END);

	long indexTableSize = ftell(indexTable);

	if (indexTableSize == 0 || id * sizeof(struct MasterIndexer) > indexTableSize)
	{
		strcpy(error, "no such ID\n");
		return false;
	}

	return true;
}

bool IsRecord(struct MasterIndexer indexer, char* error)
{
	if (!indexer.exists)
	{
		strcpy(error, "this record has been removed\n");
		return false;
	}

	return true;
}


void info()
{
	FILE* indexTable = fopen("master.ind", "rb");

	if (indexTable == NULL)
	{
		printf("database doesn't exist\n");
		return;
	}

	int masterCount = 0;
	int slaveCount = 0;

	fseek(indexTable, 0, SEEK_END);
	int indAmount = ftell(indexTable) / sizeof(struct MasterIndexer);

	struct Master master;

	char dummy[51];

	for (int i = 1; i <= indAmount; i++)
	{
		if (GetMaster(&master, i, dummy))
		{
			masterCount++;
			slaveCount += master.slavesCount;

			printf("Master #%d has %d slave(s)\n", i, master.slavesCount);
		}
	}

	fclose(indexTable);

	printf("Number of masters: %d\n", masterCount);
	printf("Number of slaves: %d\n", slaveCount);
}