#include "Block.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

glm::vec3 Block::TransformToDivisions(glm::vec3 pos) const
{
	return { std::round((pos.y / (double)y_size + 0.5f) * (y_divisions - 1)), std::round((pos.x / (double)x_size + 0.5f) * (x_divisions - 1)),pos.z };
}

void Block::SetViewPos(glm::vec3 view_pos)
{
	this->view_pos = view_pos;
}

void Block::Update()
{
	need_update = true;
}

float Block::GetHeight(int x, int y)
{
	if (x >= 0 && x < x_divisions && y >= 0 && y < x_divisions)
		return data[x * x_divisions + y];
	else
		return -100.0f;
}

void Block::SetHeight(int x, int y, float val)
{
	if (x >= 0 && x < x_divisions && y >= 0 && y < x_divisions)
		data[x * x_divisions + y] = val;
}

void Block::create_block_points()
{
	x_delta = x_size / (x_divisions - 1);
	y_delta = y_size / (y_divisions - 1);

	float current_x = -x_size / 2.0f;
	float current_y = -y_size / 2.0f;

	for (int i = 0; i < x_divisions; i++) {
		// Point xyz
		points.push_back(current_x);
		points.push_back(current_y);
		points.push_back(0.0f);
		// Normal xyz
		points.push_back(0.0f);
		points.push_back(-1.0f);
		points.push_back(0.0f);

		points.push_back(current_x / x_size + 0.5f);
		points.push_back(0.0f);


		// second_point

		points.push_back(current_x);
		points.push_back(current_y);
		points.push_back(-784.0f);
		// Normal xyz
		points.push_back(0.0f);
		points.push_back(-1.0f);
		points.push_back(0.0f);

		points.push_back(current_x / x_size + 0.5f);
		points.push_back(1.0f);
		current_x += x_delta;
	}
	current_x = -x_size / 2.0f;
	current_y = y_size / 2.0f;
	for (int i = 0; i < x_divisions; i++) {
		// Point xyz
		points.push_back(current_x);
		points.push_back(current_y);
		points.push_back(0.0f);
		// Normal xyz
		points.push_back(0.0f);
		points.push_back(1.0f);
		points.push_back(0.0f);

		points.push_back(current_x / x_size + 0.5f);
		points.push_back(0.0f);


		// second_point

		points.push_back(current_x);
		points.push_back(current_y);
		points.push_back(-784.0f);
		// Normal xyz
		points.push_back(0.0f);
		points.push_back(1.0f);
		points.push_back(0.0f);

		points.push_back(current_x / x_size + 0.5f);
		points.push_back(1.0f);

		current_x += x_delta;
	}
	current_x = -x_size / 2.0f;
	current_y = -y_size / 2.0f;
	for (int i = 0; i < y_divisions; i++) {
		// Point xyz
		points.push_back(current_x);
		points.push_back(current_y);
		points.push_back(0.0f);
		// Normal xyz
		points.push_back(-1.0f);
		points.push_back(0.0f);
		points.push_back(0.0f);

		points.push_back(current_y / y_size + 0.5f);
		points.push_back(0.0f);


		// second_point

		points.push_back(current_x);
		points.push_back(current_y);
		points.push_back(-784.0f);
		// Normal xyz
		points.push_back(-1.0f);
		points.push_back(0.0f);
		points.push_back(0.0f);

		points.push_back(current_y / y_size + 0.5f);
		points.push_back(1.0f);

		current_y += y_delta;
	}
	current_x = x_size / 2.0f;
	current_y = -y_size / 2.0f;
	for (int i = 0; i < y_divisions; i++) {
		// Point xyz
		points.push_back(current_x);
		points.push_back(current_y);
		points.push_back(0.0f);
		// Normal xyz
		points.push_back(1.0f);
		points.push_back(0.0f);
		points.push_back(0.0f);

		points.push_back(current_y / y_size + 0.5f);
		points.push_back(0.0f);



		// second_point

		points.push_back(current_x);
		points.push_back(current_y);
		points.push_back(-784.0f);
		// Normal xyz
		points.push_back(1.0f);
		points.push_back(0.0f);
		points.push_back(0.0f);

		points.push_back(current_y / y_size + 0.5f);
		points.push_back(1.0f);

		current_y += y_delta;
	}
	current_x = -x_size / 2.0f;
	current_y = -y_size / 2.0f;

	for (int i = 0; i < x_divisions; i++) {
		for (int j = 0; j < y_divisions; j++) {

			points.push_back(current_x);
			points.push_back(current_y);
			points.push_back(-784.0f);
			// Normal xyz
			points.push_back(0.0f);
			points.push_back(0.0f);
			points.push_back(-190.0f);

			points.push_back(current_x / x_size + 0.5f);
			points.push_back(current_y / y_size + 0.5f);
			current_y += y_delta;
		}
		current_y = -y_size / 2.0f;
		current_x += x_delta;
	}

	// Indexing triangles
	for (int i = 0; i < x_divisions - 1; i++) {
		quads.push_back(2 * i);
		quads.push_back(2 * i + 1);
		quads.push_back(2 * i + 2);

		quads.push_back(2 * i + 2);
		quads.push_back(2 * i + 1);
		quads.push_back(2 * i + 3);


		quads.push_back(2 * x_divisions + 2 * i);
		quads.push_back(2 * x_divisions + 2 * i + 1);
		quads.push_back(2 * x_divisions + 2 * i + 2);

		quads.push_back(2 * x_divisions + 2 * i + 2);
		quads.push_back(2 * x_divisions + 2 * i + 1);
		quads.push_back(2 * x_divisions + 2 * i + 3);

	}

	for (int i = 0; i < y_divisions - 1; i++) {
		quads.push_back(4 * x_divisions + 2 * i);
		quads.push_back(4 * x_divisions + 2 * i + 1);
		quads.push_back(4 * x_divisions + 2 * i + 2);

		quads.push_back(4 * x_divisions + 2 * i + 2);
		quads.push_back(4 * x_divisions + 2 * i + 1);
		quads.push_back(4 * x_divisions + 2 * i + 3);

		quads.push_back(4 * x_divisions + 2 * y_divisions + 2 * i);
		quads.push_back(4 * x_divisions + 2 * y_divisions + 2 * i + 1);
		quads.push_back(4 * x_divisions + 2 * y_divisions + 2 * i + 2);

		quads.push_back(4 * x_divisions + 2 * y_divisions + 2 * i + 2);
		quads.push_back(4 * x_divisions + 2 * y_divisions + 2 * i + 1);
		quads.push_back(4 * x_divisions + 2 * y_divisions + 2 * i + 3);

	}

	for (int i = 0; i < x_divisions - 1; i++) {
		for (int j = 0; j < y_divisions - 1; j++) {
			quads.push_back(4 * x_divisions + 4 * y_divisions + (i + 0) * x_divisions + (j + 0));
			quads.push_back(4 * x_divisions + 4 * y_divisions + (i + 0) * x_divisions + (j + 1));
			quads.push_back(4 * x_divisions + 4 * y_divisions + (i + 1) * x_divisions + (j + 1));

			quads.push_back(4 * x_divisions + 4 * y_divisions + (i + 1) * x_divisions + (j + 1));
			quads.push_back(4 * x_divisions + 4 * y_divisions + (i + 1) * x_divisions + (j + 0));
			quads.push_back(4 * x_divisions + 4 * y_divisions + (i + 0) * x_divisions + (j + 0));

		}
	}
	blocks_need_creation = false;
}

