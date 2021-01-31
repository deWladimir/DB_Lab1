#pragma once

struct Master
{
	int id;
	char name[32];
	long firstSlaveAddress;
	int slavesCount;
};

struct Slave
{
	int masterId;
	int actorId;
	int age;
	char name[50];
	int exists;
	long selfAddress;
	long nextAddress;
};

struct MasterIndexer
{
	int id;
	int address;
	int exists;
};