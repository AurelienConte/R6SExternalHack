#pragma once
#include "Memory.h"
#include "Offsets.h"
#include "Vectors.h"
#include <Windows.h>
#include <Psapi.h>
#include "ABClient.h"
#include "Util.h"

using namespace std;
using namespace asmjs;

extern Memory;

Memory::Memory() {};
ABClient abc;

BOOL Memory::SetBaseAddress()
{
	try {

		static vector<uint64_t> ProcessIds = Util::GetProcessIdsByName("RainbowSix.exe");
		printf("Process need to been run as admin ! \n");
		printf("Youn need to have notepadd installed.. and not started !\n");
		printf("Connecting to Pipe...\n");
		abc.Connect();
		printf("Connected. Pinging....\n");
		abc.Ping();
		system("pause");
		printf("Looking for process R6S ...\n");
		abc.AccuireProcessHandle(ProcessIds[0], 0x1478 /* LSASS set */);
		printf("Get moduleBase of R6S...\n");
		BaseAddress = abc.GetProcessModuleBase("RainbowSix.exe");
		printf("Base Retrieved !\n");
	}
	catch (exception e)
	{
		printf("Exception occured: %s (0x%X)\n", e.what(), GetLastError());
	}
	return (true);
}

uint64_t Memory::GetBaseAddress() {
	return BaseAddress;
}

void Memory::UpdateAddresses() {
	//Game manager pointer from games base address + the GameManager offset
	pGameManager = abc.RPM<DWORD_PTR>(GetBaseAddress() + ADDRESS_GAMEMANAGER);
	//Entity list pointer from the GameManager + EntityList offset
	pEntityList = abc.RPM<DWORD_PTR>(pGameManager + OFFSET_GAMEMANAGER_ENTITYLIST);

	//Renderer pointer from games base address + Renderer offset
	pRender = abc.RPM<DWORD_PTR>(GetBaseAddress() + ADDRESS_GAMERENDERER);
	//Game Renderer pointer from Renderer + GameRenderer offset
	pGameRender = abc.RPM<DWORD_PTR>(pRender + OFFSET_GAMERENDERER_DEREF);
	//EngineLink pointer from GameRenderer + EngineLink offset
	pEngineLink = abc.RPM<DWORD_PTR>(pGameRender + OFFSET_GAMERENDERER_ENGINELINK);
	//Engine pointer from EngineLink + Engine offset
	pEngine = abc.RPM<DWORD_PTR>(pEngineLink + OFFSET_ENGINELINK_ENGINE);
	//Camera pointer from Engine + Camera offset
	pCamera = abc.RPM<DWORD_PTR>(pEngine + OFFSET_ENGINE_CAMERA);
}

DWORD_PTR Memory::GetEntity(int i) {
	DWORD_PTR entityBase = abc.RPM<DWORD_PTR>(pEntityList + (i * OFFSET_ENTITY));
	return abc.RPM<DWORD_PTR>(entityBase + OFFSET_ENTITY_REF);
}

DWORD_PTR Memory::GetLocalEntity() {
	//Loop through the first 12
	for (int i = 0; i < 12; i++) {
		//get current entity
		DWORD_PTR entity = GetEntity(i);
		//get that entity's name
		std::string entityName = GetEntityPlayerName(entity);

		//check it against our local name
		if (strcmp(entityName.c_str(), LocalName.c_str()) == 0) {
			return entity;
		}
	}

	//return the first entity if we didn't find anything
	return GetEntity(0);
}

DWORD Memory::GetEntityHealth(DWORD_PTR entity) {
	//Entity info pointer from the Entity
	DWORD_PTR EntityInfo = abc.RPM<DWORD_PTR>(entity + OFFSET_ENTITY_ENTITYINFO);
	//Main component pointer from the entity info
	DWORD_PTR MainComponent = abc.RPM<DWORD_PTR>(EntityInfo + OFFSET_ENTITYINFO_MAINCOMPONENT);
	//Child component pointer form the main component
	DWORD_PTR ChildComponent = abc.RPM<DWORD_PTR>(MainComponent + OFFSET_MAINCOMPONENT_CHILDCOMPONENT);

	//Finally health from the child component
	return abc.RPM<DWORD>(ChildComponent + OFFSET_CHILDCOMPONENT_HEALTH);
}