void Block::update_object()
{
	if (blocks_need_creation) {
		quads.clear();
		points.clear();

		create_block_points();

		shader.use();
		// 1. bind Vertex Array Object
		glBindVertexArray(VAO);
		// 2. copy our vertices array in a vertex buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), points.data(), GL_DYNAMIC_DRAW);
		// 3. copy our index array in a element buffer for OpenGL to use
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * quads.size(), quads.data(), GL_DYNAMIC_DRAW);
		// 4. then set the vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		int x_deltaPos = glGetUniformLocation(shader.ID, "x_delta");
		glUniform1f(x_deltaPos, x_delta);

		int y_deltaPos = glGetUniformLocation(shader.ID, "y_delta");
		glUniform1f(y_deltaPos, y_delta);

		int x_sizePos = glGetUniformLocation(shader.ID, "x_size");
		glUniform1f(x_sizePos, x_size);

		int y_sizePos = glGetUniformLocation(shader.ID, "y_size");
		glUniform1f(y_sizePos, y_size);

		int z_sizePos = glGetUniformLocation(shader.ID, "z_size");
		glUniform1f(z_sizePos, z_size);

		// wood texture binding
		auto texLocation = glGetUniformLocation(shader.ID, "wood_texture");
		glUniform1i(texLocation, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);


		create_heighmap();

		blocks_need_creation = false;
	}
	generate_texture();
}

