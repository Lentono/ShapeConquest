#include "GlobalStructures.h"
#include <iomanip>
float GlobalStructures::ScreenBoundsX;
float GlobalStructures::ScreenBoundsY;
float GlobalStructures::SpawnGridCellSize;
std::vector<std::shared_ptr<GlobalStructures::Row>> GlobalStructures::m_spawnCellGridRow;

void GlobalStructures::CreateSpawnGrid()
{
	const int  x = (((ScreenBoundsX * 2)) - (SpawnGridCellSize)) / SpawnGridCellSize;
	const int y = ((ScreenBoundsY * 2) - (SpawnGridCellSize)) / SpawnGridCellSize;

	const auto offset = (ScreenBoundsX * 2) / (x);
	SpawnGridCellSize = offset;
	for (auto i = 0; i < y; i++)
	{
		std::shared_ptr<Row> newRow = std::make_shared<Row>();
		for (auto j = 0; j < x; j++)
		{
			auto newCell = std::make_shared<SpawnCell>();
			newCell->position = glm::vec3((j * offset) + (offset / 2) - ScreenBoundsX, (i * offset) + (offset / 2) - ScreenBoundsY, 0);
			newCell->blocked = false;
			newRow->row.push_back(newCell);
		}
		m_spawnCellGridRow.push_back(newRow);
	}


}

std::shared_ptr<GlobalStructures::SpawnCell> GlobalStructures::GetSpawnCell(const bool ignoreBlocked)
{
	auto count = 0;
	while (count < 100) // 100 deemed a reasonable amount of cells to check
	{
		count++;
		const auto randNumY = rand() % ((m_spawnCellGridRow.size() - 1) - 0 + 1) + 0;
		const auto randNumX = rand() % ((m_spawnCellGridRow.at(randNumY)->row.size() - 1) - 0 + 1) + 0;
		if (!m_spawnCellGridRow.at(randNumY)->row.at(randNumX)->blocked || ignoreBlocked)
		{
			return m_spawnCellGridRow.at(randNumY)->row.at(randNumX);
		}
	}
	return nullptr;
}

std::shared_ptr<GlobalStructures::SpawnCell> GlobalStructures::GetSpawnCellNearLocation(const glm::vec3& position, const bool ignoreBlocked /*= false*/)
{

	int xIndex = glm::abs(-ScreenBoundsX - position.x) / SpawnGridCellSize;
	int yIndex = glm::abs(-ScreenBoundsY - position.y) / SpawnGridCellSize;
	if (ignoreBlocked)
	{
		return m_spawnCellGridRow.at(yIndex)->row.at(xIndex);
	}
	else
	{
		if (yIndex < m_spawnCellGridRow.size())
		{
			if (xIndex < m_spawnCellGridRow.at(yIndex)->row.size())
			{
				if (!m_spawnCellGridRow.at(yIndex)->row.at(xIndex)->blocked)
				{
					return m_spawnCellGridRow.at(yIndex)->row.at(xIndex);
				}
				else
				{
					std::shared_ptr<SpawnCell> spawnCell = nullptr;
					auto spawnCellNotFound = true;
					unsigned int iteration = 8;
					unsigned int transitionDir = 2;

					do
					{
						xIndex -= SpawnGridCellSize;
						yIndex += SpawnGridCellSize;

						for (unsigned int i = 0; i < iteration; i++)
						{
							if (yIndex <= m_spawnCellGridRow.size())
							{
								if (xIndex <= m_spawnCellGridRow.at(yIndex)->row.size())
								{
									if (!m_spawnCellGridRow.at(yIndex)->row.at(xIndex)->blocked)
									{
										spawnCell = m_spawnCellGridRow.at(yIndex)->row.at(xIndex);
										spawnCellNotFound = false;
									}
								}
							}

							if (i < transitionDir * 1)
							{
								xIndex += SpawnGridCellSize;
							}
							else if (i < transitionDir * 2)
							{
								yIndex -= SpawnGridCellSize;
							}
							else if (i < transitionDir * 3)
							{
								xIndex -= SpawnGridCellSize;
							}
							else if (i < transitionDir * 4)
							{
								yIndex += SpawnGridCellSize;
							}
						}

						iteration += 8;
						transitionDir = iteration / 4;
					} while (spawnCellNotFound);

					return spawnCell;
				}
			}
		}
	}
}