Vector3 Memory::GetEntityFeetPosition(DWORD_PTR entity) {
	//We get the feet position straight from the entity
	return abc.RPM<Vector3>(entity + OFFSET_ENTITY_FEET);
}

Vector3 Memory::GetEntityHeadPosition(DWORD_PTR entity) {
	//We get the head position straight from the entity
	return abc.RPM<Vector3>(entity + OFFSET_ENTITY_HEAD);
}

Vector3 Memory::GetEntityNeckPosition(DWORD_PTR entity) {
	//We get the head position straight from the entity
	return abc.RPM<Vector3>(entity + OFFSET_ENTITY_NECK);
}

std::string Memory::GetEntityPlayerName(DWORD_PTR entity) {
	DWORD_PTR playerInfo = abc.RPM<DWORD_PTR>(entity + OFFSET_ENTITY_PLAYERINFO);
	DWORD_PTR playerInfoD = abc.RPM<DWORD_PTR>(playerInfo + OFFSET_ENTITY_PLAYERINFO_DEREF);

	return abc.RPMString(abc.RPM<DWORD_PTR>(playerInfoD + OFFSET_PLAYERINFO_NAME) + 0x0);
}

BYTE Memory::GetEntityTeamId(DWORD_PTR entity) {
	//Team id comes from player info
	DWORD_PTR playerInfo = abc.RPM<DWORD_PTR>(entity + OFFSET_ENTITY_PLAYERINFO);
	//We have to derefrnce it as 0x0
	DWORD_PTR playerInfoD = abc.RPM<DWORD_PTR>(playerInfo + OFFSET_ENTITY_PLAYERINFO_DEREF);

	return abc.RPM<BYTE>(playerInfoD + OFFSET_PLAYERINFO_TEAMID);
}

PlayerInfo Memory::GetAllEntityInfo(DWORD_PTR entity) {
	PlayerInfo p;

	p.Health = GetEntityHealth(entity);
	p.Name = GetEntityPlayerName(entity);
	p.Position = GetEntityFeetPosition(entity);
	p.w2s = WorldToScreen(p.Position);
	p.w2sHead = WorldToScreen(GetEntityHeadPosition(entity));
	p.TeamId = GetEntityTeamId(entity);

	return p;
}

Vector3 Memory::GetViewTranslation() {
	//View translation comes straight from the camera
	return abc.RPM<Vector3>(pCamera + OFFSET_CAMERA_VIEWTRANSLATION);
}

Vector3 Memory::GetViewRight() {
	//View right comes directly from the camera
	return abc.RPM<Vector3>(pCamera + OFFSET_CAMERA_VIEWRIGHT);
}

Vector3 Memory::GetViewUp() {
	//View up comes directly from the camera
	return abc.RPM<Vector3>(pCamera + OFFSET_CAMERA_VIEWUP);
}

Vector3 Memory::GetViewForward() {
	//View forward comes directly from the camera
	return abc.RPM<Vector3>(pCamera + OFFSET_CAMERA_VIEFORWARD);
}

float Memory::GetFOVX() {
	//FOV comes directly from the camera
	return abc.RPM<float>(pCamera + OFFSET_CAMERA_VIEWFOVX);
}

float Memory::GetFOVY() {
	//FOV comes directly from the camera
	return abc.RPM<float>(pCamera + OFFSET_CAMERA_VIEWFOVY);
}

Vector3 Memory::WorldToScreen(Vector3 position) {
	Vector3 temp = position - GetViewTranslation();

	float x = temp.Dot(GetViewRight());
	float y = temp.Dot(GetViewUp());
	float z = temp.Dot(GetViewForward() * -1);

	return Vector3((displayWidth / 2) * (1 + x / GetFOVX() / z), (displayHeight / 2) * (1 - y / GetFOVY() / z), z);
}