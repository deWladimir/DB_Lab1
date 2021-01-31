#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ActorCheckFunc.h"
#include "ActorSlave.h"

#define M_IND "MASTER.ind"
#define M_DATA "MASTER.fl"
#define M_GARBAGE "MASTER_GARBAGE.txt"
#define INDEX_SIZE sizeof(struct MasterIndexer)
#define M_SIZE sizeof(struct Master)


void NoteMaster(int id)
{
	FILE* garbageZone = fopen(M_GARBAGE, "rb");

	int garbageCount;
	fscanf(garbageZone, "%d", &garbageCount);

	int* delIds = malloc(garbageCount * sizeof(int));

	for (int i = 0; i < garbageCount; i++)
	{
		fscanf(garbageZone, "%d", delIds + i);
	}

	fclose(garbageZone);
	garbageZone = fopen(M_GARBAGE, "wb");
	fprintf(garbageZone, "%d", garbageCount + 1);

	for (int i = 0; i < garbageCount; i++)
	{
		fprintf(garbageZone, " %d", delIds[i]);
	}

	fprintf(garbageZone, " %d", id);
	free(delIds);
	fclose(garbageZone);
}



int InsertMaster(struct Master record)
{
	FILE* indexTable = fopen(M_IND, "a+b");
	FILE* database = fopen(M_DATA, "a+b");
	FILE* garbageZone = fopen(M_GARBAGE, "rb");
	struct MasterIndexer indexer;

	

		long indexerSize = INDEX_SIZE;

		fseek(indexTable, 0, SEEK_END);

		if (ftell(indexTable))
		{
			fseek(indexTable, -indexerSize, SEEK_END);
			fread(&indexer, INDEX_SIZE, 1, indexTable);

			record.id = indexer.id + 1;
		}
		else
		{
			record.id = 1;
		}
	

	record.firstSlaveAddress = -1;
	record.slavesCount = 0;

	fwrite(&record, M_SIZE, 1, database);

	indexer.id = record.id;
	indexer.address = (record.id - 1) * M_SIZE;
	indexer.exists = 1;

	printf("Your master id is %d\n", record.id);

	fseek(indexTable, (record.id - 1) * INDEX_SIZE, SEEK_SET);
	fwrite(&indexer, INDEX_SIZE, 1, indexTable);
	fclose(indexTable);
	fclose(database);

	return 1;
}

int GetMaster(struct Master* master, int id, char* error)
{
	FILE* indexTable = fopen(M_IND, "rb");
	FILE* database = fopen(M_DATA, "rb");

	if (!IsFile(indexTable, database, error))
	{
		return 0;
	}

	struct MasterIndexer indexer;

	if (!IsIndex(indexTable, error, id))
	{
		return 0;
	}

	fseek(indexTable, (id - 1) * INDEX_SIZE, SEEK_SET);
	fread(&indexer, INDEX_SIZE, 1, indexTable);

	if (!IsRecord(indexer, error))
	{
		return 0;
	}

	fseek(database, indexer.address, SEEK_SET);
	fread(master, sizeof(struct Master), 1, database);
	fclose(indexTable);
	fclose(database);

	return 1;
}

int UpdateMaster(struct Master master, char* error)
{
	FILE* indexTable = fopen(M_IND, "r+b");
	FILE* database = fopen(M_DATA, "r+b");

	if (!IsFile(indexTable, database, error))
	{
		return 0;
	}

	struct MasterIndexer indexer;
	int id = master.id;

	if (!IsIndex(indexTable, error, id))
	{
		return 0;
	}

	fseek(indexTable, (id - 1) * INDEX_SIZE, SEEK_SET);
	fread(&indexer, INDEX_SIZE, 1, indexTable);

	if (!IsRecord(indexer, error))
	{
		return 0;
	}

	fseek(database, indexer.address, SEEK_SET);
	fwrite(&master, M_SIZE, 1, database);
	fclose(indexTable);
	fclose(database);

	return 1;
}

int DeleteMaster(int id, char* error)
{
	FILE* indexTable = fopen(M_IND, "r+b");

	if (indexTable == NULL)
	{
		strcpy(error, "database doesn't exist");
		return 0;
	}

	if (!IsIndex(indexTable, error, id))
	{
		return 0;
	}

	struct Master master;
	GetMaster(&master, id, error);

	struct MasterIndexer indexer;

	fseek(indexTable, (id - 1) * INDEX_SIZE, SEEK_SET);
	fread(&indexer, INDEX_SIZE, 1, indexTable);

	indexer.exists = 0;

	fseek(indexTable, (id - 1) * INDEX_SIZE, SEEK_SET);

	fwrite(&indexer, INDEX_SIZE, 1, indexTable);
	fclose(indexTable);

	NoteMaster(id);


	if (master.slavesCount)
	{
		FILE* slavesDb = fopen(S_DATA, "r+b");
		struct Slave slave;

		fseek(slavesDb, master.firstSlaveAddress, SEEK_SET);

		for (int i = 0; i < master.slavesCount; i++)
		{
			fread(&slave, S_SIZE, 1, slavesDb);
			fclose(slavesDb);
			DeleteSlave(master, slave, slave.actorId, error);

			slavesDb = fopen(S_DATA, "r+b");
			fseek(slavesDb, slave.nextAddress, SEEK_SET);
		}

		fclose(slavesDb);
	}
	return 1;
}

void PrintMaster(struct Master master)
{
	printf("Master name: %s\n", master.name);
}

void ReadMaster(struct Master* master)
{
	char name[16];

	name[0] = '\0';

	printf("Input master name: ");
	scanf("%s", name);

	strcpy(master->name, name);

}