void Block::create_heighmap()
{
	//double tx = 0.0f;
	//for (int i = 0; i < x_divisions; i++) {
	//	height_map.push_back(std::vector<float>());
	//	double t = 0.0f;
	//	for (int j = 0; j < y_divisions; j++) {
	//		height_map[i].push_back(0.9f * z_size + std::cos(t) * 0.1f * z_size + std::cos(tx) * 0.1f * z_size);
	//		t += 0.03141592653 * 2.3;
	//		//height_map[i].push_back(z_size);
	//	}
	//	tx += 0.03141592653 * 2.3;
	//}
	for (int i = 0; i < x_divisions; i++) {
		for (int j = 0; j < y_divisions; j++) {
			//data.push_back(height_map[i][j]);
			data.push_back(z_size);
		}
	}
}

void Block::generate_texture()
{
	glDeleteTextures(1, &textureID);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// load and generate the texture
	//data.clear();
	//for (int i = 0; i < height_map.size(); i++) {
	//	for (int j = 0; j < height_map[i].size(); j++) {
	//		data.push_back(height_map[i][j]);
	//	}
	//}
	if (data.size() > 0)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, x_divisions, y_divisions, 0, GL_RED, GL_FLOAT, data.data());
		glGenerateMipmap(GL_TEXTURE_2D);
	}

}


Block::Block(float x_size_, float y_size_, float z_size_, int x_divisions_, int y_divisions_, Shader sh) :
	Object(sh, 8)
{
	x_size = x_size_;
	y_size = y_size_;
	z_size = z_size_;
	x_divisions = x_divisions_;
	y_divisions = y_divisions_;

	shader = Shader("shader_tex.vs", "shader_tex.fs");
	update_object();
	shader.use();
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load("texture.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

void Block::DrawObject(glm::mat4 mvp)
{
	if (need_update) {
		update_object();
		need_update = false;
	}

	Object::DrawObject(mvp);

	glm::mat4 model = translate * rotate * resize;
	glm::mat4 vp = mvp;
	shader.use();
	glm::mat4 trmodel = glm::transpose(glm::inverse(model));
	int projectionLoc = glGetUniformLocation(shader.ID, "model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(model));

	int trmodelLoc = glGetUniformLocation(shader.ID, "trmodel");
	glUniformMatrix4fv(trmodelLoc, 1, GL_FALSE, glm::value_ptr(trmodel));

	int mvLoc = glGetUniformLocation(shader.ID, "vp");
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(vp));

	int viewPos = glGetUniformLocation(shader.ID, "viewPos");
	glUniform3fv(viewPos, 1, &view_pos[0]);

	// height binding
	int texLocation = glGetUniformLocation(shader.ID, "height_texture");
	glUniform1i(texLocation, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, quads.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
