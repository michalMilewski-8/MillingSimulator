#include "Point.h"

Point::Point(glm::vec3 position, Shader sh) :Object(sh, 7)
{
	this->color = glm::vec4(54 / 255.0f, 134 / 255.0f, 199 / 255.0f, 1.0f);
	current_path = std::make_unique<Line>(sh);
	update_object();
	MoveObject(position);
}

void Point::InitializeDrilledMaterial(glm::vec3 size, glm::uvec2 points_denisity, glm::vec3 starting_pos)
{
	if (size.x <= 0.0f || size.y <= 0.0f || size.z <= 0.0f || points_denisity.x < 2 || points_denisity.y < 2) return;
	current_block = std::make_unique<Block>(size.x, size.y, size.z, points_denisity.x, points_denisity.y, shader);
	UpdateDrillSize(drill_size_real);
}

ErrorRaport Point::MoveTool(float delta_time, float speed)
{
	if (!current_block) return {};
	if (!current_path->IsValidPath()) return {};
	float movement_dist = speed * delta_time;
	ErrorRaport error;
	while (movement_dist > 0.0 && current_path->IsValidPath()) {
		//float movement_dist = std::min(0.1f, movement_dist_r);
		glm::vec3 movement_to_perform = current_path->GetSecondPoint() - GetPosition();
		float dist = glm::length(movement_to_perform);
		if (dist <= movement_dist) {
			error = drill_path(GetPosition(), current_path->GetSecondPoint());
			if (error.is_error) return error;
			MoveObject(movement_to_perform);
			movement_dist -= dist;
			current_path->DeleteFirstPoint();
		}
		else {
			auto new_position = GetPosition() + movement_to_perform * (movement_dist / dist);
			error = drill_path(GetPosition(), new_position);
			if (error.is_error) return error;
			MoveObjectTo(new_position);
			movement_dist -= dist;
		}
		//movement_dist_r -= movement_dist;
	}
	current_block->Update();
	return {};
}

ErrorRaport Point::DrillAll()
{
	if (!current_block) return {};
	if (!current_path->IsValidPath()) return {};
	ErrorRaport error;
	int length = current_path->size();
	int done = 0;
	int last_percent = 0;
	int percent;
	while (current_path->IsValidPath()) {
		error = drill_path(GetPosition(), current_path->GetSecondPoint());
		if (error.is_error) return error;
		MoveObjectTo(current_path->GetSecondPoint());
		current_path->DeleteFirstPoint();
		done++;
		percent = (done / (float)length) * 100;
		if (last_percent != percent) {
			std::cout << "Progress: " << percent <<"% done"<< std::endl;
			last_percent = percent;
		}
	}
	current_block->Update();
	return {};
}

void Point::SetViewPos(glm::vec3 view_pos)
{
	if (!!current_block)
		current_block->SetViewPos(view_pos);
}

void Point::DrawObject(glm::mat4 mvp)
{
	Object::DrawObject(mvp);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPointSize(5);
	glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	if (!!current_block)
		current_block->DrawObject(mvp);
}

void Point::UpdateDrillSize(float new_size)
{
	drill_size_real = new_size;
	if (!current_block) return;
	stamp.clear();
	drill_size_divisions = {
		std::max(new_size * current_block->GetXDivisions() / current_block->GetXSize(),1.0f) ,
		std::max(new_size * current_block->GetYDivisions() / current_block->GetYSize(),1.0f) };

	float new_size_sq = new_size * new_size / 4.0f;
	float half_new_size = new_size / 2.0f;
	float x_diff_to_real = current_block->GetXSize() / current_block->GetXDivisions();
	float y_diff_to_real = current_block->GetYSize() / current_block->GetYDivisions();
	for (int i = -(int)(drill_size_divisions.x / 2.0f); i <= (int)(drill_size_divisions.x / 2.0f); i++) {
		float x_real = i * x_diff_to_real;
		float x_real_sq = x_real * x_real;
		for (int j = -(int)(drill_size_divisions.y / 2.0f); j <= (int)(drill_size_divisions.y / 2.0f); j++) {
			float y_real = j * y_diff_to_real;
			float y_real_sq = y_real * y_real;
			if (new_size_sq >= y_real_sq + x_real_sq) {
				if (stamp_type == StampType::Flat) {
					stamp.push_back({ i,j,0.0f });
				}
				else {
					stamp.push_back({ i,j,half_new_size - std::sqrtf(new_size_sq - (y_real_sq + x_real_sq)) });
				}
			}

		}
	}

}

float Point::GetDrillSize()
{
	return drill_size_real;
}

void Point::update_object()
{
	unsigned int pointt = 0;
	float points[7] = { 0,0,0,color.r,color.g,color.b,1 };

	if (selected) {
		points[3] = 1;
		points[4] = 0;
		points[5] = 0;
	}

	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 7, points, GL_DYNAMIC_DRAW);
	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int), &pointt, GL_DYNAMIC_DRAW);
	// 4. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

ErrorRaport Point::drill_path(glm::vec3 start, glm::vec3 end)
{
	//http://tech-algorithm.com/articles/drawing-line-using-bresenham-algorithm/
	ErrorRaport error;

	start = current_block->TransformToDivisions(start);
	end = current_block->TransformToDivisions(end);

	error = drill(start);
	if (error.is_error) return error;

	auto original_pos = start;
	int w = std::round(end.x - start.x);
	int h = std::round(end.y - start.y);

	int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;
	if (w < 0) dx1 = -1; else if (w > 0) dx1 = 1;
	if (h < 0) dy1 = -1; else if (h > 0) dy1 = 1;
	if (w < 0) dx2 = -1; else if (w > 0) dx2 = 1;
	int longest = std::abs(w);
	int shortest = std::abs(h);
	if (!(longest > shortest)) {
		longest = std::abs(h);
		shortest = std::abs(w);
		if (h < 0) dy2 = -1; else if (h > 0) dy2 = 1;
		dx2 = 0;
	}
	int numerator = longest >> 1;
	float z_diff = (end.z - start.z) / longest;
	for (int i = 0; i <= longest; i++) {
		error = drill({ start.x, start.y, start.z + z_diff * i });
		if (error.is_error) return error;
		numerator += shortest;
		if (!(numerator < longest)) {
			numerator -= longest;
			start.x += dx1;
			start.y += dy1;
		}
		else {
			start.x += dx2;
			start.y += dy2;
		}
	}
	error = drill(end);
	if (error.is_error) return error;
	return {};
}

ErrorRaport Point::drill(glm::vec3 drill_point)
{
	int x = drill_point.x;
	int y = drill_point.y;
	float z = drill_point.z;

	float diff, x_n, y_n, z_n, current_h;


	for (int i = 0; i < stamp.size(); i++) {
		x_n = stamp[i].x + x;
		y_n = stamp[i].y + y;
		z_n = stamp[i].z_diff + z;
		current_h = current_block->GetHeight(x_n, y_n);
		if (current_h > z_n) {
			if (current_h - drill_max_depth > z_n)
				return { "It's too deep senpai..." };
			if (z_n < block_min_height)
				return { "You can not make it that thin" };
			current_block->SetHeight(x_n, y_n, z_n);
		}
	}
	return {};
}

