#ifndef HEADER_MEM_
#define HEADER_MEM_
#include <windows.h>
#include <string>
#include "Vectors.h"
#endif

//Struct to hold player info
struct PlayerInfo {
	DWORD		Health;
	Vector3		Position;
	Vector3		w2s;
	Vector3		w2sHead;
	BYTE		TeamId;
	std::string	Name;
};

class Memory {



private:
	//Local Name --- user pseudo go HERE !!
	std::string LocalName = "OnlyBoosted";

	//BaseAddress
	uint64_t BaseAddress;

	//Game
	DWORD_PTR pGameManager;
	DWORD_PTR pEntityList;

	//Camera
	DWORD_PTR pRender;
	DWORD_PTR pGameRender;
	DWORD_PTR pEngineLink;
	DWORD_PTR pEngine;
	DWORD_PTR pCamera;

public:
	template <typename Type, typename Base, typename Offset>
	static inline Type Ptr(Base base, Offset offset)
	{
		static_assert(std::is_pointer<Type>::value || std::is_integral<Type>::value, "Type must be a pointer or address");
		static_assert(std::is_pointer<Base>::value || std::is_integral<Base>::value, "Base must be a pointer or address");
		static_assert(std::is_pointer<Offset>::value || std::is_integral<Offset>::value, "Offset must be a pointer or address");

		return base ? reinterpret_cast<Type>((reinterpret_cast<uint64_t>(base) + static_cast<uint64_t>(offset))) : nullptr;
	}

	template <typename Type>
	static bool IsValidPtr(Type* ptr)
	{
		return (ptr && sizeof(ptr)) ? true : false;
	}

	static bool IsValidPtr(void* ptr)
	{
		return (ptr && sizeof(ptr)) ? true : false;
	}

	//Hard coded display size cause lazy
	int displayWidth = GetSystemMetrics(SM_CXSCREEN);
	int displayHeight = GetSystemMetrics(SM_CYSCREEN);

	Memory();

	BOOL SetBaseAddress();

	//Just casts base address to a DWORD_PTR
	uint64_t GetBaseAddress();

	//Simple method to update all top-level addresses
	//Just in case something changes
	void UpdateAddresses();

	//Method to get i entity from the entity list
	//Just doest EntityList] + i * 0x0008
	DWORD_PTR GetEntity(int i);

	//method to return a pointer to the local entity
	DWORD_PTR GetLocalEntity();

	//Method to get an entity's health
	//Takes pointer to an entity as param
	//Returns health value as a DWORD (120 max health for pvp cause downed state)
	DWORD GetEntityHealth(DWORD_PTR entity);

	//Method to get entity's feet position
	//Takes pointer to an entity as param
	//Returns a Vector3 of the entity's 3D coordinates at their feet
	Vector3 GetEntityFeetPosition(DWORD_PTR entity);

	//Method to get an entity's head position
	//Takes pointer to an entity as param
	//Retunrs Vector3 of the entity's 3D coordiantes at their head
	Vector3 GetEntityHeadPosition(DWORD_PTR entity);

	//Method to get an entity's neck position
	//Takes pointer to an entity as param
	//Retunrs Vector3 of the entity's 3D coordiantes at their neck
	Vector3 GetEntityNeckPosition(DWORD_PTR entity);

	//Method to get an entity's name
	std::string GetEntityPlayerName(DWORD_PTR entity);

	//Method to get an entities team id
	BYTE GetEntityTeamId(DWORD_PTR entity);

	//Method that gathers all the possible info about a player
	//Returns a PlayerInfo struct
	//Makes life easier
	//If you think this is a performance issues then fix it, but it really isn't
	PlayerInfo GetAllEntityInfo(DWORD_PTR entity);

	//Method to get your view translation
	//Returns Vector3 of the view translation
	//Used for World2Screen stuff
	Vector3 GetViewTranslation();

	//Method to get your view right
	//Returns Vector3 of your view right
	//Used for World2Screen stuff
	Vector3 GetViewRight();

	//Method to get your view up
	//Returns Vector3 of your view up
	//USed for Wolrd2Screen stuff
	Vector3 GetViewUp();

	//Method to get your view forward
	//Returns Vector3 of your view forward
	//Used for World2Screen stuff
	Vector3 GetViewForward();

	//Method to get your current FOV X
	//Returns float of your FOV on the X axis
	//Used for Wolrd2Screen stuff
	float GetFOVX();

	//Method to get your current FOV Y
	//Returns float of your FOV on the Y axis
	//Used for World2Screen stuff
	float GetFOVY();

	//World 2 screen function
	//Translates 3d coordinates to 2d screen positions
	//Use that screen position to draw stuff
	Vector3 WorldToScreen(Vector3 position);
};