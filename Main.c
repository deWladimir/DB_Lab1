#pragma warning(disable:6031)

#include <stdio.h>
#include <stdlib.h>
#include "InfoStructs.h"
#include "FilmMaster.h"
#include "ActorSlave.h"

int main()
{
	struct Master master;
	struct Slave slave;

	printf("Here we have\nMASTER means FILM, FILM has Name\nSLAVE means ACTOR, Actor has name and Age/n");

	printf("Press 1 to INSERT Master\nPress 2 to GET Master\nPress 3 to UPDATE Master\nPress 4 to DELETE Master\nPress 5 to INSERT Slave\nPress 6 to GET Slave\nPress 7 to UPDATE Slave\nPress 8 to DELETE Slave\nPress 9 to INFO\nPress 0 to QUIT\n");
	while (2)
	{
		int option;
		int id;
		char error[51];


		scanf("%d", &option);


		if (option == 1)
		{
			ReadMaster(&master);
			InsertMaster(master);
		}

		else if (option == 2)
		{
			printf("Input ID: ");
			scanf("%d", &id);
			GetMaster(&master, id, error) ? PrintMaster(master) : printf("Error: %s\n", error);
		}

		else if (option == 3)
		{
			printf("Input ID: ");
			scanf("%d", &id);

			master.id = id;

			ReadMaster(&master);
			UpdateMaster(master, error) ? printf("Success\n") : printf("Error: %s\n", error);
		}

		else if (option == 4)
		{
			printf("Input ID: ");
			scanf("%d", &id);
			DeleteMaster(id, error) ? printf("Success\n") : printf("Error: %s\n", error);
		}

		else if (option == 5)
		{
			printf("Input master ID: ");
			scanf("%d", &id);

			if (GetMaster(&master, id, error))
			{
				slave.masterId = id;
				printf("Input slave ID: ");
				scanf("%d", &id);

				slave.actorId = id;
				ReadSlave(&slave);
				InsertSlave(master, slave, error);
				printf("Success\n");
			}
			else
			{
				printf("Error: %s\n", error);
			}
		}

		else if (option == 6)
		{
			printf("Input master ID: ");
			scanf("%d", &id);

			if (GetMaster(&master, id, error))
			{
				printf("Input slave ID: ");
				scanf("%d", &id);
				GetSlave(master, &slave, id, error) ? PrintSlave(slave, master) : printf("Error: %s\n", error);
			}
			else
			{
				printf("Error: %s\n", error);
			}
		}

		else if (option == 7)
		{
			printf("Input master ID: ");
			scanf("%d", &id);

			if (GetMaster(&master, id, error))
			{
				printf("Input slave ID: ");
				scanf("%d", &id);

				if (GetSlave(master, &slave, id, error))
				{
					ReadSlave(&slave);
					updateSlave(slave, id, error);
					printf("Succes\n");
				}
				else
				{
					printf("Error: %s\n", error);
				}
			}
			else
			{
				printf("Error: %s\n", error);
			}
		}

		else if (option == 8)
		{
			printf("Input master ID: ");
			scanf("%d", &id);

			if (GetMaster(&master, id, error))
			{
				printf("Input slave ID: ");
				scanf("%d", &id);

				if (GetSlave(master, &slave, id, error))
				{
					DeleteSlave(master, slave, id, error);
					printf("Success\n");
				}
				else
				{
					printf("Error: %s\n", error);
				}
			}
			else
			{
				printf("Error: %s\n", error);
			}
		}

		else if (option == 9)
		{
			info();
		}

		else if (option == 0)
		{
			return 0;
		}

		else {
			printf("Invalid input, please try again\n");
		}

		
	}
	return 0;
}