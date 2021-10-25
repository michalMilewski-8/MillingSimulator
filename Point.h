#pragma once
#include "Object.h"
#include "Line.h"
#include "Block.h"

struct StampPoint {
	StampPoint(int x_, int y_, float z_) : x(x_), y(y_), z_diff(z_) {};
	int x;
	int y;
	float z_diff;
};

enum StampType {
Flat,
Round
};

class Point :
	public Object
{
public:
	Point(glm::vec3 position, Shader sh);
	void InitializeDrilledMaterial(glm::vec3 size, glm::uvec2 points_denisity, glm::vec3 starting_pos = { 0,0,0 });

	void MoveTool(float delta_time, float speed);
	void DrillAll();
	void SetViewPos(glm::vec3 view_pos);
	void SetStampType(StampType type) { stamp_type = type; UpdateDrillSize(drill_size_real); };
	void SetStampType(StampType type, float new_size) { stamp_type = type; UpdateDrillSize(new_size); };

	void DrawObject(glm::mat4 mvp) override;

	std::unique_ptr<Line> current_path;
	std::unique_ptr<Block> current_block;

	void UpdateDrillSize(float new_size);
	float GetDrillSize();
private:
	void update_object() override;
	void drill_path(glm::vec3 start, glm::vec3 end);
	void drill(glm::vec3 drill_point);

	float drill_size_real = 10.0f;
	glm::uvec2 drill_size_divisions = {1,1};
	StampType stamp_type = StampType::Flat;

	std::vector<StampPoint> stamp = {};
};

