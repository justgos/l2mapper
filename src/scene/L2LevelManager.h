#pragma once

#include "../_afx.h"

struct L2MapTileInfo
{
	uint16 map_x;
	uint16 map_y;
};

class L2LevelManager
{
public:
	void Init();

	void loadLevel(int map_x, int map_y);
	void loadTiles(jfArray<L2MapTileInfo*, uint16> *tiles);
	void setLevelVisible(int map_x, int map_y, bool value);

	void update();

	L2LibVarsHolder *getVars() { return varsHolder; };

	AActor *pickActor(_vector3 orig, _vector3 dir, float *dist);
	UBspNode *pickBspNode(_vector3 orig, _vector3 dir, UModel **model, float *dist);
protected:
	static void loadTile(void *arg);
protected:
	jfArray<ULevel*, uint32> m_Maps;
	uint16 loadingTilesCounter;

	L2LibVarsHolder *varsHolder;
public:
	L2LevelManager(void);
	~L2LevelManager(void);
};

