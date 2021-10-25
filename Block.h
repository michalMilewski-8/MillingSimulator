#pragma once
#include "Object.h"

class Block :
    public Object
{
public:
    Block(float x_size_, float y_size_, float z_size_, int x_divisions_, int y_divisions_, Shader sh);

    void DrawObject(glm::mat4 mvp) override;

    int GetXDivisions() const { return x_divisions; }
    int GetYDivisions() const { return y_divisions; }
    float GetXSize() const { return x_size; }
    float GetYSize() const { return y_size; }

    glm::vec3 TransformToDivisions(glm::vec3 pos) const;

    void SetViewPos(glm::vec3 view_pos);
    void Update() override;
    float GetHeight(int x, int y);
    void SetHeight(int x, int y, float val);

private:
    void create_block_points();
    void update_object() override;
    void create_heighmap();
    void generate_texture();


    int x_divisions;
    int y_divisions;

    float x_size;
    float y_size;
    float z_size;

    std::vector<std::vector<float>> height_map;
    std::vector<float>data = {};

    glm::vec3 view_pos;
    bool blocks_need_creation = true;
    float x_delta;
    float y_delta;

    unsigned int texture;
    std::vector<float> points;
    std::vector<unsigned int> quads;
};

