#pragma once
#include <glm/vec3.hpp>
#include <glm/integer.hpp>
#include <vector>
#include <memory>
enum class ShootMode
{
	RapidFire,
	Burst,
	SingleShot,
	TriShot,
	TriShotBurst,
	TriShotRapidFire,
	Last
};

enum class PowerUpType
{
	Shield,
	GravityBomb,
	ShootMode,
	Speed,
	Last
};
class GlobalStructures
{
public:
	GlobalStructures(const GlobalStructures& other) = delete;
	GlobalStructures(GlobalStructures&& other) = delete;
	~GlobalStructures() = default;

	GlobalStructures& operator = (const GlobalStructures& other) = delete;
	GlobalStructures& operator = (GlobalStructures&& other) = delete;

	static float ScreenBoundsX;
	static float ScreenBoundsY;
	static float SpawnGridCellSize;

	struct SpawnCell
	{
		glm::vec3 position = glm::vec3();
		bool blocked = false;
	};

	struct Row
	{
		std::vector<std::shared_ptr<SpawnCell>> row;
	};

	static void CreateSpawnGrid();

	static std::shared_ptr<SpawnCell> GetSpawnCell(const bool ignoreBlocked = false);
	static std::shared_ptr<SpawnCell> GetSpawnCellNearLocation(const glm::vec3& position, const bool ignoreBlocked = false);

	static std::vector<std::shared_ptr<Row>> m_spawnCellGridRow;

private:
	GlobalStructures() = default;
};

