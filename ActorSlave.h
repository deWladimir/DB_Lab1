#pragma once
//#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include "InfoStructs.h"
#include "ActorCheckFunc.h"
#include "FilmMaster.h"

#define S_DATA "SLAVE.fl"
#define S_GARBAGE "SLAVE_GARBAGE.txt"
#define S_SIZE sizeof(struct Slave)


void ReDatabase(FILE* database)
{
	fclose(database);
	database = fopen(S_DATA, "r+b");
}

void LAddress(FILE* database, struct Master master, struct Slave slave)
{
	ReDatabase(database);

	struct Slave previous;

	fseek(database, master.firstSlaveAddress, SEEK_SET);

	for (int i = 0; i < master.slavesCount; i++)
	{
		fread(&previous, S_SIZE, 1, database);
		fseek(database, previous.nextAddress, SEEK_SET);
	}

	previous.nextAddress = slave.selfAddress;
	fwrite(&previous, S_SIZE, 1, database);
}

void ReAddress(FILE* database, struct Slave previous, struct Slave slave, struct Master* master)
{
	if (slave.selfAddress == master->firstSlaveAddress)
	{
		if (slave.selfAddress == slave.nextAddress)
		{
			master->firstSlaveAddress = -1;
		}
		else
		{
			master->firstSlaveAddress = slave.nextAddress;
		}
	}
	else
	{
		if (slave.selfAddress == slave.nextAddress)
		{
			previous.nextAddress = previous.selfAddress;
		}
		else
		{
			previous.nextAddress = slave.nextAddress;
		}

		fseek(database, previous.selfAddress, SEEK_SET);
		fwrite(&previous, S_SIZE, 1, database);
	}
}

void NoteSlave(long address)
{
	FILE* garbageZone = fopen(S_GARBAGE, "rb");

	int garbageCount;
	fscanf(garbageZone, "%d", &garbageCount);

	long* delAddresses = malloc(garbageCount * sizeof(long));

	for (int i = 0; i < garbageCount; i++)
	{
		fscanf(garbageZone, "%ld", delAddresses + i);
	}

	fclose(garbageZone);
	garbageZone = fopen(S_GARBAGE, "wb");
	fprintf(garbageZone, "%ld", garbageCount + 1);

	for (int i = 0; i < garbageCount; i++)
	{
		fprintf(garbageZone, " %ld", delAddresses[i]);
	}

	fprintf(garbageZone, " %d", address);
	free(delAddresses);
	fclose(garbageZone);
}

void WriteGarbageAddress(int garbageCount, FILE* garbageZone, struct Slave* record)
{
	long* delIds = malloc(garbageCount * sizeof(long));

	for (int i = 0; i < garbageCount; i++)
	{
		fscanf(garbageZone, "%d", delIds + i);
	}

	record->selfAddress = delIds[0];
	record->nextAddress = delIds[0];

	fclose(garbageZone);
	fopen(S_GARBAGE, "wb");
	fprintf(garbageZone, "%d", garbageCount - 1);

	for (int i = 1; i < garbageCount; i++)
	{
		fprintf(garbageZone, " %d", delIds[i]);
	}

	free(delIds);
	fclose(garbageZone);
}

int InsertSlave(struct Master master, struct Slave slave, char* error)
{
	slave.exists = 1;

	FILE* database = fopen(S_DATA, "a+b");
	FILE* garbageZone = fopen(S_GARBAGE, "rb");


		ReDatabase(database);
		fseek(database, slave.selfAddress, SEEK_SET);
		fseek(database, 0, SEEK_END);

		int dbSize = ftell(database);

		slave.selfAddress = dbSize;
		slave.nextAddress = dbSize;
	

	fwrite(&slave, S_SIZE, 1, database);

	if (!master.slavesCount)
	{
		master.firstSlaveAddress = slave.selfAddress;
	}
	else
	{
		LAddress(database, master, slave);
	}

	fclose(database);

	master.slavesCount++;
	UpdateMaster(master, error);

	return 1;
}

int GetSlave(struct Master master, struct Slave* slave, int productId, char* error)
{
	if (!master.slavesCount)
	{
		strcpy(error, "No slaves for this master\n");
		return 0;
	}

	FILE* database = fopen(S_DATA, "rb");


	fseek(database, master.firstSlaveAddress, SEEK_SET);
	fread(slave, S_SIZE, 1, database);

	for (int i = 0; i < master.slavesCount; i++)
	{
		if (slave->actorId == productId)
		{
			fclose(database);
			return 1;
		}

		fseek(database, slave->nextAddress, SEEK_SET);
		fread(slave, S_SIZE, 1, database);
	}

	strcpy(error, "No such slave\n");
	fclose(database);
	return 0;
}


int updateSlave(struct Slave slave, int productId)
{
	FILE* database = fopen(S_DATA, "r+b");

	fseek(database, slave.selfAddress, SEEK_SET);
	fwrite(&slave, S_SIZE, 1, database);
	fclose(database);

	return 1;
}

int DeleteSlave(struct Master master, struct Slave slave, int productId, char* error)
{
	FILE* database = fopen(S_DATA, "r+b");
	struct Slave previous;

	fseek(database, master.firstSlaveAddress, SEEK_SET);

	do
	{
		fread(&previous, S_SIZE, 1, database);
		fseek(database, previous.nextAddress, SEEK_SET);
	} while (previous.nextAddress != slave.selfAddress && slave.selfAddress != master.firstSlaveAddress);

	ReAddress(database, previous, slave, &master);
	NoteSlave(slave.selfAddress);

	slave.exists = 0;

	fseek(database, slave.selfAddress, SEEK_SET);
	fwrite(&slave, S_SIZE, 1, database);
	fclose(database);

	master.slavesCount--;
	UpdateMaster(master, error);

}

void PrintSlave(struct Slave slave, struct Master master)
{
	printf("Name: %d\n", slave.name);
	printf("Age: %d\n", slave.age);
}

void ReadSlave(struct Slave* slave)
{
	int x;
	char name[50];
	printf("Input name: ");
	scanf("%s", &name);

	strcpy(slave->name, name);

	printf("Input age: ");
	scanf("%d", &x);

	slave->age = x;
}