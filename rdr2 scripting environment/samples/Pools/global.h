	#pragma once

	#include "script.h"
	#include <string>
	#include <vector>
	#include <sstream>
	#include <algorithm>
	#include <math.h>
	#include <utility>
	#include "keyboard.h"

	using namespace std;

	Hash joaat(const char* text);
	float DFC(float p1, float p2, float p3, float p4, float p5, float p6, bool useZ = false);
	float DFC(Vector3 p1, Vector3 p2, bool useZ = false);
	int RINT(int p1, int p2);
	float RFLOAT(float p1, float p2);
	int CreateBlipForVolume(Hash style, Hash sprite, const char* blipName, Volume volume);
	int CreateBlipForPed(Hash style, Hash sprite, const char* blipName, Ped ped);
	int CreateBlipForCoords(Hash style, Hash sprite, const char* blipName, Vector3 coords);
	/*Can be combined/used in harmony with function below.
	getter function for the vehicle handle that a ped is in.*/
	Vehicle getVehicle(Ped ped);
	/*returns true or false if ped is in any vehicle*/
	bool inVehicle(Ped ped);
	Vector3 GetCoords(Ped ped);
	/*Simple ped creator function, returns the ped handle so that you have full control over all peds you spawn*/
	int CreatePed(Hash model, float posx, float posy, float posz, float heading = 0.f, int outfitVariation = 0);
	int CreatePed(Hash model, Vector3 pos, float heading = 0.f, int outfitVariation = 0);
	int CreatePedOnMount(Ped mount, Hash model, int seatIndex = -1, int outfitVariation = 0);
	int CreateVehicle(Hash model, float posx, float posy, float posz, float heading = 0.f);
	int CreateVehicle(Hash model, Vector3 pos, float heading = 0.f);
	int CreatePedInVehicle(Vehicle vehicle, Hash model, int seatIndex = -1, int outfitVariation = 0);

	/*
	Slightly modified version of SgtJoe's script kill code
	only difference is this can kill multiple scripts at once
	Example param -> std::vector<Hash> killscripts = {joaat("carmodydell"), joaat("beat_bear_trap"), joaat("campfire_gang")};
	*/
	void kill_scripts(std::vector<Hash> scripts);
	Prompt promptHandler(const char* promptText, Hash controlAction, int promptType);
	//credit TuffyTown (Halen84)
	void DisplayObjective(const char* objective);
	void DisplayMissionName(const char* name, int duration);
	void DisplayHelpText(const char* text, int duration);
	void DisplayLeftToast(const char* title, const char* subtitle, const char* textureDictionary, const char* textureName, int duration);
	Ped getClosestEnemy(float distance);
	void addPedToPlayerGroup(Ped ped);
	void createGroup(int& outGroup, int formation, float formationSeperation);
	/*TuffyTown (Halen84) Add Item To Inventory Functions https://www.rdr2mods.com/forums/topic/2139-addingremoving-items-to-inventory-using-natives/ */
	/*------------------------------------------------------------------------------------------------------------------------------------------------*/
	sGuid CreateNewGUID();
	sGuid GetPlayerInventoryItemGUID(Hash item, sGuid guid, Hash slotId);
	sGuid GetPlayerInventoryGUID();
	Hash GetItemGroup(Hash item);
	sSlotInfo GetItemSlotInfo(Hash item);
	bool AddItemWithGUID(Hash item, sGuid& guid, sSlotInfo& slotInfo, int quantity, Hash addReason);
	bool AddItemToInventory(Hash item, int quantity);
	bool RemoveItemFromInventory(int inventoryid, Hash item, int quanity, Hash removeReason);
	/*back to none inventory stuff*/
	Vector3 addVector3(const Vector3& v1, const Vector3& v2);
	Vector3 subVector3(const Vector3& v1, const Vector3& v2);
	Vector3 multiplyVector3(const Vector3& v1, const Vector3& v2);
	Vector3 divideVector3(const Vector3& v1, const Vector3& v2);
	std::string Vector3ToString(const Vector3& v);
	template<typename T, typename F>
	void forEach(const std::vector<T>& vec, F func);
	int GetMount(Ped ped);
	Hash GetWeapon(Ped ped);
	Hash GetBestWeapon(Ped ped);
	int GetPedTarget(Ped ped);
	Volume CreateVolumeBox(float posx, float posy, float posz, float size);
	Volume CreateVolumeBox(Vector3 pos, float size);
	Volume CreateVolumeSphere(float posx, float posy, float posz, float size);
	Volume CreateVolumeSphere(Vector3 pos, float size);
	//the ped parameter is the ped you want to dismount a horse or something.
	void DismountPed(Ped ped);
	void DismountAnyVehicle(Ped ped, int flags = ECF_WAIT_FOR_ENTRY_POINT_TO_BE_CLEAR);
	void AddWeaponToPed(Ped ped, Hash weaponHash, int ammoCount = 100, int attachPoint = WEAPON_ATTACH_POINT_HAND_PRIMARY, bool inHand = true);

